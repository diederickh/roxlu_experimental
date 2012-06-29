#ifndef ROXLU_OPENNI_RECORDERH
#define ROXLU_OPENNI_RECORDERH

#include <OpenNI/XnCppWrapper.h>
#include <OpenNI/XnTypes.h>
#include <string> 

using std::string;

namespace roxlu {
namespace openni {

namespace ro = roxlu::openni;

class OpenNI;

#define MAX_STRINGS 20

typedef struct NodeCapturingData {
	XnCodecID 		capture_format;
	XnUInt32 		num_captured_frames;
	bool 			is_recording;
	xn::Generator* 	generator;
} NodeCapturingData;


typedef struct {
	int 		values_count;
	XnCodecID 	values[MAX_STRINGS];
	string 		index_to_name[MAX_STRINGS];
} NodeCodec;

class Recorder {

public:
	Recorder();
	~Recorder();

	bool setup(
				 ro::OpenNI& ni
				,const string& filePath
				,XnCodecID depthFormat=XN_CODEC_16Z_EMB_TABLES
				,XnCodecID imageFormat=XN_CODEC_JPEG
				,XnCodecID irFormat=XN_CODEC_NULL
				,XnCodecID audioFormat=XN_CODEC_NULL
			);

	bool start();
	bool stop();
	void update();
	void setStartDelay(XnUInt32 seconds);
	
private:
	XnStatus captureFrame();
	
	enum CapturingState	{
		 NOT_CAPTURING
		,SHOULD_CAPTURE
		,CAPTURING
	};

	enum CaptureNodeType {
		 CAPTURE_DEPTH_NODE
		,CAPTURE_IMAGE_NODE
		,CAPTURE_IR_NODE
		,CAPTURE_AUDIO_NODE
		,CAPTURE_NODE_COUNT
	};

	NodeCapturingData nodes[CAPTURE_NODE_COUNT];
	xn::Recorder* recorder;
	string filepath;
	XnUInt32 start_on; // time to start, in seconds
	bool skip_first_frame;
	CapturingState state;
	XnUInt32 captured_frame_id;
	string display_message;

	NodeCodec depth_format;
	NodeCodec image_format;
	NodeCodec ir_format;
	NodeCodec audio_format;
	ro::OpenNI* ni;
};

inline void Recorder::setStartDelay(XnUInt32 delay) {
	start_on = delay;
}

}} // roxlu::openni

#endif