#ifndef ROXLU_VIDEOCAPTURE_V4L2_TYPES_H
#define ROXLU_VIDEOCAPTURE_V4L2_TYPES_H

#include <string>

enum VideoCaptureV4L2IOMethod {
  IO_METHOD_READ,
  IO_METHOD_MMAP,
  IO_METHOD_USERPTR
};

// Used with MMAP io
struct VideoCaptureV4L2Buffer {
  VideoCaptureV4L2Buffer();
  void* start;
  size_t length;
};

// Represents a V4L2 device
struct VideoCaptureV4L2Device {
  VideoCaptureV4L2Device();
  void print();

  std::string path;
  std::string id_vendor;
  std::string id_product;
  std::string driver;
  std::string card;
  std::string bus_info;
  int version_major;
  int version_minor;
  int version_micro;
};

#endif
