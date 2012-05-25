#ifndef ROXLU_CURL_FTP
#define ROXLU_CURL_FTP

#include <string>
#include <deque>
#include <fstream>
#include "Curl.h"

// Include 32 or 64 bit version.
#include <stdint.h>
#if INTPTR_MAX == INT32_MAX
	#include "../../external/includes/curl32/curl.h"
#elif INTPTR_MAX == INT64_MAX
	#include "../../external/includes/curl64/curl.h"
#endif



using std::string;
using std::deque;

namespace roxlu {
namespace curl {

class FTP;

struct FTPFile {
	FTPFile() {
	}
	
	string url;
	string remotepath;
	CURL* curl;
	struct curl_slist *headerlist;
	double bytes_uploaded;
	size_t file_size;
	double upload_percentage;
	FTP* ftp;
};


struct FTPProgressListener {
	virtual void onProgress(FTPFile* file) = 0;
};

class FTP {
public:
	FTP();
	~FTP();
	bool setup(const string& host, const string& username, const string& password, string remotepath = "", bool verbose = false);
	bool addFile(const string& filepath, string remotepath = "", string remotefile="");
	bool update();
	void clear();
	
	static int onUploadProgress(FTPFile* ftpFile, double t, double d, double ultotal, double ulnow);
	void setProgressListener(FTPProgressListener* listener);
	float getTotalProgress();
	FTPProgressListener* progress_listener;
	
private:
	bool initCurl();
	bool verbose;
	string url;
	string host;
	string username;
	string password;
	string remotedir;
	int still_running;
	CURLM* curlm;
	deque<FTPFile*> files;
};

}} // roxlu::curl
#endif