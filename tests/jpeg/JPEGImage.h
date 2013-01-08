#ifndef ROXLU_JPEGIMAGEH
#define ROXLU_JPEGIMAGEH

#include "../../roxlu/experimental/Image.h"

#include "include/jpeglib.h"
//#include "jpeglib.h"
#include <string>
#include <tr1/memory>

using std::string;
namespace roxlu {

class JPEGImage : public Image {
public:
	JPEGImage();
	JPEGImage(string filePath);
	virtual ~JPEGImage();
	virtual bool loadImage(string filePath);

};

} // roxlu
#endif