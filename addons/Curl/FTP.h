#ifndef ROXLU_CURL_FTP
#define ROXLU_CURL_FTP

#include <string>
#include <deque>
#include <fstream>
#include "Curl.h"
#include "../../libs/curl/curl.h"

using std::string;
using std::deque;

namespace roxlu {
namespace curl {

struct FTPFile {
	string url;
	string remotepath;
	CURL* curl;
};

class FTP {
public:
	FTP();
	~FTP();
	bool setup(const string& host, const string& username, const string& password, string remotepath = "", bool verbose = false);
	bool addFile(const string& filepath, string remotepath = "");
	void update();
	
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
	deque<FTPFile> files;
};

}} // roxlu::curl
#endif