#include <Simulation.h>

void audio_in_cb(const void* input, unsigned long nframes, void* user) {
  Simulation* s = static_cast<Simulation*>(user);
  s->av.addAudioFrame(input, nframes);
}

size_t flv_write(char* data, size_t nbytes, void* user) {
  Simulation* s = static_cast<Simulation*>(user);
  if(!s->flv_fp) {
    printf("WARNING: cannot write to flv, file not opened.\n");
    return 0;
  }
  int r = fwrite(data, nbytes, 1, s->flv_fp);
  if(r != 1) {
    printf("WARNING: cannot write data chunk to file.\n");
    return 0;
  }
  return nbytes;
}

void flv_rewrite(char* data, size_t nbytes, size_t pos, void* user) {

  Simulation* s = static_cast<Simulation*>(user);
  printf("TRYING TO REWRITE: sim: %p, file: %p\n", s, s->flv_fp);
  if(!s->flv_fp) {
    printf("WARNING: cannot re-write to flv, file not opened.\n");
    return;
  }
  printf("REWRITING: sim: %p, file: %p\n", s, s->flv_fp);
  long int curr = ftell(s->flv_fp);
  fseek(s->flv_fp, pos, SEEK_SET);
  fwrite(data,nbytes, 1, s->flv_fp);
  fseek(s->flv_fp, curr, SEEK_SET);
}

void flv_flush(void* user) {
  Simulation* s = static_cast<Simulation*>(user);
  if(!s->flv_fp) {
    printf("WARNING: cannot flush, file not opened.\n");
    return;
  }
  fflush(s->flv_fp);
}

void flv_close(void* user) {
  Simulation* s = static_cast<Simulation*>(user);
  if(!s->flv_fp) {
    printf("WARNING: cannot flush, file not opened.\n");
    return;
  }
  printf("flv_close <--> CLOSING: %p, %p\n", s, s->flv_fp);
  fclose(s->flv_fp);
  s->flv_fp = NULL;

  printf("FILE CLOSED!\n");
}

Simulation::Simulation()
  :SimulationBase()
  ,vao(0)
  ,prog(0)
  ,vbo(0)
  ,tex(0)
  ,flv_fp(NULL)
{
}

Simulation::~Simulation() {
  //  av.reset();
  av.stop();
  av.waitForEncodingThreadToFinish();
  
  //  av_thread.join();
}

void Simulation::setup() {

  setWindowTitle("FLV recorder");

  setupShaderAndBuffers();

  setupCapture();
  
  setupFLV();

  setupAudio();

}

void Simulation::update() {
  cap.update();

  if(cap.hasNewData()) {
    cap.resetHasNewData();

    // Converting 640x480 YUV data takes about 3 millis
    unsigned char* src_slice[] = { cap.getPtr() } ;
    int src_stride[] = { cap.getWidth() * 2 }; // 2 bytes per pixel 16bit YUV
    int out_stride[] = { cap.getWidth() * 3 } ;// 3 bytes per pixel 24bit RGB
    unsigned char* dest[] = { capture_pixels } ;
    int h = sws_scale(sws, src_slice, src_stride, 0, cap.getHeight(), dest, out_stride);  

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cap.getWidth(), cap.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, capture_pixels);
  }  
}


void Simulation::draw() {
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(prog);
  glBindVertexArray(vao);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  glUniform1i(glGetUniformLocation(prog, "u_tex"), 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  fps.draw();

  if(av.wantsNewVideoFrame()) {
    glReadPixels(0, 0, VIDEO_W, VIDEO_H, GL_RGB, GL_UNSIGNED_BYTE, screen_pixels);
    av.addVideoFrame(screen_pixels);
  }

}

void Simulation::onMouseDown(int x, int y, int button) {
}

void Simulation::onMouseUp(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onKeyDown(int key) {
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

void Simulation::setupShaderAndBuffers() {
  // CREATE SHADER
  prog = rx_create_shader(VID_VS, VID_FS);
  glBindAttribLocation(prog, 0, "a_pos"); 
  glBindAttribLocation(prog, 1, "a_tex");
  glLinkProgram(prog);
  glUseProgram(prog);

  // SETUP BUFFERS
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLfloat vertices[] = {
   -1.0f,  -1.0f, 0.0f, 0.0f,
    1.0f,  -1.0f, 1.0f, 0.0f,
    1.0f,   1.0f, 1.0f, 1.0f,

   -1.0f,  -1.0f, 0.0f, 0.0f,
    1.0f,   1.0f, 1.0f, 1.0f,
   -1.0f,   1.0f, 0.0f, 1.0f
  };

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // tex
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);

  glUseProgram(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_W, VIDEO_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
}


void Simulation::setupFLV() {
  std::string flv_output = rx_to_exe_path("simulation.flv");
  flv_fp = fopen(flv_output.c_str(), "wb");
  if(!flv_fp) {
    printf("ERROR: cannot create simulation.flv file.\n");
    ::exit(EXIT_FAILURE);
  }

  flv.setCallbacks(flv_write, flv_rewrite, flv_flush, flv_close, this);

  av.setFLV(&flv);
  if(!av.setupVideo(VIDEO_W, VIDEO_H, VIDEO_FPS, AV_FMT_RGB24)) {
    printf("ERROR: cannot setup AV.video.\n");
    ::exit(EXIT_FAILURE);
  }

  if(!av.setupAudio(AUDIO_NUM_CHANNELS, AUDIO_SAMPLERATE, AUDIO_MAX_SAMPLES, AUDIO_AV_FORMAT)) {
    printf("ERROR: cannot setup AV.audio.\n");
    ::exit(EXIT_FAILURE);
  }
  
  if(!av.initialize()) {
    printf("ERROR: cannot initialize AV.\n");
    ::exit(EXIT_FAILURE);
  }

  av.start();
}

void Simulation::setupAudio() {
  audio.setInputListener(audio_in_cb, this);
  audio.listDevices();
  audio.openInputStream(1, AUDIO_NUM_CHANNELS, AUDIO_FORMAT, AUDIO_SAMPLERATE, AUDIO_MAX_SAMPLES);
  audio.startInputStream();
}

void Simulation::setupCapture() {
  cap.listDevices();
  //  bool r = cap.openDevice(0, VIDEO_W, VIDEO_H, VC_FMT_UYVY422 ); //  VC_FMT_YUYV422); // format might differ on windows (VC_FMT_UYVY422)
  bool r = cap.openDevice(0, VIDEO_W, VIDEO_H, VC_FMT_YUYV422); //  VC_FMT_YUYV422); // format might differ on windows (VC_FMT_UYVY422)
  if(!r) {
    printf("ERROR: cannot open video capture device. Check if the device number is correct.\n");
    ::exit(EXIT_FAILURE);
  }

  r = cap.startCapture();
  if(!r) {
    printf("ERROR: cannot start video capture.\n");
    ::exit(EXIT_FAILURE);
  }

  sws = sws_getContext(cap.getWidth(), cap.getHeight(), PIX_FMT_YUYV422,
                       cap.getWidth(), cap.getHeight(), PIX_FMT_RGB24,
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);
  if(!sws) {
    printf("ERROR: cannot create SWS conversion context.\n");
    ::exit(EXIT_FAILURE);
  }
}
