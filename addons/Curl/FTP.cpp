#include "FTP.h"

namespace roxlu {
namespace curl {

FTP::FTP() 
	:curlm(NULL)
	,still_running(0)
	,verbose(false)
{
}
	
FTP::~FTP() {
	if(curlm != NULL) {
		curl_multi_cleanup(curlm);
		for (int i=0; i< files.size(); i++) {
			curl_easy_cleanup(files[i].curl);
		}
	}
}

void FTP::update() {
	curl_multi_perform(curlm, &still_running);
	if(still_running == 0) {
		if(verbose) {
			printf(">> num files: %zu, still running: %d.\n", files.size(), still_running);
		}
		return;
	}
	if(verbose) {
		printf(">> num files: %zu, still running: %d.\n", files.size(), still_running);
	}
}

bool FTP::setup(const string& server, const string& uname, const string& pw, string remotepath, bool verbose) {
	verbose = verbose;
	host = server;
	url = "ftp://" +host +"/";
	username = uname;
	password = pw;
	remotedir = remotepath;	
	return initCurl();
}

bool FTP::addFile(const string& filepath, string remotepath) {
	FTPFile f;
	CURLcode cc;
	CURLMcode ccm;
	string filename;	
	size_t pos = 0;

	// did we setup correctly?
	if(curlm == NULL) {
		if(verbose) {
			printf("Error: call setup first.\n");
		}
		return false;
	}
	
	// check if file exist.
	std::ifstream ifs(filepath.c_str());
	if(!ifs.is_open()) {
		if(verbose) {
			printf("Error: cannot find file: '%s'\n", filepath.c_str());
		}
		return false;
	}
	ifs.close();

	// create new curl handle.
	f.curl = curl_easy_init();
	if(f.curl == NULL) {
		if(verbose){
			printf("Error: cannot create easy_curl handle.\n");
		}
		return false;
	}
	
	// find filename; primitive test...
	pos = filepath.find_last_of("/");
	if(pos == std::string::npos) {
		if(verbose) {
			printf("Warning: cannot find path in file string.\n");
		}
		filename = filepath;
	}
	else {
		filename = filepath.substr(pos+1);
	}
	f.remotepath = ((remotepath.size()) ? remotepath : remotedir) + filename;
	f.url = "ftp://" +host +"/" +f.remotepath;

	FILE* fp = fopen(filepath.c_str(), "rb");
	if(fp == NULL) {
		curl_easy_cleanup(f.curl);
		if(verbose) {
			printf("Error: cannot open the file for reading.\n");
		}
		return false;
	}
	curl_easy_setopt(f.curl, CURLOPT_READDATA, fp);
	CHECK_CURL_ERROR(cc);
	
	// enable uploading
	cc = curl_easy_setopt(f.curl, CURLOPT_UPLOAD, 1L);
	CHECK_CURL_ERROR(cc);
	
	// no signals on i.e. resolv timouts
	curl_easy_setopt(f.curl, CURLOPT_NOSIGNAL, 1); 
	
	// set upload path.
	cc = curl_easy_setopt(f.curl, CURLOPT_URL, f.url.c_str());
	CHECK_CURL_ERROR(cc);
	
	// username
	cc = curl_easy_setopt(f.curl, CURLOPT_USERNAME, username.c_str());
	CHECK_CURL_ERROR(cc);

	// password
	cc = curl_easy_setopt(f.curl, CURLOPT_PASSWORD, password.c_str());
	CHECK_CURL_ERROR(cc);	
	
	// verbose
	cc = curl_easy_setopt(f.curl, CURLOPT_VERBOSE, true);
	CHECK_CURL_ERROR(cc);
	
	// add the file to the multi handle.
	ccm = curl_multi_add_handle(curlm, f.curl);
	CHECK_CURLM_ERROR(ccm);

	files.push_back(f);
	
	return true;
}

bool FTP::initCurl() {
	curlm = curl_multi_init();
	if(curlm == NULL) {	
		if(verbose) {
			printf("Error: creating multi handle.\n"); 
		}
		return false;
	}
}

}} // roxlu::curl