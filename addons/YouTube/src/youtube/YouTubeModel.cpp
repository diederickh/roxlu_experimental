#include <youtube/YouTubeModel.h>
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <jansson.h>
#include <sstream>

/*
alter table videos add column retries INTEGER DEFAULT 0
 */

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
             "    title TEXT, "                              // title for the video as it appears on youtube
             "    description TEXT, "                        // description for the video as it appears on youtube 
             "    tags TEXT, "                               // tags for the youtube video
             "    privacy_status TEXT DEFAULT \"private\", " // private status for youtube 
             "    bytes_uploaded INTEGER DEFAULT 0,"         // - not used atm -
             "    bytes_total INTEGER DEFAULT 0,"            // total bytes of the file 
             "    state INTEGER DEFAULT 0,"                  // used to upload only videos which haven't been uploaded yet
             "    datapath INTEGER DEFAULT 0,"               // is the filename relative to the datapase of the runtime, 0 = no, 1 = yes
             "    video_resource_json TEXT,"                 // the json that we post as body when uploading 
             "    category INTEGER DEFAULT 2,"              // the category for the video on youtube
             "    retries INTEGER DEFAULT 0,"                // number of times we retried to upload the video after receiving an upload error
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

// @todo - we need to construct the correct video resource json
bool YouTubeModel::addVideoToUploadQueue(YouTubeVideo video) {

  if(!video.filename.size()) {
    RX_ERROR("Cannot add a video which has no filename set");
    return false;
  }

  std::string filepath = video.filename;
  if(video.datapath) {
    filepath = rx_to_data_path(video.filename);
  }

  size_t fsize = rx_get_file_size(filepath);
  if(!fsize) {
    RX_ERROR("Filesize of %s returned %ld", filepath.c_str(), fsize);
    return false;
  }

#if USE_JSON_PACK
  // @todo - libjansson is used as DLL, and json_dumps
  // is supposed to free() the memory; this is icky and 
  // libjansson should free() any allocated mem as it's used
  // as a dll
  std::string video_json;
  char* video_resource = NULL;
  json_t* body = NULL;
  
  if(!video.video_resource_json.size()) {
    body = json_pack("{ s: {s:s}, s: { s:s, s:i, s:s, s:s } }",
                             "status", "privacyStatus", "private", 
                             "snippet", "tags", video.tags.c_str(), 
                             "categoryId", video.category,  
                             "description", video.description.c_str(), 
                             "title", video.title.c_str());
 
    video_resource = json_dumps(body, JSON_INDENT(0));
    json_decref(body);

    video_json.assign(video_resource, strlen(video_resource));

    if(!video_resource) {
      RX_ERROR("Cannot create JSON video resource string");
      return false;
    }
  }
  else {
    video_json = video.video_resource_json;
  }
#else
  std::string video_json;

  if(!video.video_resource_json.size()) {
    std::stringstream ss;

    ss << "{ \"status\": { \"privacyStatus\" : \"private\" }, "
       <<   "\"snippet\": {"
       <<       "\"title\":\"" << video.title << "\", "
       <<       "\"tags\":\"" << video.tags << "\", "
       <<       "\"categoryId\":" << video.category << ", "
       <<       "\"description\":\"" << video.description << "\""
       <<    "}"
       << "}";

    video_json = ss.str();
  }
  else {
    video_json = video.video_resource_json;
  }

#endif

  bool r = db.insert("videos")
    .use("filename", video.filename)
    .use("state", YT_VIDEO_STATE_NONE)
    .use("bytes_total", fsize)
    .use("video_resource_json", video_json)
    .use("datapath", (video.datapath) ? 1 : 0)
    .use("title", video.title)
    .use("description", video.description)
    .use("tags", video.tags)
    .use("privacy_status", video.privacy_status)
    .use("category", video.category)
    .execute();

#if USE_JSON_PACK
  if(video_resource) {
    free(video_resource);  
    video_resource = NULL;
  }
#endif

  if(r) {
    RX_VERBOSE("Added video to the queue: %s", video.filename.c_str());
  }
  else {
    RX_ERROR("Error while trying to add: %s to the video upload queue", video.filename.c_str());
  }

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
  std::string tmp = ss.str();

  QueryResult qr(db);

  bool r = db.select("id").from("videos").where(ss.str()).order("id DESC").limit(1).execute(qr);
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

bool YouTubeModel::setVideoBytesUploaded(int id, size_t nbytes) {
  return db.update("videos").use("bytes_uploaded", nbytes).where("id", id).execute();
}

bool YouTubeModel::incrementVideoBytesUploaded(int id, size_t nbytes) {
  std::stringstream ss;
  ss << "update videos set bytes_uploaded += " << nbytes << " where id = \"" << id << "\"";
  QueryResult qr(db);
  db.query(ss.str()).execute(qr);
  return qr.finish();
}

int YouTubeModel::getNumberOfRetries(int id) {
  QueryResult qr(db);
  bool r = db.select("retries").from("videos").where("id", id).execute(qr);
  if(!r) {
    RX_ERROR("Cannot retrieve the number of retries for video: %d", id);
    return -1;
  }
  qr.next();
  return qr.getInt(0);
}

bool YouTubeModel::incrementNumberOfRetries(int id) {

  std::stringstream ss;
  ss << "update videos set retries = retries + 1 where id = \"" << id << "\"";
  std::string sql = ss.str();

  QueryResult qr(db);
  db.query(sql).execute(qr);

  if(!qr.finish()) {
    RX_ERROR("Incrementing the number of retries for video: %d failed", id);
    return false;
  }

  return true;
}

YouTubeVideo YouTubeModel::getVideo(int id) {
  YouTubeVideo video;
  QueryResult qr(db);

  bool r = db.select("id, filename, upload_url, bytes_uploaded, bytes_total, video_resource_json")
    .from("videos")
    .where("id", id)
    .execute(qr);

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
