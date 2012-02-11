#include "Curl.h"
namespace roxlu {
namespace curl {

// Constructor, creates curl handle.
// ---------------------------------------------
Curl::Curl()
	:is_initialized(false)
	,is_userpass_set(false)
	,is_callback_set(false)
	,header("")
	,curl(NULL)
{
	CURLcode r;
	
	// create curl handle.
	r = curl_global_init(CURL_GLOBAL_ALL);
	if(r != CURLE_OK) {
		printf("error: %s\n", curl_easy_strerror(r));
		return;
	}
	
	curl = curl_easy_init();
	if(!curl) {	
		printf("error: %s\n", curl_easy_strerror(r));
		curl_easy_cleanup(curl);
		curl = NULL;
	}
}

// Destructor; clean up when necessary.
// ---------------------------------------------
Curl::~Curl() {
	if(curl != NULL) {
		curl_easy_cleanup(curl);
		curl = NULL;
	}
}

void Curl::addHeader(const string& header) {
	headers.push_back(header);
}

void Curl::setHeaders(struct curl_slist** slist) {
	vector<string>::iterator it = headers.begin();
	while(it != headers.end()) {
		*slist = curl_slist_append(*slist, (*it).c_str());

		++it;
	}
}

bool Curl::doGet(const string& url) {
	CURLcode r;
	struct curl_slist* curl_header = NULL;
		
	// make sure userpass/callback is set.
	reset();
	
	// use GET
	r = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	CHECK_CURL_ERROR(r);
	
	// set URL
	r = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	CHECK_CURL_ERROR(r);
	
	// no CA verification
	r  = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); 
	CHECK_CURL_ERROR(r);
	
	// set header
	if(headers.size()) {
		setHeaders(&curl_header);
		if(curl_header) {
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_header);
		}
	}
		
	// perform request.
	bool result = true;
	r = curl_easy_perform(curl);
	if(r != CURLE_OK) {
		result = false;
	}
	
	// clear header.
	if(headers.size()) {
		curl_slist_free_all(curl_header);
		headers.clear();
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL); // opts are sticky
	}
	
	return result;
}

bool Curl::doPost(const string& url, const rcp::Collection& params, bool multiPart) {
	CURLcode r;
	struct curl_slist* curl_header = NULL;
	struct curl_httppost* post_curr = NULL;
	struct curl_httppost* post_last = NULL;
	string use_url = url; // @todo make url not const!s
			
	// Make sure userpass/callback is set.
	reset();
	
	// Remove the Expect: from header, can give problems... (when you get a 417 error)
	static const char buf[] = "Expect:";
  	curl_header = curl_slist_append(curl_header,buf);
	
	// Add fields to post.
	// ------------------------------------------------------
	if(!multiPart) {
		const list<rcp::Parameter*>& pars = params.getParameters();
		string data_str = createQueryString(pars);
		
		r = curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
		CHECK_CURL_ERROR(r);
		
		r = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, data_str.c_str());
		CHECK_CURL_ERROR(r);

	}	
	else {
	
		// @todo: 2012.02.03, we've to test this, I got some wierd things
		// with posting to twitter. I had to add all parameters to the 
		// query string and only was allowed to use files in the multipart 
		// body...
		//
		// for twitpic I had to use only the multipart body ...
		//
		//------------------------------
		// When we do a multi part, we add the parameters that are used 
		// to create the signature string as query string; this is just 
		// the ouath standard. Somehow adding them as mult-part doesnt work.

		/*  commented this because of testing with twitpic
		list<rcp::Parameter*> query_params = params.getParameters(true); 
		string qs = createQueryString(query_params);
		if(qs.length()) {
			//use_url = use_url + "?" + qs; 
		}
		*/

		// handling a multi part post.
		//const list<rcp::Parameter*>& post_params = params.getParameters(false); 
		const list<rcp::Parameter*>& post_params = params.getParameters(); 
		list<rcp::Parameter*>::const_iterator it = post_params.begin();	
	
		while(it != post_params.end()) {
			switch((*it)->type) {
				// This doesn't work somehow for twitter... I'm using the query string
				// now and urlencoding the values. But somehow it looks like
				// this string isn't added to the form.
				// update: 2012.02.03 this does work for twitpic
				// update: 2012.02.06 this also works for flickr
				case rcp::Parameter::PARAM_STRING: {
					curl_formadd(
							 &post_curr
							,&post_last
							,CURLFORM_COPYNAME
							,(*it)->getName().c_str()
							,CURLFORM_COPYCONTENTS
							,(*it)->getStringValue().c_str()
							,CURLFORM_END
					);
					break;
				}
				
				case rcp::Parameter::PARAM_FILE: {
					curl_formadd(
							 &post_curr
							,&post_last
							,CURLFORM_COPYNAME
							,(*it)->getName().c_str()
							,CURLFORM_FILE
							,(*it)->getStringValue().c_str()
							,CURLFORM_END
					);
					break;
				}
				default: {
					printf("error type not handled: %d\n", (*it)->type);
					break;
				}
			}
			++it;
		}
		
		r = curl_easy_setopt(curl, CURLOPT_HTTPPOST, post_curr); // only for multi-part
		CHECK_CURL_ERROR(r);
	}
	
	// no CA verification
	r  = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); 
	CHECK_CURL_ERROR(r);
		
	
	// set URL
	r = curl_easy_setopt(curl, CURLOPT_URL, use_url.c_str());
	CHECK_CURL_ERROR(r);
	
	// set header
	if(headers.size()) {
		setHeaders(&curl_header);
	}
	if(curl_header) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_header);
	}
			
	// perform request.
	bool result = true;
	r = curl_easy_perform(curl);
	if(r != CURLE_OK) {
		result = false;
	}
	
	curl_formfree(post_curr);
	
	// clear header.
	curl_slist_free_all(curl_header);
	headers.clear();
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL); // opts are sticky
	return result;
}

// @todo use Collection::getQueryString
string Curl::createQueryString(const list<rcp::Parameter*>& queryParams) {
	string qs;
	list<rcp::Parameter*>::const_iterator it = queryParams.begin();
	while(it != queryParams.end()) {
		
		qs.append(urlencode((*it)->getName()));
		qs.append("=");
		qs.append(urlencode((*it)->getStringValue()));
		
		++it;
		if(it != queryParams.end()) {
			qs.append("&");
		}	
	}
	return qs;
}

void Curl::reset() {
	buffer = "";
	setCallback();
	setUserPass();
}

void Curl::setCallback() {
	if(!is_callback_set) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		is_callback_set = true;
	}
}

void Curl::setUserPass() {
	if(is_userpass_set) {
		string userpass = auth_username +":" +auth_password;
		curl_easy_setopt(curl, CURLOPT_USERPWD, NULL); 
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass.c_str());
		is_userpass_set = true;
	}
}

size_t Curl::callback(char* data, size_t size, size_t nmemb, Curl* curlPtr) {
	size_t handled = size * nmemb;
	curlPtr->buffer.append(data, handled);
	return handled;
}

void Curl::setVerbose(bool verbose) {
	curl_easy_setopt(curl, CURLOPT_VERBOSE, (verbose) ? 1 : 0);	
}


}} // roxlu::twitter::curl
