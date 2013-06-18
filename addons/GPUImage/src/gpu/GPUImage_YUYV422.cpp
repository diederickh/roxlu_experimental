#include <gpu/GPUImage_YUYV422.h>

GPUImage_YUYV422::GPUImage_YUYV422() {
  fmt = AV_PIX_FMT_YUYV422;
}

GPUImage_YUYV422::~GPUImage_YUYV422() {
  shutdown();
}
