#include <assert.h>
#include <roxlu/core/Log.h>
#include <webm/IVFReader.h>


IVFFrame::IVFFrame()
  :pts(0)
  ,offset(0)
  ,millis(0)
  ,size(0)
{
}

// --------------------------------------------------

IVFReader::IVFReader(vpx_read_cb readCB, void* readUser)
  :state(IVF_STATE_NONE)
  ,time_started(0)
  ,time_base(0)
  ,frame_dx(0)
  ,last_frame_dx(-1)
  ,rbuffer(1024 * 1024 * 5)
  ,num_frames_to_buffer(300)
  ,num_read_frames(0)
  ,fp(NULL)
  ,width(0)
  ,height(0)
  ,rate(0)
  ,scale(0)
  ,cb_read(readCB)
  ,cb_user(readUser)
  ,decoder(NULL)
{
}

IVFReader::~IVFReader() {
  shutdown();
}

bool IVFReader::open(std::string filename, bool datapath) {
  if(fp) {
    RX_ERROR(IVF_ERR_ALREADY_OPEN);
    return false;
  }

  if(datapath) {
    filename = rx_to_data_path(filename);
  }
 
  fp = fopen(filename.c_str(), "rb");

  if(!fp) {
    RX_ERROR(IVF_ERR_FOPEN, filename.c_str());
    fp = NULL;
    return false;
  }

  if(!readHeader()) {
    return false;
  }

  return true;
}

void IVFReader::setFPS(double fps) {
  double fps_millis = 1.0/fps;
  time_base = (fps_millis / 30000.0); // we assume you used avconv to create the ivf file
}

bool IVFReader::readHeader() {
  if(!fp) {
    RX_ERROR(IVF_ERR_HDR_FILE);
    return false;
  }

  if(fseek(fp, 0, SEEK_SET) != 0) {
    RX_ERROR(IVF_ERR_HDR_SEEK);
    return false;
  }

  unsigned char file_hdr[IVF_FILE_HDR_SIZE];
  if(!(fread(file_hdr, 1, IVF_FILE_HDR_SIZE, fp) == IVF_FILE_HDR_SIZE)) {
    RX_ERROR(IVF_ERR_READ_HEADER);
    fclose(fp);
    fp = NULL;
    return false;
  }

  if(file_hdr[0] != 'D' || file_hdr[1] != 'K' || file_hdr[2] != 'I' || file_hdr[3] != 'F') {
    RX_ERROR(IVF_ERR_INVALID_FILE);
    fclose(fp);
    fp = NULL;
    return false;
  }

  width = rx_get_le_u16(file_hdr+12);
  height = rx_get_le_u16(file_hdr+14);

  if(!width || !height) {
    RX_ERROR(IVF_ERR_INVALID_SIZE, width, height);
    fclose(fp);
    fp = NULL;
    return false;
  }

  rate = rx_get_le_u32(file_hdr+16);
  scale = rx_get_le_u32(file_hdr+20);
  num_frames = rx_get_le_u32(file_hdr+24);

  if(!time_base) {
    time_base = double(scale)/rate;
  }

  if(decoder) {
    delete decoder;
    decoder = NULL;
  }

  VPXSettings cfg;
  cfg.in_w = width;
  cfg.in_h = height;
  cfg.out_w = width;
  cfg.out_h = height;
  cfg.fps = 30; // @todo 
  cfg.cb_read = cb_read;
  cfg.cb_user = cb_user;

  decoder = new VPXDecoder();

  if(!decoder->setup(cfg)) {
    RX_ERROR(IVF_ERR_DECODER_SETUP);
    return false;
  }


  if(num_frames > num_frames_to_buffer) {
    num_frames_to_buffer = num_frames;
  }

  return true;
}

bool IVFReader::close() {
  return shutdown();
}

bool IVFReader::shutdown() {
  if(!fp) {
    RX_ERROR(IVF_ERR_CLOSE);
    return false;
  }

  // playback
  state = IVF_STATE_NONE;
  time_started = 0;
  time_base = 0;
  frame_dx = 0;
  last_frame_dx = -1;

  // buffer
  rbuffer.reset();
  num_frames_to_buffer = 0;
  num_read_frames = 0;
  frames.clear();

  /* reading */
  if(fp) {
    fclose(fp);
    fp = NULL;
  }

  width = 0;
  height = 0;
  rate = 0;
  scale = 0;
  num_frames = 0;

  /* decoder */
  if(decoder) {
    delete decoder;
    decoder = NULL;
  }

  return true;
}

void IVFReader::update() {
  if(state != IVF_STATE_PLAY) {
    return ;
  }

  // Make sure the buffer contains at least "num_frames_to_buffer"
  updateBuffer();

  if(!frames.size()) {
    return;
  }

  if(frame_dx >= frames.size()) {
    return;
  }

  // Find the frame that we need to display
  uint64_t now = rx_millis();
  uint64_t time_playing = now - time_started;
  size_t start_dx = frame_dx;
  size_t use_frame = frame_dx;

  do {
    IVFFrame& frame = frames[frame_dx];
  
    if(frame.millis > time_playing)  {
      break;
    }
    
    use_frame = frame_dx;
    frame_dx++;

  } while(frame_dx < frames.size()-1);

  // only process new frames (when update() is called to quickly this makes sure we don't do any unnessary processing)
  if(last_frame_dx >= 0 && use_frame <= last_frame_dx) {
    return;
  }

  // we must process all the frames, even if they're not all shown (this is necessary so the decoder gets the key + inter frames)
  for(size_t i = last_frame_dx + 1; i <= use_frame; ++i) {
    IVFFrame& f = frames[i];
    size_t read = rbuffer.read((char*)frame_data, f.size); 

    decoder->decodeFrame(frame_data, f.size);

    if(num_read_frames > 0) {
      num_read_frames--;
    }
  }

  // remove all read frames
  for(size_t i = 0; i < use_frame; ++i) {
    frames.erase(frames.begin());
    frame_dx--;
    use_frame--;
  }

  if(frames.size() == 1) {
    stop();
  }
  
  last_frame_dx = use_frame;
}

void IVFReader::updateBuffer() {
  while(num_read_frames < num_frames_to_buffer) {
    if(!readFrame()) {
      num_frames_to_buffer = 0; // makes sure we stop looping when we reached the end of the file or an error occured
    }
  }
}

bool IVFReader::readFrame() {
  assert(width);
  assert(height);
  assert(fp);

  if(state != IVF_STATE_PLAY) {
    RX_ERROR(IVF_ERR_FRAME_STATE);
    return false;
  }

  if((fread(frame_hdr, 1, IVF_FRAME_HDR_SIZE, fp)) != IVF_FRAME_HDR_SIZE) {
    RX_ERROR(IVF_ERR_FRAME_READ_HDR);
    return false;
  }

  int frame_size = rx_get_le_u16(frame_hdr);
  if(frame_size > sizeof(frame_data)) {
    RX_ERROR(IVF_ERR_FRAME_TOO_BIG);
    return false;
  }

  uint64_t pts = rx_get_le_u64(frame_hdr + 4);

  if((fread(frame_data, 1, frame_size, fp)) != frame_size) {
    RX_ERROR(IVF_ERR_READ_FRAME);
    return false;
  }

  IVFFrame frame;
  frame.offset = rbuffer.getWriteIndex();
  frame.pts = pts;
  frame.size = frame_size;
  frame.millis = (pts * time_base) * 1000;
  frames.push_back(frame);

  rbuffer.write((char*)frame_data, frame_size);

  ++num_read_frames;
  return true;
}

void IVFReader::play() {

  if(state == IVF_STATE_PAUSE) {
    state = IVF_STATE_PLAY;
    time_started = time_started + (rx_millis() - time_paused); 
    return;

  }

  if(state == IVF_STATE_PLAY) {
    RX_ERROR(IVF_ERR_ALREADY_PLAYING);
    return;
  }

  reset();
  state = IVF_STATE_PLAY;

  updateBuffer(); // make sure we have some raw/encoded frames in the buffer

  time_started = rx_millis();
}

void IVFReader::stop() {
  state = IVF_STATE_NONE;
  frames.clear();
  rbuffer.reset();
  reset();
  readHeader(); 
}

void IVFReader::pause() {

  if(state == IVF_STATE_PAUSE) {
    RX_VERBOSE(IVF_V_ALREADY_PAUSED);
    return;
  }

  state = IVF_STATE_PAUSE;
  time_paused = rx_millis();
}

void IVFReader::reset() {
  time_started = 0;
  frame_dx = 0;
  last_frame_dx = -1;
  num_frames_to_buffer = 300;
  num_read_frames = 0;
}

void IVFReader::print() {
  RX_VERBOSE("ivf.width: %d", width);
  RX_VERBOSE("ivf.height: %d", height);
  RX_VERBOSE("ivf.rate: %d", rate);
  RX_VERBOSE("ivf.scale: %d", scale);
  RX_VERBOSE("ivf.num_frames: %d", num_frames);
  RX_VERBOSE("ivf.time_base: %f", time_base);

}
