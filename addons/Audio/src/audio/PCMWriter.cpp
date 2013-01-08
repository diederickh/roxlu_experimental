#include <audio/PCMWriter.h>

// Just added this as an test.... might be removed

PCMWriter::PCMWriter() 
  :ring_buffer(1024 * 1024 * 10)
  ,fp(0)
{
}


PCMWriter::~PCMWriter() {
  close();
}

bool PCMWriter::open(const std::string filepath) {
  fp = fopen(filepath.c_str(), "w");
  if(!fp) {
    printf("ERROR: cannot open file..\n");
    return false;
  }
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
  int num_channels = 1;
  ring_buffer.write((char*)input, numFrames * sizeof(short int) * num_channels);
}
