#include <ogg/CaptureBuffer.h>

CaptureBuffer::CaptureBuffer()
  :record(true)
  ,got_audio(false)
  ,got_video(false)
  ,audio_fp(NULL)
  ,video_fp(NULL)
  ,naudio_frames(0)
  ,nvideo_frames(0)
  ,video_bytes_written(0)
  ,audio_bytes_written(0)
  ,audio_samples_written(0)
  ,audio_samplerate(16000)
  ,fps(60)
  ,millis_per_frame(0)
  ,image_w(640)
  ,image_h(480)
  ,num_channels(1) // assuming unsigned short 
  ,bytes_per_image(0)
  ,bytes_per_audio(0)
  ,tmp_video_buffer(NULL)
  ,tmp_audio_buffer(NULL)
{
  audio_fp = fopen("audio.raw", "w+");
  if(!audio_fp) {
    printf("ERROR: cannot open audio file.\n");
  }

  video_fp = fopen("video.raw", "w+");
  if(!video_fp) {
    printf("ERROR: cannot open video file.\n");
  }

  bytes_per_image = image_w * image_h * 3;
  bytes_per_audio = 320 * sizeof(short int) * num_channels;

  tmp_video_buffer = new unsigned char[bytes_per_image];
  tmp_audio_buffer = new unsigned char[bytes_per_audio];

  memset(tmp_video_buffer, 0, bytes_per_image);
  memset(tmp_audio_buffer, 0, bytes_per_audio);

  millis_per_frame = (1.0/fps) * 1000;

  if(!ogg_maker.setup()) {
    ::exit(0);
  }
}

CaptureBuffer::~CaptureBuffer() {
  if(tmp_video_buffer != NULL) {
    delete[] tmp_video_buffer;
    tmp_video_buffer = NULL;
  }

  if(tmp_audio_buffer != NULL) {
    delete[] tmp_audio_buffer;
    tmp_audio_buffer = NULL;
  }
  
  if(audio_fp) {
    fclose(audio_fp);
    audio_fp = NULL;
  }

  if(video_fp) {
    fclose(video_fp);
    video_fp = NULL;
  }
  
}

void CaptureBuffer::addAudioFrame(rx_uint64 timestamp, const void* data, size_t nbytes, size_t nframes) {
  if(!record) {
    return;
  }
  got_audio = true;
  if(!got_video) {
    return;
  }

  // keep track of audio packets
  CaptureFrame fr;
  fr.type = 'A';
  fr.timestamp = timestamp;
  fr.offset = audio_bytes_written;
  fr.num_frames = nframes;
  fr.nbytes = nbytes;
  frames.push_back(fr);

  // write the raw audio data
  fwrite((char*)data, nbytes, 1, audio_fp);

  audio_bytes_written += nbytes;
  audio_samples_written += nframes;
  naudio_frames++;
}

void CaptureBuffer::addVideoFrame(rx_uint64 timestamp, const void* data, size_t nbytes) {
  if(!record) {
    return;
  }
  got_video = true;

  // only start adding after we've received an audio frame.
  if(!naudio_frames) { 
    return;
  }

  // keep track of video packets 
  CaptureFrame fr;
  fr.type = 'V';
  fr.timestamp = timestamp;
  fr.offset = video_bytes_written;
  fr.num_frames = 1; // not really used.
  fr.nbytes = nbytes;
  frames.push_back(fr);

  // write the raw frame.
  fwrite((char*)data, nbytes, 1, video_fp);

  video_bytes_written += nbytes;
  nvideo_frames++;
}

void CaptureBuffer::encode() {
  record = false;
  std::sort(frames.begin(), frames.end(), CaptureFrameSorter());
  double total_audio_samples = 0;
  double audio_real_time = 0;
  rx_uint64 video_frame = 0;
  double video_fps = 1.0/fps;
  CaptureFrame* last_video_frame_ptr = NULL;
  int last_video_frame_num = 0;
  
  fseek(audio_fp, 0, SEEK_SET);
  fseek(video_fp, 0, SEEK_SET);
  double total_capture_time = (double(audio_samples_written)/audio_samplerate);
  int last = 0;
  
  for(std::vector<CaptureFrame>::iterator it = frames.begin(); it != frames.end(); ++it) {
    CaptureFrame& f = *it;

    // AUDIO FRAME
    if(f.type == 'A' && last_video_frame_ptr != NULL) {
      total_audio_samples += f.num_frames;
      audio_real_time = total_audio_samples / audio_samplerate;
      video_frame = audio_real_time / video_fps;

      fseek(audio_fp, f.offset, SEEK_SET);
      fread(tmp_audio_buffer, sizeof(char), f.nbytes, audio_fp);
      ogg_maker.addAudioFrame(tmp_audio_buffer, f.nbytes);

      if(audio_real_time >= total_capture_time) {
        last = -1;
      }
  
      printf("Frame: %lld / %zu\n", video_frame, nvideo_frames);

      // check if we need to catch up with the video frames
      int duplicate = -1;
      while(last_video_frame_num < video_frame) {
        
        // only write a raw frame for the first frame, the other frames we need to catch up are duplicates of this one
        if(duplicate < 0) {
          int r = fseek(video_fp, last_video_frame_ptr->offset, SEEK_SET);
          size_t bytes_read = fread(tmp_video_buffer, 1, last_video_frame_ptr->nbytes, video_fp);
          if(bytes_read != last_video_frame_ptr->nbytes) {
            printf("ERROR: cannot read data from video file: %d\n", ferror(video_fp));
            ::exit(0);
          }
          ogg_maker.addVideoFrame(tmp_video_buffer, last_video_frame_ptr->nbytes, last);
        }
        last_video_frame_num++;
        ++duplicate;
      }
      if(duplicate > 0) {
        ogg_maker.duplicateFrames(duplicate);
      }
    }
    // VIDEO FRAME
    else {
      last_video_frame_ptr = &f;
    }
  }
}
