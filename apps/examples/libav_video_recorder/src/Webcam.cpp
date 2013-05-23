#include "Webcam.h"

void webcam_on_audio_in(const void* input, unsigned long nframes, void* user) {
  Webcam* w = static_cast<Webcam*>(user);
  if(w->enc.isStarted()) {
    w->enc.addAudioFrame((uint8_t*)input, nframes);
  }
}

Webcam::Webcam() 
  :must_capture(false)
  ,is_recording(false)
{
}
 
Webcam::~Webcam() {
}
 
 
bool Webcam::setup() {

  // CAPTURE
  cap.listDevices();
  bool r = cap.openDevice(0, 640, 480, VC_FMT_YUYV422); // mac, "yuvs"
  if(!r) {
    RX_ERROR("Cannot open the capture device");
    return false;
  }
 
  surf.setup(cap.getWidth(), cap.getHeight());
 
   // VIDEO SETTTINGS
  AVEncoderSettings cfg;
  cfg.in_w = cfg.out_w = cap.getWidth();
  cfg.in_h = cfg.out_h = cap.getHeight();
  cfg.in_pixel_format = AV_PIX_FMT_UYVY422; 
  cfg.time_base_den = 25;
  cfg.time_base_num = 1;
 
  // AUDIO SETTINGS
  cfg.sample_fmt = AV_SAMPLE_FMT_S16P;
  cfg.audio_bit_rate = 64000;
  cfg.sample_rate = 44100;
  cfg.num_channels = 1;
 
  enc.setup(cfg);
 
  // AUDIO
  audio.listDevices();
  audio.setInputListener(webcam_on_audio_in, this);
  audio.openInputStream(4, cfg.num_channels, paInt16, 
                        44100, 
                        enc.enc.getAudioInputFrameSizePerChannel(cfg) );
  audio.startInputStream();
 
  return true;
}
 
bool Webcam::startCapture() {
 
  bool r = cap.startCapture();
  if(!r) {
    RX_ERROR("Cannot start the capture");
    return false;
  }
 
  must_capture = true;
  return true;
}
 
bool Webcam::stopCapture() {
  must_capture = false;
  return true;
}
 
void Webcam::update() {
  if(must_capture) {
    cap.update();
 
    if(cap.hasNewData()) {

      if(is_recording) {
        enc.addVideoFrame(cap.getNewDataPtr(), cap.getNumBytes());
      }

      surf.setPixels(cap.getNewDataPtr(), cap.getNumBytes());
    }
  }
}
 
void Webcam::draw() {
  surf.draw(0, 0, cap.getWidth(), cap.getHeight());
}
 
bool Webcam::startRecording() {
  is_recording = true;
  std::string filename = "recording_" +rx_strftime("%s") +".mp4";
  if(!enc.start(filename, true)) {
    RX_ERROR("Cannot start the encoder");
    ::exit(EXIT_FAILURE);
  }

  return true;
}
 

bool Webcam::stopRecording() {
  is_recording = false;
  enc.stop();
  return true;
}
 
