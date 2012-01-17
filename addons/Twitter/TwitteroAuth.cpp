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
	printf("URL: %s\n", url.c_str());
	printf("Found: %lu -- %lu\n", pos, string::npos);
	if(pos != string::npos) {
		printf("getHeader - parsing url %lu\n", pos);
		printf("url:%s\n", url.c_str());
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
	printf("\n\nHEADER:'%s'\n", header.c_str());
	getStringFromKeyValuePairs(key_values, params, sep);
	printf("\nPARAMS:\n'%s'\n\n", params.c_str());
	header.assign(roxlu::twitter::AUTHHEADER_STRING);
	header.append(params);
	return (header.length()) ? true : false;
}
/*
Authorization: OAuth oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw",oauth_nonce="baea213007110aed26f20b79e9e0cfa1", oauth_signature="%2FGk4yKQf07U%2FBj8KXGdTdZffIkg%3D", oauth_signature_method="HMAC-SHA1", oauth_timestamp="1326839965", oauth_token="67066605-4xouV1ySq5yqiOUU6VOXcOE7SiB1CCoPcDh7Ptb59", oauth_version="1.0"
Authorization: OAuth oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw",oauth_nonce="13268411561bc",oauth_signature="kadcKvtj1hE1Qzq%2BYml9mMLTj0I%3D",oauth_signature_method="HMAC-SHA1",oauth_timestamp="1326841156",oauth_token="466622389-Osbd3Mm1SDVLOqugzCQ5y6MP1RkLMw81VIREB5NR",oauth_version="1.0"
*/

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
			,const map<string, string>& keyValues
			,string& signature /* out */
)
{
	// @todo implement a smarter way of key-value parsing and encoding for the signature. 
	// i.e. see: https://dev.twitter.com/apps/1527994/oauth?nid=46 for some examples
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
	printf("Signature base string:'%s'\n", sig_base.c_str());
	return (signature.length()) ? true : false;
}
/*
GET&http%3A%2F%2Fapi.twitter.com%2F1%2Fstatuses%2Fhome_timeline.json%3Fcount%3D20&count%3D20%26oauth_consumer_key%3Dkyw8bCAWKbkP6e1HMMdAvw%26oauth_nonce%3D13268414151c9%26oauth_signature_method%3DHMAC-SHA1%26oauth_timestamp%3D1326841415%26oauth_token%3D466622389-Osbd3Mm1SDVLOqugzCQ5y6MP1RkLMw81VIREB5NR%26oauth_version%3D1.0
GET&http%3A%2F%2Fapi.twitter.com%2F1%2Fstatuses%2Fhome_timeline.json&count%3D2%26include_entities%3Dtrue%26oauth_consumer_key%3Dkyw8bCAWKbkP6e1HMMdAvw%26oauth_nonce%3Dbaea213007110aed26f20b79e9e0cfa1%26oauth_signature_method%3DHMAC-SHA1%26oauth_timestamp%3D1326839965%26oauth_token%3D67066605-4xouV1ySq5yqiOUU6VOXcOE7SiB1CCoPcDh7Ptb59%26oauth_version%3D1.0

*/


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
	if(npos != string::npos) {
		npos = npos +roxlu::twitter::TOKENSECRET_KEY.length() +1;
		dummy = buffer.substr(npos);
		npos = dummy.find("&");
		if(npos != string::npos) {
			token_secret = dummy.substr(0, npos);
		}
	}	
	//printf("Tokenkey: '%s', Token secret: '%s'\n", token_key.c_str(), token_secret.c_str()); 	
	return true;
}

}; // roxlu