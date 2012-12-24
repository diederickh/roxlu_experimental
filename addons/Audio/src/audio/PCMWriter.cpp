#include <audio/PCMWriter.h>

// Just added this as an test.... might be removed

PCMWriter::PCMWriter() 
  :ring_buffer(1024 * 1000)
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

void PCMWriter::onAudioIn(const void* input, unsigned long numFrames) {
  if(!fp) {
    return;
  }
  ring_buffer.write((char*)input, numFrames * sizeof(short int));
}
