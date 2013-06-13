#include <videocapture/linux/v4l2/VideoCaptureV4L2Types.h>
#include <roxlu/core/Log.h>

// ----------------------------------------------------

VideoCaptureV4L2Device::VideoCaptureV4L2Device()
  :version_major(0)
  ,version_minor(0)
  ,version_micro(0)
{
}

void VideoCaptureV4L2Device::print() {
  RX_VERBOSE("VideoCaptureV4L2Device.path: %s", path.c_str());
  RX_VERBOSE("VideoCaptureV4L2Device.id_vendor: %s", id_vendor.c_str());
  RX_VERBOSE("VideoCaptureV4L2Device.id_product: %s", id_product.c_str());
  RX_VERBOSE("VideoCaptureV4L2Device.driver: %s", driver.c_str());
  RX_VERBOSE("VideoCaptureV4L2Device.card: %s", card.c_str());
  RX_VERBOSE("VideoCaptureV4L2Device.bus_info: %s", bus_info.c_str());
  RX_VERBOSE("VideoCaptureV4L2Device.version: %u.%u.%u", version_major, version_minor, version_micro);
}

// ----------------------------------------------------

VideoCaptureV4L2Buffer::VideoCaptureV4L2Buffer()
  :length(0)
  ,start(NULL)
{
}
