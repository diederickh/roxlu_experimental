#ifndef ROXLU_CAPTURE_BUFFER_H
#define ROXLU_CAPTURE_BUFFER_H

#include <roxlu/Roxlu.h>
#include <ogg/OggMaker.h>
#include <ogg/Types.h>
#include <vector>

/*

 Temporary buffer which stores all RAW frames (rgb only for now) into 
 a file. It does the same with the audio data. Once everything is captured
 it encodes everything and makes sure that the video frames are kept in 
 sync by using the audio frames as base-time.

 Usage:
 ------

 // setup:
 CaptureBuffer cap;
 cap.setup(...)


 // when you receive a new audio frame, add it directly:
 cap.addAudioFrame(Timer::millis(), ...)
 

 // to add video frames you check 'wantsNewVideoFrame()'
 if(cap.wantsNewVideoFrame()) {
    cap.addVideoFrame(..)
 }
 */

struct CaptureFrame {
  char type; // V = video, A = audio
  rx_uint64 timestamp;
  size_t offset; // offset in file.
  size_t num_frames; // for audio
  size_t nbytes;
};

struct CaptureFrameSorter {
  bool operator()(const CaptureFrame& a, const CaptureFrame& b) {
    return a.timestamp < b.timestamp;
  }
};

class CaptureBuffer {
 public:
  CaptureBuffer();
  ~CaptureBuffer();
  bool setup(OggVideoFormat vfmt,  // video format
    int w,  // width of data for images passed to addVideoFrame
    int h,  // height of data for images passed to addVideoFrame
    OggAudioFormat afmt, // audio format
    int numChannels, // num channels in audio input
    int samplerate,  // audio samplerate: 16000, 44100
    size_t bytesPerSample, // bytes per frame/sample: sizeof(short), sizeof(float)
    size_t maxFramesPerCall  // when calling addAudioFrame, nframes will be less or then this
  );

  void addAudioFrame(rx_uint64 timestamp, const void* data, size_t nframes);
  bool wantsNewVideoFrame(); // returns true when we need a new video frame (based on the set framerate)
  void addVideoFrame(rx_uint64 timestamp, const void* data); 
  void encode();
 public:
  bool is_setup;
  bool record;
  bool got_audio;
  bool got_video;
  FILE* audio_fp;
  FILE* video_fp;
  size_t naudio_frames;
  size_t nvideo_frames;
  std::vector<CaptureFrame> frames;
  size_t video_bytes_written;
  size_t audio_bytes_written;
  size_t audio_samples_written;
  int audio_samplerate; 
  int fps;
  int millis_per_frame;
  int image_w;
  int image_h;
  int num_channels;// number of audio channels
  size_t bytes_per_image; // exact number of  bytes that is passed to addVideoFrame
  size_t bytes_per_audio;  // max bytes that is passed to addAudioFrame 
  size_t bytes_per_sample; // bytes for one frame (one channel)
  unsigned char* tmp_video_buffer;
  unsigned char* tmp_audio_buffer;
  OggMaker ogg_maker;
  rx_uint64 fps_timeout; // when this timeout occurs we need a new video frame
};
#endif
