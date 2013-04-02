#include <flv/FLVScreenRecorder.h>

// @todo we probably want a bigger distance between the read and write  - tested quickly didnt see a difference
// @todo check if we need to call AV::waitForEncodingThreadToFinish();
// PBO indices.

FLVScreenRecorder::FLVScreenRecorder() 
  :channels(4)
#if !defined(SCREEN_RECORDER_USE_PBO)
  ,pixels(NULL)
#endif
  ,dx(0)
  ,is_recording(false)
  ,is_file_opened(false)
{
  memset(pbos, 0, sizeof(pbos));
}

FLVScreenRecorder::~FLVScreenRecorder() {
  stop();
  av.waitForEncodingThreadToFinish();

#if !defined(SCREEN_RECORDER_USE_PBO) 
  if(pixels != NULL) {
    delete[] pixels;
    pixels = NULL;
  }
#endif
  is_file_opened = false;
  is_recording = false;
  channels = 0;
  dx = 0;


}

bool FLVScreenRecorder::open(std::string filepath) {
  RX_VERBOSE("Open file: %s", filepath.c_str());

  if(!flv_writer.open(filepath)) {
    return false;
  }
  is_file_opened = true;
  return true;
}

bool FLVScreenRecorder::setup(FLVScreenRecorderSettings cfg) {
  assert(cfg.vid_in_w != 0);
  assert(cfg.vid_in_h != 0);
	
  settings = cfg;

  av.setVerticalFlip(true);

  bool r = av.setupVideo(settings.vid_in_w, settings.vid_in_h, 
                         settings.vid_out_w, settings.vid_out_h, settings.vid_fps, 
                         AV_FMT_BGRA32, &flv);

  if(!r) {
    RX_ERROR(("cannot setup video"));
    return false;
  }

  if(settings.audio_num_channels != 0 && settings.audio_max_samples != 0) {
    r = av.setupAudio(settings.audio_num_channels, settings.audio_samplerate, 
                      settings.audio_max_samples, settings.audio_format);
    if(!r) {
      RX_ERROR(("cannot setup audio"));
      return false;
    }
  }

  flv.setCallbacks(flv_file_write,
                   flv_file_rewrite,
                   flv_file_flush,
                   flv_file_close,
                   &flv_writer);

#if !defined(SCREEN_RECORDER_USE_PBO)
  pixels = new unsigned char[settings.vid_in_w * settings.vid_in_h * channels];
  memset(pixels, 0, settings.vid_in_w * settings.vid_in_h * channels);
#endif

  if(!av.initialize()) {
    RX_WARNING(("cannot initialize AV."));
    return false;
  }

#if defined(SCREEN_RECORDER_USE_PBO) 
  glGenBuffers(SCREEN_RECORDER_NUM_PBOS, pbos);
  for(int i = 0; i < SCREEN_RECORDER_NUM_PBOS; ++i) {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
    glBufferData(GL_PIXEL_PACK_BUFFER, settings.vid_in_w * settings.vid_in_h * channels, pixels, GL_STREAM_READ);
  }

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
#endif

  return true;
}

void FLVScreenRecorder::grabFrame() {

  if(!is_recording) {
    RX_WARNING(("stopped, cant grab."));
    return;
  }

#if !defined(SCREEN_RECORDER_USE_PBO)
  if(av.wantsNewVideoFrame()) {
    glReadPixels(0, 0, settings.vid_in_w, settings.vid_in_h, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    av.addVideoFrame(pixels);
  }
#else
  if(av.wantsNewVideoFrame()) {
    RX_VERBOSE("Grab a frame.");
    rx_int64 now = rx_millis();
    int write_dx = (dx++) % SCREEN_RECORDER_NUM_PBOS;
    int read_dx = (write_dx + 1) % SCREEN_RECORDER_NUM_PBOS;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[write_dx]);
    glReadPixels(0, 0, settings.vid_in_w, settings.vid_in_h, GL_BGRA, GL_UNSIGNED_BYTE, 0); 

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[read_dx]);
    GLubyte* ptr = (GLubyte*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if(ptr) {
      av.addVideoFrame(ptr);
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  }

#endif
}

void FLVScreenRecorder::addAudio(void* input, int nframes) {
  av.addAudioFrame(input, nframes);
}

void FLVScreenRecorder::start() {
  if(is_recording) {
    RX_WARNING(("already started."));
    return;
  }

#if defined(SCREEN_RECORDER_USE_PBO)
  for(int i = 0; i < SCREEN_RECORDER_NUM_PBOS; ++i ) {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
    glReadPixels(0,0,settings.vid_in_w, settings.vid_in_h, GL_BGRA, GL_UNSIGNED_BYTE, 0);
  }
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
#endif  

  RX_VERBOSE("Start recording.");
  is_recording = true;
  av.start();
}

void FLVScreenRecorder::stop() {
  if(!is_recording) {
    RX_WARNING(("already stopped."));
    return;
  }

  RX_VERBOSE("Stop recording.");
  av.stop();

  is_recording = false;
  is_file_opened = false; // av.stop() calls close on the flv writer
  dx = 0;

  // reset pixel buffers
#if defined(SCREEN_RECORDER_USE_PBO)

  for(int i = 0; i < SCREEN_RECORDER_NUM_PBOS; ++i) {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
    glBufferData(GL_PIXEL_PACK_BUFFER, settings.vid_in_w * settings.vid_in_h * channels, pixels, GL_STREAM_READ);
  }
  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

#endif
}

FLVScreenRecorderSettings::FLVScreenRecorderSettings() {
  reset();
}
FLVScreenRecorderSettings::~FLVScreenRecorderSettings() {
  reset();
}

void FLVScreenRecorderSettings::reset() {
  vid_in_w = 0;
  vid_in_h = 0;
  vid_out_w = 0;
  vid_out_h = 0;
  vid_fps = 0;
  audio_num_channels = 0;
  audio_samplerate = 0;
  audio_max_samples = 0;
  audio_format = AV_FMT_INT16;
}
