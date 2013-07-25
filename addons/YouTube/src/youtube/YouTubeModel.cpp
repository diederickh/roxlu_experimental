#include <youtube/YouTubeModel.h>
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <jansson.h>


// ----------------------------------------------------------------------------------

YouTubeModel::YouTubeModel() {
  if(!db.open("youtube.db", true)) {
    RX_ERROR("Cannot open database");
    ::exit(EXIT_FAILURE);
  }

  {
    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS videos ("
             "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
             "    filename TEXT, "
             "    bytes_uploaded INTEGER DEFAULT 0,"
             "    bytes_total INTEGER DEFAULT 0,"
             "    state INTEGER DEFAULT 0,"
             "    video_resource_json TEXT,"
             "    upload_url TEXT )").execute(qr);
    qr.finish();
  }

  {
    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS state("
             "   name TEXT UNIQUE, "
             "   value TEXT)").execute(qr);
    qr.finish();
  }

}

YouTubeModel::~YouTubeModel() {
}

bool YouTubeModel::addVideoToUploadQueue(std::string filename, bool datapath) {

  std::string filepath = filename;
  if(datapath) {
    filepath = rx_to_data_path(filename);
  }

  size_t fsize = rx_get_file_size(filepath);
  if(!fsize) {
    RX_ERROR("Filesize of %s returned %ld", filepath.c_str(), fsize);
    return false;
  }
  
  //json_t* body = json_pack("{s:{s:s}, s:{s:s}}", "snippet", "title", "test", "status", "privacyStatus", "private");
  //char* video_resource = json_dumps(body, JSON_INDENT(0));
  char video_resource[1024 * 10];
  sprintf(video_resource, "{\"status\": {\"privacyStatus\": \"private\"}, \"snippet\": {\"tags\": null, \"categoryId\": \"%d\", \"description\": \"Test Description\", \"title\": \"%s\"}}",
          22
          ,"test2");
  /*
  if(!video_resource) {
    RX_ERROR("Cannot create JSON video resource string");
    json_decref(body);
    return false;
  }
  */


  bool r = db.insert("videos")
    .use("filename", filename)
    .use("state", YT_VIDEO_STATE_NONE)
    .use("bytes_total", fsize)
    .use("video_resource_json", video_resource)
    .execute();
  
  return r;
}
void YouTubeModel::setRefreshToken(std::string rtoken) {
  if(!db.insert("state").use("name", "refresh_token").use("value", rtoken).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the refresh token");
  }
}

void YouTubeModel::setAccessToken(std::string atoken, uint64_t timeout) {
  if(!db.insert("state").use("name", "access_token").use("value", atoken).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the access token");
  }

  if(!db.insert("state").use("name", "token_timeout").use("value", timeout).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the token timeout");
  }
}

int YouTubeModel::hasVideoInUploadQueue(int state) {
  std::stringstream ss;
  ss << "state = " << state;

  QueryResult qr(db);

  bool r = db.select("id").from("videos").where(ss.str()).limit(1).execute(qr);
  if(!r) {
    RX_ERROR("Cannot check if there are videos in the upload queue.");
    return false;
  }

  qr.next();
  int64_t id = qr.getInt(0);
  return id;
}

bool YouTubeModel::setVideoState(int id, int state) {
  return db.update("videos").use("state", state).where("id", id).execute();
}

bool YouTubeModel::setVideoUploadURL(int id, std::string url) {
  return db.update("videos").use("upload_url", url).where("id", id).execute();
}

YouTubeVideo YouTubeModel::getVideo(int id) {
  YouTubeVideo video;
  QueryResult qr(db);

  bool r = db.select("id, filename, upload_url, bytes_uploaded, bytes_total, video_resource_json").from("videos").execute(qr);
  if(!r) {
    RX_ERROR("Error while trying to get a video from the upload queue: %d", id);
    return video;
  }
  qr.next();

  video.id = qr.getInt(0);
  video.filename = qr.getString(1);
  video.upload_url = qr.getString(2);
  video.bytes_uploaded = qr.getInt(3);
  video.bytes_total = qr.getInt(4);
  video.video_resource_json = qr.getString(5);

  return video;
}
