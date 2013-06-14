#include <roxlu/core/Log.h>
#include <videocapture/linux/v4l2/VideoCaptureV4L2.h>

// WRAPPER - see: https://gist.github.com/maxlapshin/1253534
int v4l2_ioctl(int fh, int request, void* arg) {
  int r;

  do {
    r = ioctl(fh, request, arg);
  } while(-1 == r && EINTR == errno);

  return r;
}

// ----------------------------------------------------

VideoCaptureV4L2::VideoCaptureV4L2() 
  :capture_device_fd(0)
  ,io_method(IO_METHOD_MMAP)
{

}

VideoCaptureV4L2::~VideoCaptureV4L2() {
  stopCapture();
  closeDevice();
}

// CAMERA CONTROL
// ----------------------------------------------------
int VideoCaptureV4L2::listDevices() {
  struct udev* udev;
  struct udev_enumerate* enumerate;
  struct udev_list_entry* devices;
  struct udev_list_entry* dev_list_entry;
  struct udev_device* dev;
 
  udev = udev_new();
  if(!udev) {
    RX_ERROR("Cannot udev_new()\n");
    return 0;
  }
 
  enumerate = udev_enumerate_new(udev);
  udev_enumerate_add_match_subsystem(enumerate, "video4linux");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);
 
  bool found = false;
  int count = 0;

  udev_list_entry_foreach(dev_list_entry, devices) {
    const char* path = udev_list_entry_get_name(dev_list_entry);
    dev = udev_device_new_from_syspath(udev, path);

    const char* devpath = udev_device_get_devnode(dev);
    dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
    if(!dev) {
      continue;
    }
    
    VideoCaptureV4L2Device v4l2_device;
    if(getDeviceInfo(devpath, v4l2_device)) {
      RX_VERBOSE("[%d] - %s %u.%u.%u", count, v4l2_device.card.c_str(), v4l2_device.version_major, v4l2_device.version_minor, v4l2_device.version_micro);
    }
    ++count;
  }

  udev_enumerate_unref(enumerate);
  udev_unref(udev);
  return count;
}

bool VideoCaptureV4L2::openDevice(int dx, VideoCaptureSettings cfg) {
  // test io
  if(io_method != IO_METHOD_MMAP) {
    RX_ERROR("We only support mmap io for now");
    return false;
  }

  // find and open the device
  if(!getDevice(dx, capture_device)) {
    RX_ERROR("Cannot get device info for index: %d", dx);
    return false;
  }
  
  capture_device_fd = openDevice(capture_device.path);
  if(capture_device_fd < 0) {
    return false;
  }

  // setup the capture format (pixel format, width, height)
  if(!setCaptureFormat(capture_device_fd, cfg.width, cfg.height, cfg.in_pixel_format)) {
    closeDevice(capture_device_fd);
    return false;
  }

  // get caps
  struct v4l2_capability caps;
  if(!getCapability(capture_device_fd, &caps)) {
    return false;
  }
  if(!(caps.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    RX_ERROR("Not a video capture device; we only support video capture devices");
    closeDevice(capture_device_fd);
    return false;
  }

  // check io
  bool can_io_readwrite = (caps.capabilities & V4L2_CAP_READWRITE) == V4L2_CAP_READWRITE;
  bool can_io_stream = (caps.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING;
  if(!can_io_readwrite && !can_io_stream) {
    RX_ERROR("Cannot use read() or memory streaming with this device");
    closeDevice(capture_device_fd);
    return false;
  }
  if(!can_io_stream) {
    RX_ERROR("The device cannot memory stream; we only support this method for now");
    closeDevice(capture_device_fd);
    return false;
  }

  // initalize IO
  if(io_method == IO_METHOD_MMAP) {
    if(!initializeMMAP(capture_device_fd)) {
      return false;
    }
  }
  else {
    RX_ERROR("Unsupported io method");
    closeDevice(capture_device_fd);    
    return false;
  }

  return true;
}

bool VideoCaptureV4L2::closeDevice() {
  if(io_method == IO_METHOD_MMAP) {
    shutdownMMAP();
  }
  else {
    RX_ERROR("We have not yet implemented the `shutdown` method for this IO type");
  }

  return closeDevice(capture_device_fd);
}

bool VideoCaptureV4L2::startCapture() {
  if(!capture_device_fd) {
    RX_ERROR("Cannot start capture as the device descriptor is invalid");
    return false;
  }
  
  if(io_method != IO_METHOD_MMAP) {
    RX_ERROR("Canot start capturing because the set io_method is not supported");
    return false;
  }

  for(int i = 0; i < capture_buffers.size(); ++i) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    
    if(v4l2_ioctl(capture_device_fd, VIDIOC_QBUF, &buf) == -1) {
      RX_ERROR("VIDIO_QBUF failed - invalid mmap buffer.");
      return false;
    }
  }

  // stream on!
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(v4l2_ioctl(capture_device_fd, VIDIOC_STREAMON, &type) == -1) {
    RX_ERROR("Failed to start the video capture stream");
    return false;
  }

  setState(VIDCAP_STATE_CAPTURING);

  return true;
}

bool VideoCaptureV4L2::stopCapture() {
  if(!capture_device_fd) {
    RX_ERROR("Cannot stop captureing becuause the device descriptor is invalid");
    return false;
  }

  // stream off!
  enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(v4l2_ioctl(capture_device_fd, VIDIOC_STREAMOFF, &type) == -1) {
    RX_ERROR("Cannot stop captureing because of an ioctl error. (did you really start capturing before?)");
    return false;
  }

  setState(VIDCAP_STATE_NONE);
  return true;
}

void VideoCaptureV4L2::update() {

  if(!capture_device_fd) {
    return;
  }

  if(state != VIDCAP_STATE_CAPTURING) {
    return;
  }

#if 0  
  fd_set fds;
  struct timeval tv;
  int r;
    
  FD_ZERO(&fds);
  FD_SET(capture_device_fd, &fds);
  
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  r = select(capture_device_fd + 1, &fds, NULL, NULL, &tv);
  if(r == -1) {
    if(errno == EINTR) {
      return;
    }
    else {
      RX_ERROR("Error with select(), %s", strerror(errno));
      return;
    }
  }
  else if(r == 0) {
    // timeout
    RX_VERBOSE("Timeout when trying to get data from the cature device");
    closeDevice();
    return;
  }
#endif

  readFrame();
}

bool VideoCaptureV4L2::readFrame() {
  if(!capture_device_fd) {
    RX_ERROR("Cannot readFrame() because the capture device descriptor is invalid");
    return false;
  }

  if(io_method != IO_METHOD_MMAP) {
    RX_ERROR("Cannot readFrame() because the io method is not supported");
    return false;
  }

  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  
  if(v4l2_ioctl(capture_device_fd, VIDIOC_DQBUF, &buf) == -1) {
    if(errno == EAGAIN) {
      return true; // everything ok; just not ready yet
    }
    else if(errno == EIO) {
      RX_ERROR("IO error");
      return true; // we could handle this as an error.
    }
    else {
      RX_ERROR("Error with reading the memory buffer: %s", strerror(errno));
      return false;
    }
  }

  assert(buf.index < capture_buffers.size());

  if(cb_frame) {
    cb_frame(capture_buffers[buf.index]->start, buf.bytesused, cb_user);
  }

  if(v4l2_ioctl(capture_device_fd, VIDIOC_QBUF, &buf) == -1) {
    RX_ERROR("Error with queueing the buffer again: %s", strerror(errno));
    return false;
  }

  return true;
}


// DEVICE RELATED
// ----------------------------------------------------

// Get capabilities for the given (opened) device descriptor
bool VideoCaptureV4L2::getCapability(int fd, struct v4l2_capability* caps) {
  if(!fd) {
    RX_ERROR("Cannot get capablity; invalid fd;");
    return false;
  }

  memset(caps, 0, sizeof(*caps));
  if(v4l2_ioctl(fd, VIDIOC_QUERYCAP, caps) == -1) {
    RX_ERROR("Cannot query capability for: %d", fd);
    return false;
  }
  return true;
}

// Get information about the device at the given index
bool VideoCaptureV4L2::getDeviceInfo(int dx, VideoCaptureV4L2Device& result) {
  if(!getDevice(dx, result)) {
    RX_ERROR("Cannot find the input device");
    return false;
  }
  
  return getDeviceInfo(result.path.c_str(), result);
}


// get device information with the given devnode path
bool VideoCaptureV4L2::getDeviceInfo(const char* path, VideoCaptureV4L2Device& result) {
 int fd = openDevice(path);
  if(fd < 0) {
    return false;
  }

  struct v4l2_capability cap;
  memset(&cap, 0, sizeof(cap));
  if(v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
    RX_ERROR("Cannot query the device capabilities");
    return false;
  }

  result.driver.assign((char*)cap.driver, strlen((char*)cap.driver));
  result.card.assign((char*)cap.card, strlen((char*)cap.card));
  result.bus_info.assign((char*)cap.bus_info, strlen((char*)cap.bus_info));;
  result.version_major = (cap.version >> 16) & 0xFF;

  result.version_minor = (cap.version >> 8) & 0xFF;
  result.version_micro = (cap.version & 0xFF);
  closeDevice(fd);
  return true;
}


// Get all capabilities for the given device index
std::vector<AVCapability> VideoCaptureV4L2::getCapabilities(int dx) {
  struct v4l2_capability cap;
  struct v4l2_standard standard;
  struct v4l2_input input;
  struct v4l2_fmtdesc fmtdesc;
  struct v4l2_format format;
  std::vector<AVCapability> result;

  memset(&cap, 0, sizeof(cap));

  VideoCaptureV4L2Device device;
  if(!getDevice(dx, device)) {
    RX_ERROR("Cannot find the input device");
    return result;
  }

  int fd = openDevice(device.path);
  if(fd < 0) {
    return result;
  }

  if(v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
    RX_ERROR("Cannot query the device capabilities");
    return result;
  }

  device.driver.assign((char*)cap.driver, strlen((char*)cap.driver));
  device.card.assign((char*)cap.card, strlen((char*)cap.card));
  device.bus_info.assign((char*)cap.bus_info, strlen((char*)cap.bus_info));;
  device.version_major = (cap.version >> 16) & 0xFF;
  device.version_minor = (cap.version >> 8) & 0xFF;
  device.version_micro = (cap.version & 0xFF);

  // capabilities
  if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {

    for(int i = 0; ; ++i) {
      memset(&fmtdesc, 0, sizeof(fmtdesc));
      fmtdesc.index = i;
      fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if(v4l2_ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == -1) {
        break;
      }

      AVCapability capability;

      // frame sizes and fps for this pixel format
      struct v4l2_frmsizeenum frames;
      frames.index = 0;
      frames.pixel_format = fmtdesc.pixelformat;
      while(!v4l2_ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frames)) {

        if(frames.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
          struct v4l2_frmivalenum fpse;
          fpse.index = 0;
          fpse.pixel_format = frames.pixel_format;
          fpse.width = frames.discrete.width;
          fpse.height = frames.discrete.height;

          while(!v4l2_ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fpse)) {
            if(fpse.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
              capability.framerate.num = fpse.discrete.numerator;
              capability.framerate.den = fpse.discrete.denominator;
              capability.size.width = frames.discrete.width;
              capability.size.height = frames.discrete.height;
              capability.pixel_format = v4l2_pixel_format_to_libav_pixel_format(frames.pixel_format);

              if(capability.pixel_format == AV_PIX_FMT_NONE) {
                RX_VERBOSE("Unhandled pixel format: %s", v4l2_pixel_format_to_string(fmtdesc.pixelformat).c_str());
              }
              result.push_back(capability);
            }
            fpse.index++;
          }
        }
        frames.index++;
      }
    } // for
  }
  

  closeDevice(fd);

  return result;
}

bool VideoCaptureV4L2::closeDevice(int fd) {
  if(fd < 0) {
    RX_ERROR("Wrong device descriptor");
    return false;
  }

  if(close(fd) == -1) {
    RX_ERROR("Error while trying to close: %d", fd);
    return false;
  }

  fd = 0;
  return true;
}

// Open the given device (by path) , returns the file descriptor or < 0 on error
int VideoCaptureV4L2::openDevice(std::string path) {
  if(!path.size()) {
    RX_ERROR("Wrong path");
    return -1;
  }

  int fd = open(path.c_str(), O_RDWR | O_NONBLOCK, 0);
  if(fd == -1) {
    RX_ERROR("Cannot open: %s", path.c_str());
    return -2;
  }

  return fd;
}

// Return some information about a capture device
bool VideoCaptureV4L2::getDevice(int dx, VideoCaptureV4L2Device& result) {
  struct udev* udev;
  struct udev_enumerate* enumerate;
  struct udev_list_entry* devices;
  struct udev_list_entry* dev_list_entry;
  struct udev_device* dev;
 
  udev = udev_new();
  if(!udev) {
    RX_ERROR("Cannot udev_new()\n");
    return false;
  }
 
  enumerate = udev_enumerate_new(udev);
  udev_enumerate_add_match_subsystem(enumerate, "video4linux");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);
 
  bool found = false;
  int count = 0;

  udev_list_entry_foreach(dev_list_entry, devices) {

    if(count == dx) {
      const char* path = udev_list_entry_get_name(dev_list_entry);
      dev = udev_device_new_from_syspath(udev, path);
      if(!dev) {
        RX_ERROR("Cannot get the device");
        break;
      }

      result.path = udev_device_get_devnode(dev);
      if(!result.path.size()) {
        RX_ERROR("Cannot get devpath");
        break;
      }

      dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
      if(!dev) {
        RX_ERROR("Cannot find related usb device");
        break;
      }

      result.id_vendor = udev_device_get_sysattr_value(dev, "idVendor");
      result.id_product = udev_device_get_sysattr_value(dev, "idProduct");
      found = true;
      break;
    }
    ++count;
  }

  udev_enumerate_unref(enumerate);
  udev_unref(udev);
  return found;
}

bool VideoCaptureV4L2::setCaptureFormat(int fd, int width, int height, AVPixelFormat pixelFormat) {
  if(!fd) {
    RX_ERROR("Cannot set capture format on an invalid device descriptor");
    return false;
  }

  // As described in section 4.1.3, http://www.linuxtv.org/downloads/legacy/video4linux/API/V4L2_API/spec-single/v4l2.html#id2832544
  // we first retrieve the current pixel format and then adjust the settings to our needs before
  // trying the format.
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(v4l2_ioctl(fd, VIDIOC_G_FMT, &fmt) == -1) {
    RX_ERROR("Cannot retrieve the current format that we need to change/set it");
    return false;
  }

  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  fmt.fmt.pix.pixelformat = v4l2_libav_pixel_format_to_v4l2_pixel_format(pixelFormat);
  if(fmt.fmt.pix.pixelformat == 0) {
    RX_ERROR("Cannot find a v4l2 pixel format for the given a LibAV PixelFormat");
    return false;
  }
   
  // try the new format
  if(v4l2_ioctl(fd, VIDIOC_TRY_FMT, &fmt) == -1) {
    RX_ERROR("The video capture device doesnt support the given format");
    return false;
  }

  // set the new format
  if(v4l2_ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
    RX_ERROR("Cannot set the given format %d x %d, (%s)", width, height, strerror(errno));
    return false;
  }

  return true;
}

bool VideoCaptureV4L2::printCurrentCaptureFormat(int fd) {
  if(!fd) {
    RX_ERROR("Cannot retrieve the capture format for the given device descriptor");
    return false;
  }

  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof(fmt));
  
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(v4l2_ioctl(fd, VIDIOC_G_FMT, &fmt) == -1) {
    RX_ERROR("Cannot retrieve the current format");
    return false;
  }

  v4l2_print_format(fmt);
  return true;
}

// IO METHODS
// ----------------------------------------------------
bool VideoCaptureV4L2::initializeMMAP(int fd) {
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof(req));
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if(v4l2_ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
    RX_ERROR("Cannot use mmap()");
    return false;
  }

  if(req.count < 2) {
    RX_ERROR("Insufficient buffer memory. ");
    return false;
  }

  for(int i = 0; i < req.count; ++i) {
    // create our reference to the mmap'd buffer
    VideoCaptureV4L2Buffer* buffer = new VideoCaptureV4L2Buffer();
    if(!buffer) {
      RX_ERROR("Cannot allocate the V4L2 buffer for mmap'd IO");
      goto error;
    }
    capture_buffers.push_back(buffer);

    // create the v4l2 buffer
    struct v4l2_buffer vbuf;
    memset(&vbuf, 0, sizeof(vbuf));
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    vbuf.index = i;

    // map the buffer.
    if(v4l2_ioctl(fd, VIDIOC_QUERYBUF, &vbuf) == -1) {
      RX_ERROR("Cannot query the buffer for index: %d", vbuf.index);
      goto error;
    }
    buffer->length = vbuf.length;
    buffer->start = mmap(NULL, /* start anywhere */
                         vbuf.length,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         fd, vbuf.m.offset);

    if(buffer->start == MAP_FAILED) {
      if(errno == EBADF) {
        RX_ERROR("Cannot map memory, fd is not a valid descriptor. (EBADF)");
      }
      else if(errno == EACCES) {
        RX_ERROR("Cannot map memory, fd is open for reading and writing. (EACCESS)");
      }
      else if(errno == EINVAL) {
        RX_ERROR("Cannot map memory, the start or length offset are not suitable. Flags or prot value is not supported. No buffers have been allocated. (EINVAL)");
      }
      else {
        RX_ERROR("MMAP failed.");
      }
      goto error;
    }
    
  } // for 

  return true;

 error:
  shutdownMMAP();
  return false;
}

bool VideoCaptureV4L2::shutdownMMAP() {
  if(io_method != IO_METHOD_MMAP) {
    RX_ERROR("The current IO method is not MMAP, so we can't free/release it");
    return false;
  }
  for(std::vector<VideoCaptureV4L2Buffer*>::iterator it = capture_buffers.begin(); it != capture_buffers.end(); ++it) {
    VideoCaptureV4L2Buffer* buf = *it;
    if(munmap(buf->start, buf->length) == -1) {
      RX_ERROR("Cannot unmap a memory buffer (?)");
    }
    delete buf;
  }
  capture_buffers.clear();
  return true;
}
