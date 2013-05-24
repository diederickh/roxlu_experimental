#include <videocapture/VideoCapture.h>

void video_capture_callback(void* pixels, size_t nbytes, void* user) {
  VideoCapture* c = static_cast<VideoCapture*>(user);

  if(c->allocated_bytes < nbytes) {
    while(c->allocated_bytes < nbytes) {
      c->allocated_bytes = std::max<size_t>(c->allocated_bytes * 2, 4096);
    }
    
    if(c->bytes != NULL) {
      delete[] c->bytes;
    }
    c->bytes = new unsigned char[c->allocated_bytes];
  }
  // @todo not all I/O methods need to copy the data!
  //memcpy(c->bytes, pixels, nbytes);

  // convert between pixel formats if necessary
  if(c->sws) {
    int img_nbytes = avpicture_fill((AVPicture*)c->video_frame_in, (uint8_t*)pixels, 
                                    c->settings.in_pixel_format, c->getWidth(), c->getHeight());
    //                                    c->settings.in_pixel_format, c->getWidth(), c->getHeight());

    RX_VERBOSE("linesize: %d", c->video_frame_out->linesize[0]);
    /*
   int h = sws_scale(c->sws,
                     c->video_frame_in->data, c->video_frame_in->linesize, 0, c->getHeight(),
                     c->video_frame_out->data, c->video_frame_out->linesize);
    */
    //int linesize[4] = {c->video_frame_in->width * 2, 0,0,0};
    AVFrame* f = c->video_frame_in;
    f->linesize[0] = f->width * 2;
    RX_VERBOSE("%d %d %d %d - width: %d, %d", int(f->linesize[0]), f->linesize[1], f->linesize[2], f->linesize[3], f->width, c->getHeight());
   int h = sws_scale(c->sws,
                     c->video_frame_in->data, f->linesize, 0, c->getHeight(),
                     c->video_frame_out->data, c->video_frame_out->linesize);
   int size = avpicture_get_size(c->settings.out_pixel_format, c->settings.width, c->settings.height);
   RX_VERBOSE("SIZE: %d", size);
   memcpy(c->bytes, c->video_frame_out->data[0], size);
   // c->bytes = c->video_frame_out->data[0];
   //av_frame_unref(c->video_frame_in);
   RX_VERBOSE("H:%d", h);

  }
  RX_VERBOSE("CALCULATE THE WIDTH AND HEIGHT!");
  c->has_new_data = true;
  c->nbytes = 640 * 480 * 3; 
}

VideoCapture::VideoCapture()
  :allocated_bytes(0)
  ,has_new_data(false)
  ,nbytes(0)
  ,bytes(NULL)
  ,fmt(VC_NONE)
  ,sws(NULL)
  ,video_frame_in(NULL)
  ,video_frame_out(NULL)
{
#if defined(__APPLE__)
  capture = rx_capture_avfoundation;
#elif defined(_WIN32)
  capture = rx_capture_directshow;
#else 
  capture = rx_capture_v4l2;
#endif
  capture.initialize(&capture);
}

AVFrame* VideoCapture::allocVideoFrame(enum AVPixelFormat fmt, int w, int h) {
  uint8_t* buf;
  AVFrame* pic;
  int size;

  pic = avcodec_alloc_frame();
  if(!pic) {
    RX_VERBOSE("Cannot allocate a frame");
    return NULL;
  }
  
  pic->width = w;
  pic->height = h;
  pic->format = fmt;

  size = avpicture_get_size(fmt, w,h);
  buf = (uint8_t*)av_malloc(size);
  if(!buf) {
    RX_ERROR("Error while allocating the AVFrame that we use to convert webcam pixel format");
    av_free(pic);
    return NULL;
  }

  avpicture_fill((AVPicture*)pic, buf, fmt, w, h);
  
  return pic;

}

VideoCapture::~VideoCapture() {
  capture.close_device(&capture);
  RX_ERROR("FREE SWS");
}


