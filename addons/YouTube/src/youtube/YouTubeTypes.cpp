#include <roxlu/core/Log.h>
#include <youtube/YouTubeTypes.h>

YouTubeVideo::YouTubeVideo() 
  :bytes_uploaded(0)
  ,bytes_total(0)
  ,id(0)
  ,datapath(false)
  ,category(22)
  ,privacy_status("private")
{
}

void YouTubeVideo::print() {
  RX_VERBOSE("-----------------------------------------------");
  RX_VERBOSE("video.filename: %s", filename.c_str());
  RX_VERBOSE("video.video_resource_json: %s", video_resource_json.c_str());
  RX_VERBOSE("video.upload_url: %s", upload_url.c_str());
  RX_VERBOSE("video.title: `%s`", title.c_str());
  RX_VERBOSE("video.description: %s", description.c_str());
  RX_VERBOSE("video.tags: %s", tags.c_str());
  RX_VERBOSE("video.privacy_status: %s", privacy_status.c_str());
  RX_VERBOSE("video.bytes_uploaded: %ld", bytes_uploaded);
  RX_VERBOSE("video.bytes_total: %ld", bytes_total);
  RX_VERBOSE("video.id: %ld", id);
  RX_VERBOSE("video.datapath: %c", (datapath) ? 'Y' : 'N');
  RX_VERBOSE("-----------------------------------------------");
}
