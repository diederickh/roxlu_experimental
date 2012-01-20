#ifndef ROXLU_TWEETCONSTANTSH
#define ROXLU_TWEETCONSTANTSH

#define CHECK_CURL_ERROR(result)	if((result) != CURLE_OK) { printf("Curl error: %s.\n", curl_easy_strerror((result))); return false; }
#define CHECK_CURLM_ERROR(result)	if((result) != CURLM_OK) { printf("Curl multi error: %s.\n", curl_multi_strerror((result))); return false; }


#endif
