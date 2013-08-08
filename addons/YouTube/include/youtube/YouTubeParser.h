/*

 # YouTubeParser

 This file contains functions to parse the json responses that the API receives in JSON format.

 */

#ifndef ROXLU_YOUTUBE_PARSER_H
#define ROXLU_YOUTUBE_PARSER_H

#include <youtube/YouTubeTypes.h>
#include <jansson.h>
#include <vector>

bool youtube_parse_errors(std::string json, std::vector<YouTubeError>& resultzb);
bool youtube_parse_video_resource(std::string json, YouTubeVideo& result);

#endif
