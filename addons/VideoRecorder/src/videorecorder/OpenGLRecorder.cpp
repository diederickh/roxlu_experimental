#include <videorecorder/OpenGLRecorder.h>
#include <videorecorder/VideoRecorder.h>
#include <stdio.h>

OpenGLRecorder::OpenGLRecorder() 
  :rec(NULL)
  ,io_flv(NULL)
  ,pixels(NULL)
  ,dest_pixels(NULL)
  ,w(0)
  ,h(0)
  ,out_w(0)
  ,out_h(0)
{
}

OpenGLRecorder::~OpenGLRecorder() {
  if(pixels != NULL) {
    delete[] pixels;
    pixels = NULL;
  }
  if(dest_pixels != NULL) {
    delete[] dest_pixels;
    dest_pixels = NULL;
  }
  if(rec != NULL) {
    rec->close();
  }
  if(io_flv != NULL) {
    delete io_flv;
    io_flv = NULL;
  }
}

void OpenGLRecorder::open(const char* filepath, int outW, int outH) {
  if(rec == NULL) {
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport); eglGetError();
    w = viewport[2];
    h = viewport[3];
    out_w = outW;
    out_h = outH;

    if(!outW || !outH) {
      out_w = w;
      out_h = h;
    }

    io_flv = new VideoIOFLV();
    rec = new VideoRecorder(w,h,out_w,out_h, 60, true, false);
    rec->setIO(io_flv);
    rec->open(filepath);
    pixels = new unsigned char[w * h * 3];
    dest_pixels = new unsigned char[w * h * 3];
  }
}

void OpenGLRecorder::grabFrame() {
  if(rec == NULL) {
    //printf("ERROR: first open() the gl recorder.\n");
    return;
  }
  //  glReadBuffer(GL_BACK); eglGetError();
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);  eglGetError();
  int stride = w * 3;
  for(int j = 0; j < h; ++j) {
    int src_dx = ((h - j) * w * 3);
    int dest_dx =  j * w * 3;
    memcpy(dest_pixels + dest_dx, pixels + src_dx, stride);
  }
  rec->addVideoFrame(dest_pixels);
}

void OpenGLRecorder::close() {
  rec->close();
  delete rec;
  rec = NULL;
}
