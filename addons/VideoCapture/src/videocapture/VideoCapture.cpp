#include <videocapture/VideoCapture.h>
#include <image/Image.h>

// Callback gets called by implementation
// --------------------------------------------------------------------------
void videocapture_process_frame_callback(void* pixels, size_t nbytes, void* user) {

  VideoCapture* c = static_cast<VideoCapture*>(user);
  assert(c->cb_user);
  assert(c->cb_frame);

  int img_nbytes = avpicture_fill((AVPicture*)c->video_frame_in, (uint8_t*)pixels, 
                                  c->settings.in_pixel_format, c->settings.width, 
                                  c->settings.height);

  if(c->sws) {
    AVFrame* f = c->video_frame_in;
    int h = sws_scale(c->sws,
                      f->data, f->linesize, 0, c->settings.height,
                      c->video_frame_out->data, c->video_frame_out->linesize);

    if(h != c->settings.height) {
      RX_ERROR(ERR_VIDCAP_SWS_SCALE);
      return;
    }
  }

  c->cb_frame(c->video_frame_in, c->nbytes_in,
              c->video_frame_out, c->nbytes_out,
              c->cb_user);

}

// --------------------------------------------------------------------------

VideoCapture::VideoCapture(VideoCaptureImplementation imp) 
  :sws(NULL)
  ,video_frame_in(NULL)
  ,video_frame_out(NULL)
  ,nbytes_in(0)
  ,nbytes_out(0)
  ,cb_user(NULL)
  ,cb_frame(NULL)
  ,cap(NULL)
{

  // Create the capture implementation
  switch(imp) {
#if defined(_WIN32)
    case VIDEOCAPTURE_DIRECTSHOW:                 {  cap = new VideoCaptureDirectShow2();     break;     }
    case VIDEOCAPTURE_WINDOWS_MEDIA_FOUNDATION:   {  cap = new VideoCaptureMediaFoundation(); break;     }
#endif
    default: {
      RX_ERROR("Unhandled VideoCaptureImplemtation type");
      ::exit(EXIT_FAILURE);
    }
  }
  if(imp == VIDEOCAPTURE_DIRECTSHOW) {

  }
  else if(imp == VIDEOCAPTURE_WINDOWS_MEDIA_FOUNDATION) {
    
  }
  else {

  }
  cap->setFrameCallback(videocapture_process_frame_callback, this);
}

VideoCapture::~VideoCapture() {
  RX_ERROR("Free video_frame_{in,out}, close device if its opened");

  if(cap) {
    cap->stopCapture();
    cap->closeDevice();
    delete cap;
    cap = NULL;
  }

  nbytes_in = 0;
  nbytes_out = 0;
  cb_user = NULL;
  cb_frame = NULL;
}

AVFrame* VideoCapture::allocVideoFrame(enum AVPixelFormat fmt, int w, int h) {
  uint8_t* buf;
  AVFrame* pic;
  int size;

  pic = avcodec_alloc_frame();
  if(!pic) {
    RX_VERBOSE(ERR_VIDCAP_ALLOC_FRAME);
    return NULL;
  }

  pic->width = w;
  pic->height = h;
  pic->format = fmt;

  size = avpicture_get_size(fmt, w,h);
  buf = (uint8_t*)av_malloc(size);
  if(!buf) {
    RX_ERROR(ERR_VIDCAP_ALLOC_FRAMEBUF);
    av_free(pic);
    return NULL;
  }

  avpicture_fill((AVPicture*)pic, buf, fmt, w, h);
  return pic;
}  


bool VideoCapture::openDevice(int device, VideoCaptureSettings cfg,
                              videocapture_frame_callback frameCB,
                              void* user) 
{
  settings = cfg;
  cb_frame = frameCB;
  cb_user = user;

  if(needsSWS()) {
    sws = sws_getContext(settings.width, settings.height, settings.in_pixel_format,
                         settings.width, settings.height, settings.out_pixel_format,
                         SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if(!sws) {
      RX_ERROR(ERR_VIDCAP_SWS);
      return false;
    }
    
    video_frame_in = allocVideoFrame(settings.in_pixel_format, settings.width, settings.height);
    video_frame_out = allocVideoFrame(settings.out_pixel_format, settings.width, settings.height);
    nbytes_in = avpicture_get_size(settings.in_pixel_format, settings.width, settings.height);
    nbytes_out = avpicture_get_size(settings.out_pixel_format, settings.width, settings.height);
  }
  else {
    video_frame_in = allocVideoFrame(settings.in_pixel_format, settings.width, settings.height);
    nbytes_in = avpicture_get_size(settings.in_pixel_format, settings.width, settings.height);
    nbytes_out = nbytes_in;
  }
  
  return cap->openDevice(device, cfg);
}


bool VideoCapture::needsSWS() {
  return settings.out_pixel_format != AV_PIX_FMT_NONE
    && settings.in_pixel_format != settings.out_pixel_format;
}



