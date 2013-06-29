#include <gpu/GPUImage.h>

GPUImage::GPUImage() 
  :imp(NULL)
{

}

GPUImage::~GPUImage() {
  if(imp) {
    delete imp;
    imp = NULL;
  }
}

bool GPUImage::setup(AVPixelFormat fmt) {

  switch(fmt) {                                
    case AV_PIX_FMT_UYVY422: {
      imp = new GPUImage_UYVY422();
      break;
    }
    case AV_PIX_FMT_YUYV422: {
      imp = new GPUImage_YUYV422();
      break;
    }
    case AV_PIX_FMT_RGB24: {
      imp = new GPUImage_RGB24();
      break;
    }
    case AV_PIX_FMT_RGBA: {
      imp = new GPUImage_RGBA();
      break;
    }
    default: {
      return false;
    }
  }

  if(!imp) {
    return false;
  }

  if(!imp->setup()) {
    RX_ERROR("Cannot setup the gpu pixel format handler.");
    return false;
  }

  return true;
}



