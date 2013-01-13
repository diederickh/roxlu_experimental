#include <videocapture/VideoCaptureV4L2.h>

VideoCaptureV4L2::VideoCaptureV4L2() 
  :io_method(LINCAP_IO_METHOD_READ)
  ,is_opened(false)
  ,fd(0)
  ,width(0) 
  ,height(0) 
  ,frame_cb(NULL)
  ,frame_user(NULL)
{
}

VideoCaptureV4L2::~VideoCaptureV4L2() {
  closeDevice();
}

void VideoCaptureV4L2::setup() {
}

int VideoCaptureV4L2::openDevice(int device, int w, int h, VideoCaptureFormat capFormat) {
  std::vector<LinuxCaptureDevice> devices = getDeviceList();
  if(device >= devices.size()) {
    return 0;
  }

  LinuxCaptureDevice cap = devices[device];
  fd = open(cap.path.c_str(), O_RDWR | O_NONBLOCK, 0);
  if(fd == -1) {
    printf("ERROR: cannot open: %s\n", cap.path.c_str());
    fd = 0;
    return 0;
  }

  // query capabilities
  struct v4l2_capability caps;
  memset(&caps, 0, sizeof(caps));
  if (ioctl(fd, VIDIOC_QUERYCAP, &caps) == -1) {
    printf("ERROR: cannot detect capabilities of camera.\n");
    return 0;
  }

  // test if we can use this device as capture device
  if(!(caps.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    printf("ERROR: cannot use device for video capture.\n");
    return 0;
  }

  // check the io type
  io_method = LINCAP_IO_METHOD_READ;
  bool can_io_readwrite = (caps.capabilities & V4L2_CAP_READWRITE) == V4L2_CAP_READWRITE;
  bool can_io_stream =  (caps.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING;
  if(!can_io_readwrite && !can_io_stream) {
    printf("ERROR: cannot use read() or memory streaming with this device.\n");
    return 0;
  }
  if(can_io_stream) {
    io_method = LINCAP_IO_METHOD_MMAP;
  }

  if(!isFormatSupported(capFormat, w, h, 1)) {
    printf("ERROR: cannot set pixelformat and size: %dx%d\n", w,h);
    return 0;
  }

  width = w;
  height = h;

  // initialize IO
  if(io_method == LINCAP_IO_METHOD_MMAP) {
    initMMAP();
  }
  else if(io_method == LINCAP_IO_METHOD_READ) {
    printf("ERROR: this device can use read()/write() io, but we did not yet program this.\n");
    ::exit(0);
  }
  else if(io_method == LINCAP_IO_METHOD_USERPTR) {
    printf("ERROR: this device can use i/o with userptr, but we need to program it.\n");
    ::exit(EXIT_FAILURE);
  }
  is_opened = true;
  return 1;
}

int VideoCaptureV4L2::videoCaptureFormatToPixelFormat(VideoCaptureFormat fmt) {
  switch(fmt) {
    case VC_FMT_RGB24: return V4L2_PIX_FMT_RGB24;
    case VC_FMT_YUYV422: return V4L2_PIX_FMT_YUYV;
    case VC_FMT_UYVY422: return V4L2_PIX_FMT_UYVY;
    case VC_FMT_I420: return V4L2_PIX_FMT_YUV420;
    default: return 0;
  }
}

int VideoCaptureV4L2::isFormatSupported(VideoCaptureFormat fmt, int w, int h, int applyFormat) {
  if(!fd) {
    printf("ERROR: first open the device before testing a format.\n");
    return 0;
  }

  // As described in section 4.1.3, http://www.linuxtv.org/downloads/legacy/video4linux/API/V4L2_API/spec-single/v4l2.html#id2832544
  // we first retrieve the current pixel format and then adjust the settings to our needs before
  // trying the format.
  struct v4l2_format curr_fmt;
  memset(&curr_fmt, 0, sizeof(curr_fmt));
  curr_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(ioctl(fd, VIDIOC_G_FMT, &curr_fmt) == -1) {
    printf("ERROR: cannot get VIDIOC_G_FMT in isFormatSupported.\n");
    return 0;
  }

  curr_fmt.fmt.pix.width = w;
  curr_fmt.fmt.pix.height = h;
  curr_fmt.fmt.pix.pixelformat = videoCaptureFormatToPixelFormat(fmt);
  if(curr_fmt.fmt.pix.pixelformat == 0) {
    printf("ERROR: cannot convert VideoCaptureFormat to v4l2 pixelformat.\n");
    return 0;
  }

  if(ioctl(fd, VIDIOC_TRY_FMT, &curr_fmt) == -1) {
    printf("ERROR: the format is not supported!\n");
    return 0;
  }

  if(applyFormat) {
    printf("-----------------------------------------------\n");
    printV4L2Format(&curr_fmt);
    printf("-----------------------------------------------\n");
    if(ioctl(fd, VIDIOC_S_FMT, &curr_fmt) == -1) {
      printf("ERROR: cannot set new format; is the device busy maybe? @todo add some checking.\n");
      return 0;
    }
  }
  return 1;
}

int VideoCaptureV4L2::startCapture() {
  if(!is_opened) {
    printf("ERROR: cannot start capture; the device is not opened.\n");
    return 0;
  }
  switch(io_method) {
    case LINCAP_IO_METHOD_MMAP: {
      for(int i = 0; i < buffers.size(); ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if(ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
          printf("ERROR: cannot start capture because of invalid mmap buffer.\n");
          return 0;
        }
      }
      
      // STREAM ON!
      enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if(ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        printf("ERRORL cannot start capture.\n");
        return 0;
      }
      break;
    }
    default: {
      printf("ERROR: unhandled io method. (%d)\n", io_method);
      return 0;
    }
  }
  return 1;
}

int VideoCaptureV4L2::stopCapture() {
  if(!is_opened) {
    printf("ERROR: cannot stop capture.. we didn't start yet.\n");
    return 0;
  }

  if(io_method == LINCAP_IO_METHOD_MMAP) {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
      printf("ERROR: cannot VIDIOC_STREAMOFF.\n");
      return 0;
    }
    printf("VERBOSE: stopped capture.\n");
    return 1;
  }
  else {
    printf("@todo ERROR: we have only implement mmap i/o so we need to add some code to handle stopCapture.\n");
    return 0;
  }
  return 1;
}

void VideoCaptureV4L2::update() {
  if(!is_opened) {
    printf("WARNING: calling update() but not device opened.\n");
    return;
  }

  struct v4l2_buffer buf;
  
  switch(io_method) {
    case LINCAP_IO_METHOD_MMAP: {

      memset(&buf, 0, sizeof(buf));
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;

      if(ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {

        if(errno == EAGAIN) {
          //printf("VERBOSE: non-blocking, and no new frame in buffer (EAGAIN). \n");
          return;
        }
        else if(errno == EINVAL) {
          printf("ERROR: The buffer type is not supported, or the index is out of bound. no buffer allocated. (EINVAL)\n");
          return;
        }
        else if(EIO) {
          printf("ERROR: VIDIOC_DQBUF failed due to an internal error. VIDIOC_QBUF.(EIO)\n");
          return ;
        }
        else {
          printf("ERROR: cannot get buffer; unhandled..\n");
          return;
        }
      }

      if(frame_cb) {
        //printf("buf.index: %d, buf.bytes_used: %d, buffers[buf.index].length: %zu\n", buf.index, buf.bytesused, buffers[buf.index]->length);
        frame_cb(buffers[buf.index]->start, buffers[buf.index]->length, frame_user);
      }

      if(ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
        printf("ERROR: cannot enqueue buf again.\n");
        return;
      }
      break;
    }
    default: {
      printf("ERROR: unhandled io_method for update().\n");
      break;
    }
  }
}


// Initializes mmap io, see: http://linuxtv.org/downloads/v4l-dvb-apis/capture-example.html
bool VideoCaptureV4L2::initMMAP() {
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof(req));
  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if(ioctl(fd, VIDIOC_REQBUFS, &req) == -1) { // req.count will be set to the actual number of created buffers
    printf("ERROR: cannot use mmap().\n");
    return false;
  }
  
  if(req.count < 2) {
    printf("ERROR: Insufficient buffer memory. - @todo test this/check this, copied from: http://linuxtv.org/downloads/v4l-dvb-apis/capture-example.html\n");
    return false;
  }

  for(int i = 0; i < req.count; i++) {
    LinuxCaptureBuffer* buffer = new LinuxCaptureBuffer();
    memset(buffer, 0, sizeof(LinuxCaptureBuffer));
    buffers.push_back(buffer);
    
    struct v4l2_buffer vbuf;
    memset(&vbuf, 0, sizeof(vbuf));
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    vbuf.index = i;

    // map the buffer
    if(ioctl(fd, VIDIOC_QUERYBUF, &vbuf) == -1) {
      printf("ERROR: cannot query mmap buffer, for index: %d\n", vbuf.index);
      return false;
    }
    buffer->length = vbuf.length;
    buffer->start = mmap(NULL /* start anywhere */,
                         vbuf.length,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED, 
                         fd, vbuf.m.offset);

    if(buffer->start == MAP_FAILED) {
      if(errno == EBADF) {
        printf("ERROR: cannot map memory, fd is not a valid descriptor. (EBADF)\n");
        return false;
      }
      else if(errno == EACCES) {
        printf("ERROR cannot map memory, fd is not open for reading and writing. (EACCES)\n");
        return false;
      }
      else if(errno == EINVAL) {
        printf("ERROR cannot map memory, the start or length or offset are not suitable. Flags or prot value is not supported. No buffers have been allocated. (EINVAL)\n");
        return false;
      }
    }
  }
  return true;
}

int VideoCaptureV4L2::closeDevice() {
  if(!is_opened) {
    return 0;
  }

  // Turn off streaming, i/o
  stopCapture();

  // Unmap mappe dmemory
  if(io_method == LINCAP_IO_METHOD_MMAP) {
    for(int i = 0; i < buffers.size(); ++i) {
      printf("Unmapping MMAP buffer: %d/%zu\n",i, buffers.size());
      LinuxCaptureBuffer* b = buffers[i];
      if(munmap(b->start, b->length) == -1) {
        printf("ERROR: cannot unmap buffer: %d\n", i);
      }
      delete b;
    }
    buffers.clear();
  }
  else {
    printf("ERROR: cannot close device because it's using an I/O method we haven't programmed yet.\n");
    return 0;
  }

  printf("Closing capture file descriptor.\n");
  if(close(fd) == -1) {
    printf("ERROR: cannot close device.\n");
  }

  is_opened = false;
  fd = 0;
  return 1;
}

int VideoCaptureV4L2::printVerboseInfo() {
  if(!is_opened) {
    printf("ERROR: cannot print verbose info when no device has been opened.\n");
    return 0;
  }
  return printDeviceInfo(fd);
}

int VideoCaptureV4L2::printDeviceInfo(int dev) {
  struct v4l2_capability capability;
  struct v4l2_standard standard;
  struct v4l2_input input;
  struct v4l2_fmtdesc fmtdesc;
  struct v4l2_format format;
  
  printf("--------------------------------------------\n");
  memset(&capability, 0, sizeof(capability));
  if (ioctl(dev, VIDIOC_QUERYCAP, &capability) == -1) {
    printf("ERROR: Cannot query device capabilities.\n");
    return -1;
  }
  
  printf("capability.driver: %s\n", capability.driver);
  printf("capability.card: %s\n", capability.card);
  printf("capability.bus_info: %s\n", capability.bus_info);
  printf("capability.version: %u.%u.%u\n", 
         (capability.version >> 16) & 0xFF, 
         capability.version >> 8 & 0xFF, 
         capability.version & 0xFF);
  printf("--------------------------------------------\n");

  // list standards
  // @todo: complete this
  for(int i = 0; ; ++i) {
    memset(&standard, 0, sizeof(standard));
    standard.index = i;
    if(ioctl(dev, VIDIOC_ENUMSTD, &standard) == -1) {
      break;
    }
    printf("standard.index: %d\n", standard.index);
    printf("standard.name: %s\n", standard.name);
    printf("--------------------------------------------\n");
  }

  // list inputs
  // @todo complete list.
  for(int i = 0; ; ++i) {
    input.index = i;
    if(ioctl(dev, VIDIOC_ENUMINPUT, &input) == -1) {
      break;
    }
    printf("input.index: %d\n", input.index);
    printf("input.name: %s\n", input.name);
    printf("input.type: %d\n", input.type);
    printf("--------------------------------------------\n");
  }

  // video capture capabilities
  if(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
    for(int i = 0; ; ++i) {
      memset(&fmtdesc, 0, sizeof(fmtdesc));
      fmtdesc.index = i;
      fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == -1) {
        break;
      }
      printf("fmtdesc.index: %d\n", fmtdesc.index);
      printf("fmtdesc.description: %s\n", fmtdesc.description);
      char* f = (char*)&fmtdesc.pixelformat;
      printf("fmtdesc.pixelformat: %c%c%c%c - %s\n", f[0], f[1], f[2], f[3], v4l2PixelFormatToString(fmtdesc.pixelformat).c_str());
      printf("fmtdesc.flags, V4L2_FMT_FLAG_COMPRESSED: %d\n", (fmtdesc.flags & V4L2_FMT_FLAG_COMPRESSED) == V4L2_FMT_FLAG_COMPRESSED);
      printf("fmtdesc.flags, V4L2_FMT_FLAG_EMULATED: %d\n", (fmtdesc.flags & V4L2_FMT_FLAG_EMULATED) == V4L2_FMT_FLAG_EMULATED);

      // frame sizes and fps intervals for this type.
      struct v4l2_frmsizeenum frames;
      frames.index = 0;
      frames.pixel_format = fmtdesc.pixelformat;
      while(!ioctl(dev, VIDIOC_ENUM_FRAMESIZES, &frames)) {
        if(frames.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
          printf("\t-----------------\n");
          struct v4l2_frmivalenum fpse;
          fpse.index = 0;
          fpse.pixel_format = frames.pixel_format;
          fpse.width = frames.discrete.width;
          fpse.height = frames.discrete.height;
          while(!ioctl(dev, VIDIOC_ENUM_FRAMEINTERVALS, &fpse)) {
            if(fpse.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
              printf("\t%ux%u @ %d fps\n", frames.discrete.width, frames.discrete.height, fpse.discrete.denominator);
            }
            fpse.index++;
          }

        }
        frames.index++;
      }
      printf("--------------------------------------------\n");
    }
  }
  printf("V4L2_CAP_STREAMING: %d\n", (capability.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING);
  printf("V4L2_CAP_VIDEO_CAPTURE: %d\n", (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE));
  printf("V4L2_CAP_VIDEO_CAPTURE_MPLANE: %d\n", (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE));
  printf("V4L2_CAP_READWRITE: %d\n", (capability.capabilities & V4L2_CAP_READWRITE));
  printf("--------------------------------------------\n");

  return 0;
}
int VideoCaptureV4L2::listDevices() {
  std::vector<LinuxCaptureDevice> devices = getDeviceList();
  if(!devices.size()) {
    printf("WARNING: no video4linux devices found\n");
    return 0;
  }

  struct v4l2_input inputs[V4L2_MAX_QUERYABLE_INPUTS];
  int c = 0; 
  for(std::vector<LinuxCaptureDevice>::iterator it = devices.begin(); it != devices.end(); ++it) {
    printf("[%d] %s, (idVendor: %s, idProduct: %s)\n", c++, (*it).path.c_str(), (*it).id_vendor.c_str(), (*it).id_product.c_str());
  }
  return devices.size();
}

// List of devices: http://www.ideasonboard.org/uvc/ 
// How to use libudev: http://www.signal11.us/oss/udev/
std::vector<LinuxCaptureDevice> VideoCaptureV4L2::getDeviceList() {
  std::vector<LinuxCaptureDevice> found_devices;
  struct udev* udev;
  struct udev_enumerate* enumerate;
  struct udev_list_entry* devices;
  struct udev_list_entry* dev_list_entry;
  struct udev_device* dev;

  udev = udev_new();
  if(!udev) {
    printf("ERROR: cannot udev_new()\n");
    return found_devices;
  }

  enumerate = udev_enumerate_new(udev);
  udev_enumerate_add_match_subsystem(enumerate, "video4linux");
  udev_enumerate_scan_devices(enumerate);
  devices = udev_enumerate_get_list_entry(enumerate);

  udev_list_entry_foreach(dev_list_entry, devices) {
    LinuxCaptureDevice linux_device;

    const char* path = udev_list_entry_get_name(dev_list_entry);
    dev = udev_device_new_from_syspath(udev, path);

    const char* devpath = udev_device_get_devnode(dev);
    linux_device.path.assign(devpath, strlen(devpath));
    
    dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
    if(!dev) {
      printf("VERBOSE: cannot find related usb device.\n");
      found_devices.push_back(linux_device);
      continue;
    }

    const char* id_vendor = udev_device_get_sysattr_value(dev, "idVendor");
    linux_device.id_vendor.assign(id_vendor, strlen(id_vendor));

    const char* id_product = udev_device_get_sysattr_value(dev, "idProduct");
    linux_device.id_product.assign(id_product, strlen(id_product));

    found_devices.push_back(linux_device);
  }

  udev_enumerate_unref(enumerate);
  udev_unref(udev);
  return found_devices;
}

void VideoCaptureV4L2::printV4L2Format(v4l2_format* fmt) {
  if(!fmt) {
    return;
  }
  if(fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    printf("v4l2_format.type: %s\n", v4l2BufTypeToString(fmt->type).c_str());
    printf("v4l2_format.fmt.width: %d\n", fmt->fmt.pix.width);
    printf("v4l2_format.fmt.height: %d\n", fmt->fmt.pix.height);
    printf("v4l2_format.fmt.pixelformat: %s\n", v4l2PixelFormatToString(fmt->fmt.pix.pixelformat).c_str());
  }
}

std::string VideoCaptureV4L2::v4l2BufTypeToString(int type) {
  switch(type) {
    case V4L2_BUF_TYPE_VIDEO_CAPTURE: return "V4L2_BUF_TYPE_VIDEO_CAPTURE";
    case V4L2_BUF_TYPE_VIDEO_OUTPUT: return "V4L2_BUF_TYPE_VIDEO_OUTPUT"; 
    case V4L2_BUF_TYPE_VIDEO_OVERLAY: return "V4L2_BUF_TYPE_VIDEO_OVERLAY";  
    case V4L2_BUF_TYPE_VBI_CAPTURE: return "V4L2_BUF_TYPE_VBI_CAPTURE"; 
    case V4L2_BUF_TYPE_VBI_OUTPUT: return "V4L2_BUF_TYPE_VBI_OUTPUT";  
    case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE: return "V4L2_BUF_TYPE_SLICED_VBI_CAPTURE";  
    case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT: return "V4L2_BUF_TYPE_SLICED_VBI_OUTPUT";  
    case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY: return "V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY"; 
    default: return "UNKNOWN";
  };
}

// http://www.linuxtv.org/downloads/legacy/video4linux/API/V4L2_API/spec-single/v4l2.html#rgb-formats
std::string VideoCaptureV4L2::v4l2PixelFormatToString(int pixfmt) {
  switch(pixfmt) {
    case V4L2_PIX_FMT_RGB332: return "V4L2_PIX_FMT_RGB332";
    case V4L2_PIX_FMT_RGB444: return "V4L2_PIX_FMT_RGB444";
    case V4L2_PIX_FMT_RGB555: return "V4L2_PIX_FMT_RGB555";
    case V4L2_PIX_FMT_RGB565: return "V4L2_PIX_FMT_RGB565";
    case V4L2_PIX_FMT_RGB555X: return "V4L2_PIX_FMT_RGB555X"; 
    case V4L2_PIX_FMT_RGB565X: return "V4L2_PIX_FMT_RGB565X";
    case V4L2_PIX_FMT_BGR24: return "V4L2_PIX_FMT_BGR24";
    case V4L2_PIX_FMT_RGB24: return "V4L2_PIX_FMT_RGB24";
    case V4L2_PIX_FMT_BGR32: return "V4L2_PIX_FMT_BGR32";
    case V4L2_PIX_FMT_RGB32: return "V4L2_PIX_FMT_RGB32";
    case V4L2_PIX_FMT_SBGGR8: return "V4L2_PIX_FMT_SBGGR8";
    case V4L2_PIX_FMT_SGRBG8: return "V4L2_PIX_FMT_SGRBG8";
    case V4L2_PIX_FMT_SBGGR16: return "V4L2_PIX_FMT_SBGGR16";
    case V4L2_PIX_FMT_YUV444: return "V4L2_PIX_FMT_YUV444";
    case V4L2_PIX_FMT_YUV555: return "V4L2_PIX_FMT_YUV555";
    case V4L2_PIX_FMT_YUV565: return "V4L2_PIX_FMT_YUV565";
    case V4L2_PIX_FMT_YUV32: return "V4L2_PIX_FMT_YUV32";
    case V4L2_PIX_FMT_GREY: return "V4L2_PIX_FMT_GREY";
    case V4L2_PIX_FMT_Y16: return "V4L2_PIX_FMT_Y16";
    case V4L2_PIX_FMT_YUYV: return "V4L2_PIX_FMT_YUYV";
    case V4L2_PIX_FMT_UYVY: return "V4L2_PIX_FMT_UYVY";
    case V4L2_PIX_FMT_YVYU: return "V4L2_PIX_FMT_YVYU";
    case V4L2_PIX_FMT_VYUY: return "V4L2_PIX_FMT_VYUY";
    case V4L2_PIX_FMT_Y41P: return "V4L2_PIX_FMT_Y41P";
    case V4L2_PIX_FMT_YVU420: return "V4L2_PIX_FMT_YVU420";
    case V4L2_PIX_FMT_YUV420: return "V4L2_PIX_FMT_YUV420";
    case V4L2_PIX_FMT_YVU410: return "V4L2_PIX_FMT_YVU410";
    case V4L2_PIX_FMT_YUV410: return "V4L2_PIX_FMT_YUV410";
    case V4L2_PIX_FMT_YUV422P: return "V4L2_PIX_FMT_YUV422P";
    case V4L2_PIX_FMT_YUV411P: return "V4L2_PIX_FMT_YUV411P";
    case V4L2_PIX_FMT_NV12: return "V4L2_PIX_FMT_NV12";
    case V4L2_PIX_FMT_NV21: return "V4L2_PIX_FMT_NV21";
    case V4L2_PIX_FMT_NV16: return "V4L2_PIX_FMT_NV16";
    case V4L2_PIX_FMT_NV61: return "V4L2_PIX_FMT_NV61";
    case V4L2_PIX_FMT_JPEG: return "V4L2_PIX_FMT_JPEG";
    case V4L2_PIX_FMT_MPEG: return "V4L2_PIX_FMT_MPEG";
    default: return "UNKNOWN";
  }
}

