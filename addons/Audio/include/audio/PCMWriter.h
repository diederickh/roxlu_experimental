#ifndef ROXLU_PCMWRITERH
#define ROXLU_PCMWRITERH

#include <audio/AudioListener.h>
#include <roxlu/Roxlu.h>
#include "RingBuffer.h"

class PCMWriter : public AudioListener {
public:
	PCMWriter();
	~PCMWriter();
	void onAudioIn(const void* input, unsigned long numFrames);
private:
	RingBuffer ring_buffer;
	FILE* fp;
	
};

#endif
