#include "TwitteroAuth.h"
#include <iostream>
#include <map>
#include "../../libs/crypto/urlencode.h"
#include "../../libs/crypto/HMAC_SHA1.h"
#include "../../libs/crypto/base64.h"

using std::map;

namespace roxlu {

bool TwitteroAuth::getHeader(
				 const TwitteroAuth::RequestType type
				,const string url
				,const string data
				,string& header
				,const bool includePin 
)
{
	map<string, string> key_values;
	string params;
	string signature;
	string sep;
	string base_url(url);
	
	header = "";
	
	// extract key=value pairs.
	size_t pos = url.find_first_of("?");
	if(pos != string::npos) {
		base_url = url.substr(0, pos);
		string data_part = url.substr(pos+1);
		size_t nsep = string::npos;
		size_t npos = string::npos;
		string data_val;
		string data_key;
		string data_keyval;
		
		while( (nsep = data_part.find_first_of("&")) != string::npos) {
			data_keyval = data_part.substr(0,nsep);
			npos = data_keyval.find_first_of("=");
			if(npos != string::npos) {
				data_key = data_keyval.substr(0,npos);
				data_val = data_keyval.substr(npos + 1);
				key_values[data_key] = urlencode(data_val);
			}
			data_part = data_part.substr(nsep + 1);
		}
		
		// last part.
		data_keyval = data_part.substr(nsep + 1);
		npos = data_keyval.find_first_of("=");
		if(string::npos != npos) {
			data_key = data_keyval.substr(0,npos);
			data_val = data_keyval.substr(npos+1);
			key_values[data_key] = urlencode(data_val);
		}
	}
	
	buildoAuthTokenKeyValuePairs(includePin, data, "", key_values, true);
	getSignature(type, url, key_values, signature);			
	buildoAuthTokenKeyValuePairs(includePin,"", signature, key_values, false); // with signature
	
	// get oauth header.
	sep = ",";
	getStringFromKeyValuePairs(key_values, params, sep);
	header.assign(roxlu::twitter::AUTHHEADER_STRING);
	header.append(params);
	return (header.length()) ? true : false;
}


bool TwitteroAuth::buildoAuthTokenKeyValuePairs(
				const bool includePin
				,const string& data
				,const string& signature
				,map<string, string>& keyValues
				,const bool generateTimestamp
)
{
	if(generateTimestamp) {
		updateNonce();
	}
	
	keyValues[twitter::CONSUMERKEY_KEY] = consumer_key;
	keyValues[twitter::NONCE_KEY] = nonce;
	
	if(signature.length()) {
		keyValues[twitter::SIGNATURE_KEY] = signature;
	}
	
	keyValues[twitter::SIGNATUREMETHOD_KEY] = "HMAC-SHA1";
	keyValues[twitter::TIMESTAMP_KEY] = timestamp;
	
	if(token_key.length()) {
		keyValues[twitter::TOKEN_KEY] = token_key;
	}
	
	if(includePin && pin.length()) {
		keyValues[twitter::VERIFIER_KEY] = pin;
	}
	
	keyValues[twitter::VERSION_KEY] = "1.0";
	
	if(data.length()) {
		string dummy_key;
		string dummy_val;
		size_t npos = data.find_first_of("=");
		if(string::npos != npos) {
			dummy_key = data.substr(0,npos);
			dummy_val = data.substr(npos+1);
			keyValues[dummy_key] = dummy_val;
		}
	}
	return keyValues.size() ? true : false;
}

	
bool TwitteroAuth::getSignature(
			 const RequestType type
			,const string& url
			,map<string, string>& keyValues
			,string& signature /* out */
)
{
	string params;
	string sig_base;
	string sep = "&";
	signature = "";
	getStringFromKeyValuePairs(keyValues, params, sep);
	
	switch(type) {
		case TWITTER_OAUTH_GET: {
			sig_base = "GET&";
			break;
		}
		case TWITTER_OAUTH_POST: {
			sig_base = "POST&";
			break;
		}
		case TWITTER_OAUTH_DELETE: {
			sig_base = "DELETE&";
			break;
		}
		default: {
			return false;
			break;
		}
	};
	sig_base.append(urlencode(url));
	sig_base.append("&");
	sig_base.append(urlencode(params));
	
	// create hash.
	CHMAC_SHA1 mac;
	string sign_key;
	unsigned char digest[1024];
	memset(digest, 0, 1024);
	sign_key.assign(consumer_secret);
	sign_key.append("&");
	
	if(token_secret.length()) {
		sign_key.append(token_secret);	
	}
	
	mac.HMAC_SHA1(
				(unsigned char*)sig_base.c_str()
				,sig_base.length()
				,(unsigned char*)sign_key.c_str()
				,sign_key.length()
				,digest
			);
			
	string base64_encoded = base64_encode(digest, 20);
	signature = urlencode(base64_encoded);
	return (signature.length()) ? true : false;
}


bool TwitteroAuth::TwitteroAuth::getStringFromKeyValuePairs(
			const map<string, string>& keyValues
			,string& params /* out */
			,const string& paramSep
)
{
	params = "";
	if(keyValues.size()) {
		// quote key values
		list<string> keyvalues_list;
		string dummy;
		map<string,string>::const_iterator it = keyValues.begin();
		while(it != keyValues.end()) {
			dummy.assign(it->first);
			dummy.append("=");
			
			if(paramSep == ",") { 
				dummy.append("\"");
			}
			
			dummy.append(it->second);
			
			if(paramSep == ",") {
				dummy.append("\"");
			}
			
			keyvalues_list.push_back(dummy);
			++it;
		}
		
		// create url
		keyvalues_list.sort();
		dummy.assign("");
		list<string>::iterator it_list = keyvalues_list.begin();
		while(it_list != keyvalues_list.end()) {
			if(dummy.length()) {
				dummy.append(paramSep);
			}
			dummy.append(it_list->c_str());
			++it_list;
		}
		
		// finalize...
		params.assign(dummy);
	}
	return (params.length()) ? true : false;
	
}

void TwitteroAuth::updateNonce() {
	char buf_time[512];
	char buf_rand[512];
	memset(buf_time, 0, 512);
	memset(buf_rand, 0, 512);
	srand(time(NULL));
	sprintf(buf_time, "%ld", time(NULL));
	sprintf(buf_rand, "%x", rand()%1000);
	nonce.assign(buf_time);
	nonce.append(buf_rand);
	timestamp.assign(buf_time);
	
	printf("Time: %s, nonce:%s\n", timestamp.c_str(), nonce.c_str()); 
}


bool TwitteroAuth::extractTokenKeyAndSecret(const string& buffer) {
	// @todo implement
	if(!buffer.length()) {
		return false;
	}
	
	// find oauth_token value
	size_t npos = string::npos;
	string dummy;
	npos = buffer.find(roxlu::twitter::TOKEN_KEY);
	if(npos != string::npos) {
		npos = npos +roxlu::twitter::TOKEN_KEY.length()+1;
		dummy = buffer.substr(npos);
		npos = dummy.find("&");
		if(npos != string::npos) {
			token_key = dummy.substr(0,npos);
		}
	}
	
	// find oauth_token_secret
	npos = buffer.find(roxlu::twitter::TOKENSECRET_KEY);
	if(npos = string::npos) {
		npos = npos +roxlu::twitter::TOKENSECRET_KEY.length() +1;
		dummy = buffer.substr(npos);
		npos = dummy.find("&");
		if(npos != string::npos) {
			token_secret = dummy.substr(0, npos);
		}
	}	
	
	printf("Token_Key: '%s', Token_Secret: '%s'\n", token_key.c_str(), token_secret.c_str()); 	
	return true;
}

}; // roxlu