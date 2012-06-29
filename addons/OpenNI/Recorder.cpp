#include "Recorder.h"
#include "OpenNI.h"

namespace roxlu {
namespace openni {

#define START_CAPTURE_CHECK_RC(rc, what)								\
	if (status != XN_STATUS_OK)											\
	{																	\
		printf("Failed! %s",xnGetStatusString(rc));						\
		delete recorder;												\
		recorder = NULL;												\
		return false;													\
	}


Recorder::Recorder() {
	// Depth Formats
	int index = 0;

	depth_format.values[index] = XN_CODEC_16Z_EMB_TABLES;
	depth_format.index_to_name[index] = "PS Compression (16z ET)";
	index++;

	depth_format.values[index] = XN_CODEC_UNCOMPRESSED;
	depth_format.index_to_name[index] = "Uncompressed";
	index++;

	depth_format.values[index] = XN_CODEC_NULL;
	depth_format.index_to_name[index] = "Not Captured";
	index++;

	depth_format.values_count = index;

	// Image Formats
	index = 0;

	image_format.values[index] = XN_CODEC_JPEG;
	image_format.index_to_name[index] = "JPEG";
	index++;

	image_format.values[index] = XN_CODEC_UNCOMPRESSED;
	image_format.index_to_name[index] = "Uncompressed";
	index++;

	image_format.values[index] = XN_CODEC_NULL;
	image_format.index_to_name[index] = "Not Captured";
	index++;

	image_format.values_count = index;

	// IR Formats
	index = 0;

	ir_format.values[index] = XN_CODEC_UNCOMPRESSED;
	ir_format.index_to_name[index] = "Uncompressed";
	index++;

	ir_format.values[index] = XN_CODEC_NULL;
	ir_format.index_to_name[index] = "Not Captured";
	index++;

	ir_format.values_count = index;

	// Audio Formats
	index = 0;

	audio_format.values[index] = XN_CODEC_UNCOMPRESSED;
	audio_format.index_to_name[index] = "Uncompressed";
	index++;

	audio_format.values[index] = XN_CODEC_NULL;
	audio_format.index_to_name[index] = "Not Captured";
	index++;

	audio_format.values_count = index;

	// Init
	filepath[0] = 0;
	state = NOT_CAPTURING;
	captured_frame_id = 0;
	display_message[0] = '\0';
	skip_first_frame = false;

	nodes[CAPTURE_DEPTH_NODE].capture_format = XN_CODEC_16Z_EMB_TABLES;
	nodes[CAPTURE_IMAGE_NODE].capture_format = XN_CODEC_JPEG;
	nodes[CAPTURE_IR_NODE].capture_format = XN_CODEC_NULL;
	nodes[CAPTURE_AUDIO_NODE].capture_format = XN_CODEC_NULL;

	recorder = NULL;
	start_on = 0;
}

Recorder::~Recorder() {
	stop();
}



bool Recorder::setup(
				 ro::OpenNI& openNI
				,const string& filePath
				,XnCodecID depthFormat
				,XnCodecID imageFormat
				,XnCodecID irFormat
				,XnCodecID audioFormat
)
{
	ni = &openNI;
	filepath = filePath;

	nodes[CAPTURE_DEPTH_NODE].capture_format = depthFormat;
	nodes[CAPTURE_IMAGE_NODE].capture_format = imageFormat;
	nodes[CAPTURE_IR_NODE].capture_format = irFormat;
	nodes[CAPTURE_AUDIO_NODE].capture_format = audioFormat;

	XnStatus status = XN_STATUS_OK;
	xn::NodeInfoList recorders_list;
	status = ni->getContext().EnumerateProductionTrees(XN_NODE_TYPE_RECORDER, NULL, recorders_list);
	START_CAPTURE_CHECK_RC(status, "Enumerate recorders");
	
	// take first
	xn::NodeInfo chosen = *recorders_list.Begin();

	recorder = new xn::Recorder;
	status = ni->getContext().CreateProductionTree(chosen, *recorder);
	START_CAPTURE_CHECK_RC(status, "Create recorder");

	status = recorder->SetDestination(XN_RECORD_MEDIUM_FILE, filepath.c_str());
	START_CAPTURE_CHECK_RC(status, "Set output file");
}

bool Recorder::start() {
	if (filepath.empty()) {
		printf("Call setup before starting.\n");
		return false;
	}

	XnUInt64 now;
	xnOSGetTimeStamp(&now);
	now /= 1000;
	start_on += (XnUInt32)now;
	printf("START ON %d\n", start_on);
	state = SHOULD_CAPTURE;
	return true;
}

bool Recorder::stop() {
	if(recorder == NULL) {
		return false;
	}
	state = NOT_CAPTURING;
	recorder->Release();
	delete recorder;
	recorder = NULL;
}

XnStatus Recorder::captureFrame() {
	XnStatus status = XN_STATUS_OK;
	if (state == CAPTURING){
		// There isn't a real need to call Record() here, as the WaitXUpdateAll() call already makes sure recording is performed.
		status = recorder->Record();
		if (status != ((XnStatus)0)){
			SHOW_RC(status, "Capture frame.");
			return (status);
		}

		// count recorded frames
		for (int i = 0; i < CAPTURE_NODE_COUNT; ++i){
			if (nodes[i].is_recording && nodes[i].generator->IsDataNew())
				nodes[i].num_captured_frames++;
		}
	}

	else if (state == SHOULD_CAPTURE){
		XnUInt64 now;
		xnOSGetTimeStamp(&now);
		now /= 1000;

		if (now >= start_on) { 
			
			if (skip_first_frame){
				skip_first_frame = false;
			}
			else{
				for (int i = 0; i < CAPTURE_NODE_COUNT; ++i){
					nodes[i].num_captured_frames = 0;
					nodes[i].is_recording = false;
				}
				state = CAPTURING;

				if (ni->getDepth().getGenerator().IsValid() && nodes[CAPTURE_DEPTH_NODE].capture_format!=XN_CODEC_NULL){
					status = recorder->AddNodeToRecording(ni->getDepth().getGenerator(), nodes[CAPTURE_DEPTH_NODE].capture_format);
					START_CAPTURE_CHECK_RC(status, "add depth node");
					nodes[CAPTURE_DEPTH_NODE].is_recording = TRUE;
					nodes[CAPTURE_DEPTH_NODE].generator = &ni->getDepth().getGenerator();
				}

				if (ni->getImage().getGenerator().IsValid() && nodes[CAPTURE_IMAGE_NODE].capture_format!=XN_CODEC_NULL){
					status = recorder->AddNodeToRecording(ni->getImage().getGenerator(), nodes[CAPTURE_IMAGE_NODE].capture_format);
					START_CAPTURE_CHECK_RC(status, "add image node");
					nodes[CAPTURE_IMAGE_NODE].is_recording = TRUE;
					nodes[CAPTURE_IMAGE_NODE].generator = &ni->getImage().getGenerator();
				}

			}
		}
	}

	return XN_STATUS_OK;
}
void Recorder::update() {
	captureFrame();
}

}} // roxlu::openni