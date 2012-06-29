#include "FTP.h"

namespace roxlu {
namespace curl {

FTP::FTP() 
	:curlm(NULL)
	,still_running(0)
	,verbose(false)
	,progress_listener(NULL)
{
}
	
FTP::~FTP() {
	if(curlm != NULL) {
		curl_multi_cleanup(curlm);
		for (int i=0; i< files.size(); i++) {
			// TODO: check for memory leak here!!!!!!
			if(files[i]->headerlist != NULL) {
				curl_slist_free_all(files[i]->headerlist);
			}
			curl_easy_cleanup(files[i]->curl);
			delete files[i];
		}
	}
}

void FTP::clear() {
	for (int i = 0; i< files.size(); i++) {
		if(files[i]->headerlist != NULL) {
			curl_slist_free_all(files[i]->headerlist);
		}
		curl_easy_cleanup(files[i]->curl);
		delete files[i];
	}
	files.clear();
}

bool FTP::update() {
	curl_multi_perform(curlm, &still_running);
	if(still_running == 0) {
		if(verbose) {
			printf(">> num files: %zu, still running: %d.\n", files.size(), still_running);
		}
		return true;
	}
	if(verbose) {
		printf(">> num files: %zu, still running: %d.\n", files.size(), still_running);
	}
	return false;
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

int FTP::onUploadProgress(FTPFile* ftpFile, double t, double d, double ultotal, double ulnow) {
	printf("t=%fL d=%fL ultotal=%fL ulnow=%fL total size: %zu f=%s\n", t, d, ultotal, ulnow, ftpFile->file_size, ftpFile->remotepath.c_str());
	ftpFile->bytes_uploaded = ulnow;
	if(ftpFile->bytes_uploaded > 0.0) {
		ftpFile->upload_percentage = ftpFile->bytes_uploaded / (float)ftpFile->file_size;
	}
	if(ftpFile->ftp->progress_listener != NULL) {
		ftpFile->ftp->progress_listener->onProgress(ftpFile);
	}	
	return 0;
}

bool FTP::addFile(const string& filepath, string remotepath, string remotefile) {
	FTPFile* f = new FTPFile();
	f->ftp = this;
	
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
	f->curl = curl_easy_init();
	if(f->curl == NULL) {
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
	f->remotepath = ((remotepath.size()) ? remotepath : remotedir) + filename;
	f->url = "ftp://" +username +"@" +host +"/" +f->remotepath;

	FILE* fp = fopen(filepath.c_str(), "rb");
	if(fp == NULL) {
		curl_easy_cleanup(f->curl);
		if(verbose) {
			printf("Error: cannot open the file for reading.\n");
		}
		return false;
	}
	
	fseek(fp, 0L, SEEK_END);
	f->file_size  = ftell(fp);
	f->bytes_uploaded = 0;
	f->upload_percentage = 0;
	fseek(fp, 0L, SEEK_SET);
	
	// when we need to rename the remote file, add the post commands.
	f->headerlist = NULL;
	if(remotefile.size()) {
		string rn_from = "RNFR " +f->remotepath;
		string rn_to = "RNTO " +remotefile;
		f->headerlist = curl_slist_append(f->headerlist, rn_from.c_str());
		f->headerlist = curl_slist_append(f->headerlist, rn_to.c_str());
		curl_easy_setopt(f->curl, CURLOPT_POSTQUOTE, f->headerlist);
		
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// TODO: we need to clear this headerlist somewhere!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	
	cc = curl_easy_setopt(f->curl, CURLOPT_READDATA, fp);
	CHECK_CURL_ERROR(cc);
	
	// enable uploading
	cc = curl_easy_setopt(f->curl, CURLOPT_UPLOAD, 1L);
	CHECK_CURL_ERROR(cc);
	
	// no signals on i.e. resolv timouts
	curl_easy_setopt(f->curl, CURLOPT_NOSIGNAL, 1); 
	
	// set upload path.
	cc = curl_easy_setopt(f->curl, CURLOPT_URL, f->url.c_str());
	CHECK_CURL_ERROR(cc);
	
	// username
	cc = curl_easy_setopt(f->curl, CURLOPT_USERNAME, username.c_str());
	CHECK_CURL_ERROR(cc);

	// password
	cc = curl_easy_setopt(f->curl, CURLOPT_PASSWORD, password.c_str());
	CHECK_CURL_ERROR(cc);	
	
	// verbose
	if(verbose) {
		cc = curl_easy_setopt(f->curl, CURLOPT_VERBOSE, true);
		CHECK_CURL_ERROR(cc);
	}
	

	// progress 
	cc = curl_easy_setopt(f->curl, CURLOPT_NOPROGRESS, 0L);
	CHECK_CURL_ERROR(cc);
	
	cc = curl_easy_setopt(f->curl, CURLOPT_PROGRESSFUNCTION, &FTP::onUploadProgress);
	CHECK_CURL_ERROR(cc);
	
	cc = curl_easy_setopt(f->curl, CURLOPT_PROGRESSDATA, f);
	CHECK_CURL_ERROR(cc);

	// add the file to the multi handle.
	ccm = curl_multi_add_handle(curlm, f->curl);
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
	return true;
}

void FTP::setProgressListener(FTPProgressListener* listener) {
	progress_listener = listener;
}

float FTP::getTotalProgress() {
	if(files.size() == 0) {
		return 0.0;
	}
	
	float num_files = files.size();
	float total_perc = 0;
	deque<FTPFile*>::iterator it = files.begin();
	while(it != files.end()) {
		total_perc += (*it)->upload_percentage;
		++it;
	}
	return total_perc / num_files;
}

}} // roxlu::curl