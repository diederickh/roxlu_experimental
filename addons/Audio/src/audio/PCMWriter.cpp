#include <audio/PCMWriter.h>

// Just added this as an test.... might be removed
PCMWriter::PCMWriter() 
  :ring_buffer(0)
  ,fp(0)
  ,num_channels(2)
  ,bytes_per_sample(sizeof(short))
  ,samplerate(44100)
  ,expected_seconds_to_record(10)
{
}

PCMWriter::~PCMWriter() {
  close();
}

// We use the expected seconds to record to allocate a memory buffer; make sure this is enough.
bool PCMWriter::open(const std::string filepath, int expectedSecondsToRecord, int sampleRate, int nchannels, size_t bytesPerSample) {
  fp = fopen(filepath.c_str(), "wb");
  if(!fp) {
    printf("ERROR: cannot open file: %s..\n", filepath.c_str());
    return false;
  }
  expected_seconds_to_record = expectedSecondsToRecord;
  samplerate = sampleRate;
  num_channels = nchannels;
  bytes_per_sample = bytesPerSample;
  size_t bytes_needed = (samplerate * expected_seconds_to_record * num_channels * bytes_per_sample);
  ring_buffer.resize(bytes_needed);
  return true;
}

void PCMWriter::close() {
  if(fp) {
    size_t written = fwrite(ring_buffer.getReadPtr(), sizeof(char), ring_buffer.size(), fp);
    fclose(fp);
    fp = 0;
  }
}

// numFrames are the number of sound 'entries' in input. For example
// when you are getting 320 frames each time your audio input callback 
// is called, then numFrames = 320.  If you're sampling at 16000hz then
// you function will be called 16000/320 = 50 times per second.
void PCMWriter::onAudioIn(const void* input, unsigned long numFrames) {
  if(!fp) {
    return;
  }
  ring_buffer.write((char*)input, numFrames * bytes_per_sample * num_channels);
}
