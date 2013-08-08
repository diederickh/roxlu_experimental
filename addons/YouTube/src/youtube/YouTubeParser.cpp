#include <youtube/YouTubeParser.h>

bool youtube_parse_errors(std::string json, std::vector<YouTubeError>& errors) {

  if(!json.size()) {
    RX_ERROR("The given video resource json is empty; cannot parse.");
    return false;
  }

  json_error_t err;
  json_t* root = json_loads(json.c_str(), 0, &err);

  if(!root) {
    RX_ERROR("Error while parsing the error json: %d, %s", err.line, err.text);
    json_decref(root);
    root = NULL;
    return false;
  }

  json_t* el = json_object_get(root,"error");
  if(!el) {
    RX_ERROR("Although we received an error we did not receive an error json string.. ");
    json_decref(root);
    root = NULL;
    return false;
  }

  json_t* el_errors = json_object_get(el, "errors");
  if(!el_errors) {
    RX_ERROR("Cannot find the errors field");
    json_decref(root);
    root = NULL;
    return false;
  }

  for(int i = 0; i < json_array_size(el_errors); ++i) {

    json_t* error = json_array_get(el_errors, i);
    if(!json_is_object(error)) {
      RX_ERROR("Invalid error json");
      json_decref(root);
      root = NULL;
      return false;
    }

    json_t* el_domain = json_object_get(error, "domain");
    if(!json_is_string(el_domain)) {
      RX_ERROR("Cannot parse the error domain field");
      json_decref(root);
      root = NULL;
      return false;
    }
                     
    json_t* el_reason = json_object_get(error, "reason");
    if(!json_is_string(el_reason)) {
      RX_ERROR("Cannot parse the reason field from the error");
      json_decref(root);
      root = NULL;
      return false;
    }

    json_t* el_message = json_object_get(error, "message");
    if(!json_is_string(el_message)) {
      RX_ERROR("Cannot parse the message field from the error");
      json_decref(el_message);
      root = NULL;
      return false;
    }
      
    YouTubeError yt_err;
    yt_err.domain = json_string_value(el_domain);
    yt_err.reason = json_string_value(el_reason);
    yt_err.message = json_string_value(el_message);
    errors.push_back(yt_err);

  }

  return true;
}


bool youtube_parse_video_resource(std::string json, YouTubeVideo& video) {

  if(!json.size()) {
    RX_ERROR("The given video resource json is empty; cannot parse.");
    return false;
  }

  json_error_t err;
  json_t* root = json_loads(json.c_str(), 0, &err);

  if(!root) {
    RX_ERROR("Error while parsing the video resource json: %d, %s", err.line, err.text);
    json_decref(root);
    root = NULL;
    return false;
  }


  json_t* el_etag = json_object_get(root, "etag");
  if(!json_is_string(el_etag)) {
    RX_ERROR("Cannot find etag in response code");
    json_decref(root);
    root = NULL;
    return false;
  }
    
  json_t* el_id = json_object_get(root, "id");
  if(!json_is_string(el_id)) {
    RX_ERROR("Cannot find the ID of the uploaded video in the json response");
    json_decref(root);
    root = NULL;
    return false;
  }

  json_t* el_snip = json_object_get(root, "snippet");
  if(!json_is_object(el_snip)) {
    RX_ERROR("Cannot find the snippet object in the video upload response");
    json_decref(root);
    root = NULL;
    return false;
  }

  json_t* el_pub_at = json_object_get(el_snip, "publishedAt");
  if(!json_is_string(el_pub_at)) {
    RX_ERROR("Cannot find the publishedAt field in the video result json");
    json_decref(root);
    root = NULL;
    return false;
  }
    
  json_t* el_chan_id = json_object_get(el_snip, "channelId");
  if(!json_is_string(el_chan_id)) {
    RX_ERROR("Cannot find the channelId in the video result json");
    json_decref(root);
    root = NULL;
    return false;
  }

  video.yt_id = json_string_value(el_id);
  video.yt_etag = json_string_value(el_etag);
  video.yt_published_at = json_string_value(el_pub_at);
  video.yt_channel_id = json_string_value(el_chan_id);

  json_decref(root);
  root = NULL;

  return true;
}
