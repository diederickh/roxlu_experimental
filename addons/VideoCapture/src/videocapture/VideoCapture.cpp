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

  if(!img_nbytes) {
    RX_ERROR(ERR_VIDCAP_FILL_PIC);
    return;
  }

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
  else {
    c->video_frame_out = c->video_frame_in;
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
  ,state(VideoCapture::STATE_NONE)
{

  // Create the capture implementation
  switch(imp) {
#if defined(_WIN32)
    case VIDEOCAPTURE_DIRECTSHOW:                 {  cap = new VideoCaptureDirectShow2();     break;     }
    case VIDEOCAPTURE_WINDOWS_MEDIA_FOUNDATION:   {  cap = new VideoCaptureMediaFoundation(); break;     }
#elif defined(__APPLE__)
    case VIDEOCAPTURE_AVFOUNDATION:               {  cap = new VideoCaptureMac();             break;     }
#elif defined(__linux)      
    case VIDEOCAPTURE_V4L2:                       {  cap = new VideoCaptureV4L2();            break;     }
#endif

#if defined(ROXLU_USE_EDSDK) && !defined(__linux)
    case VIDEOCAPTURE_EDSDK:                      {  cap = new Canon();                       break;     }     
#endif
    default: {
      RX_ERROR("Unhandled VideoCaptureImplemtation type");
      ::exit(EXIT_FAILURE);
    }
  }

  cap->setFrameCallback(videocapture_process_frame_callback, this);
}

VideoCapture::~VideoCapture() {
  // @todo Free video_frame_{in,out}, close device if its opened
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
  state = VideoCapture::STATE_NONE;
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
  if(!buf){
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

  if(state == VideoCapture::STATE_OPENED) {
    RX_ERROR("Cannot open the device because it's already opened, first close it");
    return false;
  }
  else if(state == VideoCapture::STATE_CAPTURING) {
    RX_ERROR("Cannot open the device because we're capturing, make sure to call stopCapture() and closeDevice() before openening it again");
    return false;
  }

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
  
  if(!cap->openDevice(device, cfg)) {
    RX_ERROR("Error while trying to open the device");
    return false;
  }

  state = VideoCapture::STATE_OPENED;
  return true;
}


bool VideoCapture::closeDevice() {

  if(!cap) {
    RX_ERROR("Cannot close; did not allocate the capture implementation");
    return false;
  }
  if(state == VideoCapture::STATE_NONE) {
    RX_ERROR("Did not open the device yet so we cannot close it");
    return false;
  }

  bool r = cap->closeDevice();
  cap->setState(VIDCAP_STATE_NONE);

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  // @todo - need to test if se leak the video_frame_out / in here
  if(video_frame_out) {
    avcodec_free_frame(&video_frame_out);
    video_frame_in = NULL;
  }

  if(video_frame_in) {
    avcodec_free_frame(&video_frame_in);
    video_frame_in = NULL;
  }
  
  state = VideoCapture::STATE_NONE;
  return r;
}


bool VideoCapture::needsSWS() {
  return settings.out_pixel_format != AV_PIX_FMT_NONE
    && settings.in_pixel_format != settings.out_pixel_format;
}
