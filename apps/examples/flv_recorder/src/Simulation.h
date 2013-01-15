#ifndef ROXLU_SIMULATION_H
#define ROXLU_SIMULATION_H

#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videocapture/VideoCapture.h>
#include <audio/Audio.h>
#include <flv/AV.h>

extern "C" {
  #include <libswscale/swscale.h>
};

#define AUDIO_NUM_CHANNELS 1
#define AUDIO_SAMPLERATE 44100
#define AUDIO_MAX_SAMPLES 512 // max number of frames we receive in our audio callback
#define AUDIO_AV_FORMAT AV_FMT_INT16 // flv/av format
#define AUDIO_FORMAT paInt16  // PortAudio format
#define VIDEO_W 640
#define VIDEO_H 480
#define VIDEO_FPS 60

const static char* VID_VS = GLSL(120,
  attribute vec4 a_pos;
  attribute vec2 a_tex;
  varying vec2 v_tex;
  void main() {
    gl_Position = a_pos;
    v_tex = a_tex;
  }
);

const static char* VID_FS = GLSL(120, 
  uniform sampler2D u_tex;
  varying vec2 v_tex;
  void main() {
    vec4 col = texture2D(u_tex, v_tex);
    gl_FragColor.rgb = col.rgb;
    // gl_FragColor.r = v_tex.s;
    gl_FragColor.a = 1.0;
  }
);

void audio_in_cb(const void* input, unsigned long nframes, void* user);

size_t flv_write(char* data, size_t nbytes, void* user);
void flv_rewrite(char* data, size_t nbytes, size_t pos, void* user);
void flv_flush(void* user);
void flv_close(void* user);

class Simulation : public SimulationBase {
public:
  Simulation();
  ~Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y, int dx, int dy, int button);
  void onMouseMove(int x, int y);
  void onKeyDown(int key);
  void onKeyUp(int key);
  void onWindowClose();
 private:
  void setupCapture();
  void setupAudio();
  void setupFLV();
  void setupShaderAndBuffers(); // setup shader + buffers, used to draw the video grabber images to screen
 public:
  FPS fps;

  /* Rendering */
  GLuint prog; 
  GLuint vao;
  GLuint vbo;
  GLuint tex;

  /* Audio */
  Audio audio;

  /* VideoCapture */
  VideoCapture cap;
  SwsContext* sws;
  unsigned char capture_pixels[VIDEO_W * VIDEO_H * 3]; // might a bit to large

  /* FLV */
  FLV flv;
  FILE* flv_fp;
  AV av;
  unsigned char screen_pixels[VIDEO_W * VIDEO_H * 3];
};
#endif
