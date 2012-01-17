#include "TwitterCurlValueTypeFile.h"

namespace roxlu {

TwitterCurlValueTypeFile::TwitterCurlValueTypeFile(const string& name, const string& filePath)
	:name(name)
	,filepath(filePath)
{
}


TwitterCurlValueTypeFile::~TwitterCurlValueTypeFile() {
	printf("~ file\n");
}


void TwitterCurlValueTypeFile::addToPost(CURL* curl, curl_httppost** curr, curl_httppost** last) {
	curl_formadd(
		 curr
		,last
		,CURLFORM_COPYNAME
		,name.c_str()
		,CURLFORM_FILE
		,filepath.c_str()
		,CURLFORM_END
	);
}

void TwitterCurlValueTypeFile::addToGet(string& data) {
	// a file isnt added as data string.
}

}; // roxlu