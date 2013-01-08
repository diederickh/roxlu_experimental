#include <videocapture/V4L2Capture.h>

V4L2Capture::V4L2Capture() 
  :io_method(LINCAP_IO_METHOD_READ)
  ,is_opened(false)
  ,fd(0)
  ,width(640) // @todo hardcoded for now
  ,height(480) // @todo hardcoded for now
{
}

V4L2Capture::~V4L2Capture() {
  printf("~V4LCapture()\n");
  closeDevice();
}

void V4L2Capture::setup() {
}

int V4L2Capture::openDevice(int device) {
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
  
  // try a specific format
  struct v4l2_format fmt;
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_ANY; // @todo check what is best to use here
  if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
    printf("ERROR: cannot set video format.\n");
  }

  // initialize IO
  if(io_method == LINCAP_IO_METHOD_MMAP) {
    printf("INIT MMAP\n");
    initMMAP();
  }
  else if(io_method == LINCAP_IO_METHOD_READ) {
    printf("ERROR: this device can use read()/write() io, but we did not yet program this.\n");
    ::exit(0);
  }
  else if(io_method == LINCAP_IO_METHOD_USERPTR) {
    printf("ERROR: this device can use i/o with userptr, but we need to program it.\n");
    ::exit(0);
  }
  printf("DONE\n");
         // printDeviceInfo(fd);
  is_opened = true;
  return 1;
}

int V4L2Capture::startCapture() {
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

int V4L2Capture::stopCapture() {
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

void V4L2Capture::update() {
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
      printf("buf.index: %d, buf.bytes_used: %d\n", buf.index, buf.bytesused);
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
bool V4L2Capture::initMMAP() {
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
        printf("ERROR cannot map memory, the start or lengt or offset are not suitable. Flags or prot value is not supported. No buffers have been allocated. (EINVAL)\n");
        return false;
      }
    }
  }
  return true;
}

int V4L2Capture::closeDevice() {
  if(!is_opened) {
    return 0;
  }

  stopCapture();

  if(io_method == LINCAP_IO_METHOD_MMAP) {
    for(int i = 0; i < buffers.size(); ++i) {
      LinuxCaptureBuffer* b = buffers[i];
      if(munmap(b->start, b->length) == -1) {
        printf("ERROR: cannot unmap buffer: %d\n", i);
      }
      delete b;
    }
  }
  else {
    printf("ERROR: cannot close device because it's using an I/O method we haven't programmed yet.\n");
    return 0;
  }


  if(close(fd) == -1) {
    printf("ERROR: cannot close device.\n");
  }

  is_opened = false;
  fd = 0;
  return 1;
}

int V4L2Capture::printVerboseInfo() {
  if(!is_opened) {
    printf("ERROR: cannot print verbose info when no device has been opened.\n");
    return 0;
  }
  return printDeviceInfo(fd);
}

int V4L2Capture::printDeviceInfo(int dev) {
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
      printf("fmtdesc.pixelformat: %c%c%c%c\n", f[0], f[1], f[2], f[3]);
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

int V4L2Capture::listDevices() {
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
std::vector<LinuxCaptureDevice> V4L2Capture::getDeviceList() {
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
