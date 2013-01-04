#include <videorecorder/OpenGLRecorder.h>
#include <videorecorder/VideoRecorder.h>
#include <stdio.h>

// -------------------------------------------------------------------
// OPENGL RECORDER THREAD
// -------------------------------------------------------------------
OpenGLRecorderWorker::OpenGLRecorderWorker() 
  :rec(NULL)
  ,num_bytes_in_image(0)
  ,must_flush(false)
  ,video_buffer(1024 * 1024 * 250) // 250 meg, which is more then enough for 1024x768 @ 60 fps
  ,audio_buffer(1024 * 1025 * 25) 
  ,tmp_pixel_buffer(NULL)
{
  clear();
}

OpenGLRecorderWorker::~OpenGLRecorderWorker() {
  flush();
  if(tmp_pixel_buffer != NULL) {
    delete[] tmp_pixel_buffer;
  }
}

void OpenGLRecorderWorker::copyImage(unsigned char* pixels) {
  mutex.lock();
  size_t written = video_buffer.write((const char*)pixels, num_bytes_in_image);
  mutex.unlock();
}

void OpenGLRecorderWorker::copyAudio(void* data, size_t nbytes) {
  mutex.lock();
  size_t written = audio_buffer.write((const char*)data, nbytes);
  mutex.unlock();
}

void OpenGLRecorderWorker::clear() {
  mutex.lock();
  for(std::vector<unsigned char*>::iterator it = images.begin(); it != images.end(); ++it) {
    delete *it;
  }
  images.clear();
  mutex.unlock();
}

void OpenGLRecorderWorker::flush() {

  
  //  printf("OpenGLRecorderWorker::flush(), bytes in audio buffer: %zu\n", audio_buffer.size());

  // unsigned char* tmp_buffer = new unsigned char[num_bytes_in_image];
  if(tmp_pixel_buffer == NULL) {
    tmp_pixel_buffer = new unsigned char[num_bytes_in_image];
  }
  must_flush = true;
  mutex.lock();
  size_t bytes_read = 0;
  while((bytes_read = video_buffer.read((char*)tmp_pixel_buffer, num_bytes_in_image)) > 0) {
    rec->addVideoFrame(tmp_pixel_buffer);
  }

  // bytes_read = 0;
  while((bytes_read = audio_buffer.read((char*)tmp_pixel_buffer, 320 * sizeof(short int))) > 0) {
    rec->addAudioFrame((short int *)tmp_pixel_buffer, 320 * sizeof(short int));
  }
  mutex.unlock();
  // delete[] tmp_buffer;
  clear();
}

// @todo we actually don't need to copy from the ringbuffer, we could just pass a pointer to it
void OpenGLRecorderWorker::run() {
  //  return;
  unsigned char* curr_data;
  size_t bytes_read = 0;
  //unsigned char* tmp_buffer = new unsigned char[num_bytes_in_image];
  if(tmp_pixel_buffer == NULL) {
    tmp_pixel_buffer = new unsigned char[num_bytes_in_image];
  }
  while(!must_flush) {
    //    mutex.lock();
    size_t bytes_read = 0;
    while((bytes_read = video_buffer.read((char*)tmp_pixel_buffer, num_bytes_in_image)) > 0) {
      rec->addVideoFrame(tmp_pixel_buffer);
    }

    // bytes_read = 0;
    int i = 0;
    while((bytes_read = audio_buffer.read((char*)tmp_pixel_buffer, 320 * sizeof(short int))) > 0) {
      rec->addAudioFrame((short int *)tmp_pixel_buffer, 320 * sizeof(short int));
      printf("(%d) In audio buffer: %zu, framebytes: %zu\n", i++, audio_buffer.size(), 320 * sizeof(short int));
    }
    //    mutex.unlock();

    /*
    // copy pixel data from buffer
    {
      mutex.lock();

      curr_data = NULL;
      bytes_read = video_buffer.read((char*)tmp_pixel_buffer, num_bytes_in_image);
      if(bytes_read > 0) {
        curr_data = tmp_pixel_buffer;
      }
      mutex.unlock();


      // write pixel data
      if(curr_data != NULL) {
        rec->addVideoFrame(curr_data);
        curr_data = NULL;
      }
    }

    // copy audio data from buffer
    {
      mutex.lock();
      bytes_read = audio_buffer.read((char*)tmp_pixel_buffer, 320 * sizeof(short int));
      if(bytes_read > 0) {
        curr_data = tmp_pixel_buffer;
      }
      mutex.unlock();

      // write audio data
      if(curr_data != NULL) {
        rec->addAudioFrame((short int*)curr_data, 320 * sizeof(short int));
        curr_data = NULL;
      }
    }
    */
  }
  if(curr_data != NULL) {
    curr_data = NULL;
  }
  //  delete[] tmp_buffer;
}

// -------------------------------------------------------------------
// OPENGL RECORDER
// -------------------------------------------------------------------
OpenGLRecorder::OpenGLRecorder(float fps) 
  :rec(NULL)
  ,io_flv(NULL)
  ,pixels(NULL)
  ,dest_pixels(NULL)
  ,w(0)
  ,h(0)
  ,out_w(0)
  ,out_h(0)
  ,grab_timeout(0)
  ,time_per_frame(0)
{
  setFPS(fps);
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

void OpenGLRecorder::setFPS(float fs) {
  fps = fs;
  time_per_frame = (1.0f/fps) * 1000;

}

void OpenGLRecorder::open(const std::string& filepath, int outW, int outH) {
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
    rec = new VideoRecorder(w,h,out_w,out_h, 60, true, true); // @todo, use FPS, test audio
    rec->setIO(io_flv);
    rec->setFPS(fps);
    rec->open(filepath.c_str());
    pixels = new unsigned char[w * h * 3];
    dest_pixels = new unsigned char[w * h * 3];
    worker.rec = rec;
    worker.num_bytes_in_image = w * h * 3;
    worker_thread.create(worker);
  }
}

void OpenGLRecorder::grabFrame() {
  if(rec == NULL) {
    return;
  }

  rx_uint64 now = Timer::now();
  if(now < grab_timeout) {
    return;
  }
  grab_timeout = now + time_per_frame;

  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)pixels);  eglGetError();
  int stride = w * 3;
  for(int j = 0; j < h; ++j) {
    int src_dx = ((h - j) * w * 3);
    int dest_dx =  j * w * 3;
    memcpy(dest_pixels + dest_dx, pixels + src_dx, stride);
  }
  worker.copyImage(dest_pixels);
}

void OpenGLRecorder::addAudioFrame(void* data, size_t nbytes) {
  worker.copyAudio(data, nbytes);
}

void OpenGLRecorder::close() {
  worker.flush();
  rec->close();
  delete rec;
  rec = NULL;
}
