#include <roxlu/core/Log.h>
#include <youtube/YouTubeTypes.h>

YouTubeVideo::YouTubeVideo() 
  :bytes_uploaded(0)
  ,bytes_total(0)
  ,id(0)
  ,datapath(false)
{
}

void YouTubeVideo::print() {
  RX_VERBOSE("filename: %s", filename.c_str());
  RX_VERBOSE("video_resource_json: %s", video_resource_json.c_str());
  RX_VERBOSE("upload_url: %s", upload_url.c_str());
  RX_VERBOSE("bytes_uploaded: %ld", bytes_uploaded);
  RX_VERBOSE("bytes_total: %ld", bytes_total);
  RX_VERBOSE("id: %ld", id);
  RX_VERBOSE("IPC: filename stored in datapath: %c", (datapath) ? 'Y' : 'N');
  RX_VERBOSE("IPC: title: `%s`", title.c_str());
}
