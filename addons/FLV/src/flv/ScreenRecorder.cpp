#include <flv/ScreenRecorder.h>

ScreenRecorder::ScreenRecorder() 
  :width(0)
  ,height(0)
  ,channels(4)
  ,pixels(NULL)
  ,dx(0)
  ,is_recording(false)
{
  memset(pbos, 0, sizeof(pbos));
}

ScreenRecorder::~ScreenRecorder() {
}

bool ScreenRecorder::setup(std::string filepath, 
                           unsigned int w, 
                           unsigned int h)
{
  if(!flv_writer.open(filepath)) {
    return false;
  }

  // if(!av.setupVideo(w,h,60, AV_FMT_RGB24)) {
  if(!av.setupVideo(w,h,60, AV_FMT_BGRA32)) {
    return false;
  }

  flv.setCallbacks(&FLVFileWriter::write,
                   &FLVFileWriter::rewrite,
                   &FLVFileWriter::flush,
                   &FLVFileWriter::close,
                   &flv_writer);

  av.setFLV(&flv);

  this->width = w;
  this->height = h;

#if !defined(SCREEN_RECORDER_USE_PBO)
  pixels = new unsigned char[w * h * channels];
  memset(pixels, 0, w * h * channels);
#endif

  if(!av.initialize()) {
    printf("WARNING: ScreenRecorder::setup(), cannot initialize AV.\n");
    return false;
  }

#if defined(SCREEN_RECORDER_USE_PBO) 
  glGenBuffers(SCREEN_RECORDER_NUM_PBOS, pbos);
  for(int i = 0; i < SCREEN_RECORDER_NUM_PBOS; ++i) {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
    glBufferData(GL_PIXEL_PACK_BUFFER, w * h * channels, pixels, GL_STREAM_READ);
  }

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
#endif


  return true;
}

void ScreenRecorder::beginFrame() {
}

void ScreenRecorder::endFrame() {
  if(!is_recording) {
    return;
  }

#if !defined(SCREEN_RECORDER_USE_PBO)
  if(av.wantsNewVideoFrame()) {
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    av.addVideoFrame(pixels);
  }
#else
  if(av.wantsNewVideoFrame()) {
    int write_dx = (dx++) % SCREEN_RECORDER_NUM_PBOS;
    int read_dx = (write_dx + 1) % SCREEN_RECORDER_NUM_PBOS;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[write_dx]);
    glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0); 

    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[read_dx]);
    GLubyte* ptr = (GLubyte*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    if(ptr) {
      av.addVideoFrame(ptr);
      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  }

#endif
}

void ScreenRecorder::processPixels() {
}

void ScreenRecorder::start() {
  is_recording = true;
  av.start();
}

void ScreenRecorder::stop() {
  is_recording = false;
  av.stop();
}
