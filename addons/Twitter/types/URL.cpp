#include "URL.h"
#include "TweetConstants.h"

namespace roxlu {
namespace twitter {
namespace type {

URL::URL(string url)
	:type(NONE) 
	,url(url)
	,is_downloaded(false) 
	,is_initialized(false)
	,file_extension("jpg")
	,fs(NULL)
	,curl(NULL)
	,curlm(NULL)
{
	detectURLType();
}

URL::~URL() {
	if(fs != NULL) {
		if(fs->is_open()) {
			fs->close();
		}
		delete fs;
	}
	cleanup();
}

URL& URL::operator=(const URL& o) {
	url = o.url;
	type = o.type;
	return *this;
}

void URL::detectURLType() {
	size_t pos = url.find("twitpic");
	if(pos != std::string::npos) {
		type = TWITPIC;
	}
}

void URL::update() {
	int still_running = 0;
	CURLMcode r = curl_multi_perform(curlm, &still_running);

	if(still_running == 0) {
		if(fs != NULL) {
			fs->close();
		}
		is_downloaded = true;
		cleanup();
	}
}

bool URL::download(string fromURL, string name) {
	if(is_downloaded) {
		return false;
	}
	file_name = name;
	curl = curl_easy_init();
	if(curl == NULL) {
		return false;
	}
	
	is_initialized = true;
	
	// no timeout signals.
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	
	
	// set the url to download
	curl_easy_setopt(
					  curl
					 ,CURLOPT_URL
					 ,fromURL.c_str()
					);
	
	// Make sure to follow http redirects/moved (403)
	curl_easy_setopt(
					  curl
					 ,CURLOPT_FOLLOWLOCATION
					 ,true
					);
	
	// set the write function which store the file
	curl_easy_setopt(
					  curl
					 ,CURLOPT_WRITEFUNCTION
					 ,&URL::writeData
					 );
					 
	// callback for headers (used when downloading files)
	curl_easy_setopt(
					curl
					,CURLOPT_HEADERFUNCTION
					,&URL::writeHeader
					);
	
	// the userpointer (this object)
	curl_easy_setopt(
					 curl
					 ,CURLOPT_WRITEDATA
					 ,this
					 );
	curl_easy_setopt(
					 curl
					 ,CURLOPT_WRITEHEADER
					 ,this
					 );				 
	
	// we use the multi handles because of async-io
	curlm = curl_multi_init();
	if(curlm == NULL) {
		printf("Error: cannot create multi handle to download url...");
		return false;
	}
	
	CURLMcode t = curl_multi_add_handle(curlm, curl);
	CHECK_CURLM_ERROR(t);
	
	return true;
}

bool URL::createFile() {
	if(fs != NULL) {
		return true;
	}
	
	fs = new std::ofstream();
	file_name += "." +file_extension;
	fs->open(file_name.c_str(), std::ios::binary);
	if(!fs->is_open()) {
		printf("Error: cannot download file, because we cant create a file handle.\n");
		return false;
	}
	return true;
}

size_t URL::writeHeader( void *ptr, size_t size, size_t nmemb, void *userdata) {
	size_t bytes_to_write = size * nmemb;
	URL* url = static_cast<URL*>(userdata);
	char* data_ptr = static_cast<char*>(ptr);
	string str(data_ptr);

	// parse content type.
	if(str.find("Content-Type") != std::string::npos) {
		if(str.find("image/jpeg") != std::string::npos) {
			url->file_extension = "jpg";
			url->createFile();
		}
		else if(str.find("image/png") != std::string::npos) {
			url->file_extension = "png";
			url->createFile();
		}
		else if(str.find("image/gif") != std::string::npos) {
			url->file_extension = "gif";
			url->createFile();
		}
		else {
			printf("Eror: Unhandled mime type: '%s' in URL.\n", str.c_str());
		}
	}
	return bytes_to_write;
}

size_t URL::writeData(void *ptr, size_t size, size_t nmemb, void* obj) {
	size_t bytes_to_write = size * nmemb;
	URL* url = static_cast<URL*>(obj);
	if(url->fs != NULL) {
		char* data_ptr = static_cast<char*>(ptr);
		url->fs->write(data_ptr, bytes_to_write);
	}
	else {

	}
	return bytes_to_write;
}


bool URL::cleanup() {
	if(is_initialized) {
		CURLMcode r = curl_multi_remove_handle(curlm, curl);
		CHECK_CURLM_ERROR(r);	
		curl_easy_cleanup(curl);
		r = curl_multi_cleanup(curlm);
		CHECK_CURLM_ERROR(r);	
	}
	return true;
}

string URL::getTwitPicCode() {
	if(!isTwitPic()) {
		return "";
	}
	string code;
	try {
		code = url.substr(19);
	}
	catch(...) {
		printf("Error while trying to get twitpic code: %s\n", url.c_str());
		return "";
	}
	return code;
}

string URL::getTwitPicURL() {
	string code;
	try {
		code = url.substr(19);
	}
	catch(...) {
		printf("Error: twitpic url does not have code: %s\n", url.c_str());
		return "";
	}
	return "http://twitpic.com/show/full/" +code;
}

}}} // roxlu::twitter::type