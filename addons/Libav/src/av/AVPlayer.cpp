extern "C" {
#  include <libavutil/avutil.h>
#  include <libavutil/imgutils.h>
}

#include <roxlu/Roxlu.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <av/AVPlayer.h>

// ----------------------------------------------

void avplayer_thread(void* user) {

  AVPlayer& player = *static_cast<AVPlayer*>(user);

  if(!player.initialize()) {
    return;
  }

  bool is_eof = false;
  bool must_stop = false;
  SwsContext* sws = player.sws;
  AVFrame* av_frame = NULL;
  AVPlayerFrame* player_frame = NULL;
  AVDecoderFrame* decoder_frame = NULL;
  std::vector<AVPlayerFrame*> free_frames;
  std::vector<AVPlayerFrame*> work_frames;

  while(true) {
    
    free_frames.clear();

    if(!player.getFreeFrames(free_frames)) {
      rx_sleep_millis(10);                                      // @TODO check if this is ok..., it looks like it's no problem, though lets give it a bit time

      player.lock();
      must_stop = player.must_stop;
      player.unlock();

      if(must_stop) {
        break;
      }

      continue;
    }

    // decode frames (can be audio, video, subtitle etc..)
    work_frames.clear();
    for(std::vector<AVPlayerFrame*>::iterator it = free_frames.begin(); it != free_frames.end(); ++it) {
      player_frame = *it;
      decoder_frame = player.dec.decodeFrame(is_eof);

      if(!decoder_frame) {
         break;
      }

      if(is_eof) {
        must_stop = true;
        break;
      }
      
      switch(decoder_frame->type) {

        case AV_TYPE_VIDEO: {

          player_frame->reset();

          av_frame = decoder_frame->frame;      
          player_frame->is_free = false;
          player_frame->decoder_frame = decoder_frame;
          player_frame->nbytes = player.nbytes_video_frame;
                    
          if(sws) {
            int h = sws_scale(sws, av_frame->data, av_frame->linesize, 0, 
                              av_frame->height, player_frame->pic.data, 
                              player_frame->pic.linesize);

            if(h != av_frame->height) {
              RX_ERROR(ERR_AVP_SWSCALE_FAILED);
              player_frame->is_free = true;
              break;
            }

            player_frame->data = (unsigned char*)player_frame->pic.data[0];
          }
          else {
            player_frame->data = (unsigned char*)av_frame->data[0];
          }

          work_frames.push_back(player_frame);                    

          break;
        } // AV_TYPE_VIDEO

        default: {
          RX_ERROR(ERR_AVP_THREAD_UNHANDLED_TYPE);
          break;
        }
      };

    } // end decode frame loop

    // copy the decoded frames 
    if(work_frames.size()) {
      player.lock();
      std::copy(work_frames.begin(), work_frames.end(), std::back_inserter(player.decoded_frames));
      player.unlock();
    }

    player.lock();
    must_stop = player.must_stop;
    player.unlock();

    if(must_stop) {
      break;
    }
    
  }  // end thread loop
  
  player.shutdown();

}

// ----------------------------------------------

AVPlayerFrame::AVPlayerFrame() 
  :decoder_frame(NULL)
  ,data(NULL)
  ,is_free(true)
  ,nbytes(0)
{
}

AVPlayerFrame::~AVPlayerFrame() {
  if(decoder_frame) {
    delete decoder_frame;
    decoder_frame = NULL;
  }

  avpicture_free(&pic);
  is_free = true;
  data = NULL;
  nbytes = 0;
}


// ----------------------------------------------

AVPlayer::AVPlayer() 
  :state(AVP_STATE_NONE)
  ,num_frames_to_allocate(0)
  ,time_started(0)
  ,time_paused(0)
  ,nbytes_video_frame(0)
  ,sws(NULL)
  ,must_stop(false)
  ,datapath(false)
{
  uv_mutex_init(&mutex);
  rx_init_libav();
}

AVPlayer::~AVPlayer() {
  stop();

  uv_thread_join(&thread);
  uv_mutex_destroy(&mutex);

  shutdown();

  state = AVP_STATE_NONE;
  must_stop = true;
  nbytes_video_frame = 0;
  time_started = 0;
  time_paused = 0;
  num_frames_to_allocate = 0;
}

bool AVPlayer::setup(std::string filename, bool datapath, AVPlayerSettings cfg, int numPreAllocateFrames) {
  this->filename = filename;
  this->datapath = datapath;
  this->num_frames_to_allocate = numPreAllocateFrames;
  this->settings = cfg;

  if(!open()) {
    return false;
  }

#if defined(__APPLE__)
  if(cfg.out_pixel_format != AV_PIX_FMT_NONE && cfg.out_pixel_format != AV_PIX_FMT_UYVY422) {
    RX_ERROR("For now we only support the AVPlayerSettings.out_pixel_format = AV_PIX_FMT_UYVY422! on mac.");
    ::exit(EXIT_FAILURE);
  }
#endif  

  gl_surface.setup(dec.getWidth(), dec.getHeight()); 

  return true;
}

bool AVPlayer::open() {

  if(isOpen()) {
    RX_ERROR(ERR_AVP_ALREADY_OPEN);
    return false;
  }

  if(!dec.open(filename, datapath)) {
    return false;
  }

  return true;
}

bool AVPlayer::initialize() {

  if(!isOpen()) {
    open();
  }

  if(settings.out_pixel_format == AV_PIX_FMT_NONE) {
    settings.out_pixel_format = getPixelFormat();
  }

  nbytes_video_frame = avpicture_get_size(settings.out_pixel_format, getWidth(), getHeight());

  dec.print();
  
  if(frames.size()) {
    RX_ERROR(ERR_AVP_INIT_FRAMES_EXIST);
    return false;
  }
  
  if(!initializeSWS()) {
    return false;
  }

  if(!allocateFrames()) {
    return false;
  }

  return true;
}

bool AVPlayer::shutdown() {

  dec.close();

  deleteFrames();

  decoded_frames.clear();

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }
  
  must_stop = false;

  return true;
}

bool AVPlayer::allocateFrames() {
  
  if(needsToConvertPixelFormat()) {
    // allocate frames + a buffer for SWS to put the converted pixels in.    
    if(!sws) {
      RX_ERROR(ERR_AVP_PREALLOC_NO_SWS);
      return false;
    }

    bool err = false;
    for(int i = 0; i < num_frames_to_allocate; ++i) {
      AVPlayerFrame* f = new AVPlayerFrame();
      frames.push_back(f);

      int r = avpicture_alloc(&f->pic, settings.out_pixel_format, getWidth(), getHeight()); 
      if(r < 0) {
        RX_ERROR(ERR_AVP_PREALLOC_PIC);
        err = true;
        break;
      }

      if(err) {
        deleteFrames();
        return false;
      }
    }
  }
  else {

    for(int i = 0; i < num_frames_to_allocate; ++i) {
      AVPlayerFrame* f = new AVPlayerFrame();
      frames.push_back(f);
    }

  }

  return true;
}

bool AVPlayer::play() {

  if(state == AVP_STATE_PAUSE) {
    state = AVP_STATE_PLAY;
    time_started = time_started +(millis() - time_paused);
    return true;
  }

  if(state == AVP_STATE_PLAY) {
    RX_ERROR(ERR_AVP_ALREADY_PLAYING);
    return false;
  }

  state = AVP_STATE_PLAY;

  time_started = millis();

  uv_thread_create(&thread, avplayer_thread, this);
  
  return true;
}

bool AVPlayer::pause() {

  if(state == AVP_STATE_PAUSE) {
    RX_ERROR(ERR_AVP_ALREADY_PAUSED);
    return false;
  }

  time_paused = millis();
  state = AVP_STATE_PAUSE;
  return true;
}

bool AVPlayer::stop() {
  state = AVP_STATE_NONE;
  time_started = 0; // @TODO -> move to a new function `shutdown()` (?)

  lock();
  {
    must_stop = true;
  }
  unlock();

  return true;
}


void AVPlayer::draw(int x, int y, int w, int h) {

  if(state != AVP_STATE_PLAY && state != AVP_STATE_PAUSE) {
    return;
  }


  if(state == AVP_STATE_PLAY) {
    // check if there is a frame in the queue.
    AVPlayerFrame* f = NULL;
    lock();
    if(decoded_frames.size()) {
      f = decoded_frames[0];
    }
    unlock();

    // we got a frame, check if we need to display it
    if(f) { 

      uint64_t time_playing = millis() - time_started;

      if(f->decoder_frame->pts <= time_playing) {

        if(f->decoder_frame->type == AV_TYPE_VIDEO) {
          gl_surface.setPixels((unsigned char*)f->data, f->nbytes);

          lock();
          {
            f->is_free = true;
            decoded_frames.erase(decoded_frames.begin());
          }
          unlock();

        }
        else {
          RX_VERBOSE("We got a frame which is not a video frame... we need to handle this!!");
        }
      }
    }
  }

  if(time_started) {
    gl_surface.draw(x, y, w, h);
  }
}

bool AVPlayer::initializeSWS() {

  if(sws) {
    RX_ERROR(ERR_AVP_ALREADY_SWS);
    return false;
  }

  sws = sws_getContext(getWidth(), getHeight(), getPixelFormat(), 
                       getWidth(), getHeight(), settings.out_pixel_format, 
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);

  if(!sws) {
    RX_ERROR(ERR_AVP_ALLOC_SWS);
    return false;
  }

  return true;
}

bool AVPlayer::needsToConvertPixelFormat() {
  return (settings.out_pixel_format != AV_PIX_FMT_NONE && dec.getPixelFormat() != settings.out_pixel_format);
}

bool AVPlayer::getFreeFrames(std::vector<AVPlayerFrame*>& result) {
  lock();
  for(std::vector<AVPlayerFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    AVPlayerFrame* f = *it;
    if(f->is_free) {
      result.push_back(f);
    }
  }
  unlock();

  return result.size();
}

void AVPlayer::deleteFrames() {
  for(std::vector<AVPlayerFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    delete *it;
  }
  frames.clear();
}

void AVPlayer::freeFrames() {
  lock();
  for(std::vector<AVPlayerFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    AVPlayerFrame* f = *it;
    f->is_free = true;
  }
  unlock();
}


