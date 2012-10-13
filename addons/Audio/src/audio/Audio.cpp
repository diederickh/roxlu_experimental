#include <audio/Audio.h>

Audio::Audio() 
	:input_stream(NULL)
	,input_listener(NULL)
{
	PaError err = Pa_Initialize();
	if(err != paNoError) {
		printf("ERROR: cannot initialize port audio.\n");
		printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
	}
}

Audio::~Audio() {
	PaError err = Pa_Terminate();
	if(err != paNoError) {
		printf("ERROR: cannot terminate port audio.\n");
		printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
	}

}

int Audio::listDevices() {
	int num = Pa_GetDeviceCount();
	if(num < 0) {
		printf("ERROR: Pa_CountDevices returned: 0x%x\n", num);
		return -1;
	};

	const PaDeviceInfo* dev_info;
	for(int i = 0; i < num; ++i) {
		dev_info = Pa_GetDeviceInfo(i);
		printf("[%d] = %s, max in channels: %d, max out channels: %d, default samplerate: %f\n"
				 ,i
				 ,dev_info->name
				 ,dev_info->maxInputChannels
				 ,dev_info->maxOutputChannels
				 ,dev_info->defaultSampleRate);
	}

	return -1;
}

bool Audio::isInputFormatSupported(int device, int numChannels, PaSampleFormat format, double samplerate) {
	PaStreamParameters input;
	PaError err;

	input.channelCount = numChannels;
	input.device = device;
	input.hostApiSpecificStreamInfo = NULL;
	input.sampleFormat = format;
	input.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;
  
	err = Pa_IsFormatSupported(&input, NULL, samplerate);
	if(err == paFormatIsSupported) {
		printf("+ [%d] supports channels: %d, format: %s, samplerate: %d\n", device, numChannels, getSampleFormatText(format).c_str(), int(samplerate));
		return true;
	}
	else {
		printf("- [%d] supports channels: %d, format: %s, samplerate: %d\n", device, numChannels, getSampleFormatText(format).c_str(), int(samplerate));
		return false;
	}
}

std::string Audio::getSampleFormatText(PaSampleFormat f) {
	std::string r = "";
	switch(f) {
	case paFloat32: r = "F32"; break;
	case paInt32: r = "I32"; break;
	case paInt24: r = "I24"; break;
	case paInt16: r = "I16"; break;
	case paInt8: r = "I8"; break;
	case paUInt8: r = "UI8"; break;
	case paCustomFormat: "CustomFormat"; break;
	default: r = "unknown"; break;
	};
	return r;
}



// --------------------------------------------------
bool Audio::openInputStream(
									 int device
									 ,int numChannels
									 ,PaSampleFormat format
									 ,double samplerate
									 ,unsigned long framesPerBuffer
)
{
	PaStreamParameters input;

	bzero(&input, sizeof(input));

	input.channelCount = numChannels;
	input.device = device;
	input.hostApiSpecificStreamInfo = NULL;
	input.sampleFormat = format;
	input.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

	PaError err = Pa_OpenStream(
										 &input_stream
										 ,&input
										 ,NULL
										 ,samplerate
										 ,framesPerBuffer
										 ,paNoFlag
										 ,&Audio::inputStreamCallback
										 ,(void*) this
										 );
	if(err != paNoError) {
		printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
		return false;
	}
									 
	return true;
}

int Audio::inputStreamCallback(
								  const void* input
								  ,void* output
								  ,unsigned long numFrames
								  ,const PaStreamCallbackTimeInfo* time
								  ,PaStreamCallbackFlags status
								  ,void* userData)
{
	Audio* a = static_cast<Audio*>(userData);
	if(a->input_listener) {
		a->input_listener->onAudioIn(input, numFrames);
	}
}


bool Audio::startInputStream() {
	if(input_stream == NULL) {
		printf("ERROR: cannot start input stream which hasnt been opened yet.\n");
		return false;
	}
	PaError err = Pa_StartStream(input_stream);
	if(err != paNoError) {
		printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
		return false;
	}
	return true;

}

bool Audio::stopInputStream() {
	if(input_stream == NULL) {
		printf("ERROR: cannot stop a stream which isnt' created yet.\n");
		return false;
	}
	PaError err = Pa_StopStream(input_stream);
	if(err != paNoError) {
		printf("ERROR: portaudio message: %s\n", Pa_GetErrorText(err));
		return false;
	}
	return true;
}

void Audio::setInputListener(AudioListener* listener) {
	input_listener = listener;
}
