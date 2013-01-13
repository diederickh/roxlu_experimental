#include <ogg/CaptureBuffer.h>

CaptureBuffer::CaptureBuffer()
  :is_setup(false)
  ,record(true)
  ,got_audio(false)
  ,got_video(false)
  ,audio_fp(NULL)
  ,video_fp(NULL)
  ,naudio_frames(0)
  ,nvideo_frames(0)
  ,video_bytes_written(0)
  ,audio_bytes_written(0)
  ,audio_samples_written(0)
  ,audio_samplerate(0)
  ,fps(60)
  ,millis_per_frame(0)
  ,image_w(0)
  ,image_h(0)
  ,num_channels(0) 
  ,bytes_per_image(0)
  ,bytes_per_audio(0)
  ,bytes_per_sample(0)
  ,tmp_video_buffer(NULL)
  ,tmp_audio_buffer(NULL)
  ,fps_timeout(0)
{
  audio_fp = fopen("audio.raw", "wb+");
  if(!audio_fp) {
    printf("ERROR: cannot open audio file.\n");
  }

  video_fp = fopen("video.raw", "wb+");
  if(!video_fp) {
    printf("ERROR: cannot open video file.\n");
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

bool CaptureBuffer::setup(OggVideoFormat vfmt, 
                          int w, 
                          int h,
                          OggAudioFormat afmt, 
                          int numChannels, 
                          int samplerate, 
                          size_t bytesPerSample,
                          size_t maxFramesPerCall
)
{
  image_w = w;
  image_h = h;
  audio_samplerate = samplerate;
  num_channels = numChannels;
  bytes_per_sample = bytesPerSample;
  bytes_per_image = image_w * image_h * 3;
  bytes_per_audio = maxFramesPerCall * bytesPerSample * num_channels;

  tmp_video_buffer = new unsigned char[bytes_per_image];
  tmp_audio_buffer = new unsigned char[bytes_per_audio];

  memset(tmp_video_buffer, 0, bytes_per_image);
  memset(tmp_audio_buffer, 0, bytes_per_audio);

  millis_per_frame = (1.0/fps) * 1000;
  

  if(!ogg_maker.setup(vfmt, afmt, num_channels, samplerate, bytesPerSample)) {
    return false;
  }

  is_setup = true;
  return true;
}

void CaptureBuffer::addAudioFrame(rx_uint64 timestamp, 
                                  const void* data, 
                                  size_t nframes
)
{
  if(!is_setup) {
    return;
  }

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
  fr.nbytes = nframes * bytes_per_sample * num_channels;
  frames.push_back(fr);

  // write the raw audio data
  fwrite((char*)data, fr.nbytes, 1, audio_fp);

  audio_bytes_written += fr.nbytes;
  audio_samples_written += nframes;
  naudio_frames++;
}

void CaptureBuffer::addVideoFrame(rx_uint64 timestamp, const void* data) {
  if(!is_setup) {
    return;
  }

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
  fr.nbytes = bytes_per_image;
  frames.push_back(fr);

  // write the raw frame.
  fwrite((char*)data, bytes_per_image, 1, video_fp);

  video_bytes_written += bytes_per_image;
  nvideo_frames++;
}

bool CaptureBuffer::wantsNewVideoFrame() {
  if(!fps_timeout) {
    fps_timeout = rx_millis() + millis_per_frame;
    return true;
  }
  rx_uint64 now = rx_millis();
  if(now >= fps_timeout) {
    fps_timeout = now + millis_per_frame;
    return true;
  }
  return false;
}

void CaptureBuffer::encode() {
  if(!is_setup) {
    printf("WARNING: cannot encode because we havent been setup.\n");
    return;
  }
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
      ogg_maker.addAudioFrame(tmp_audio_buffer, f.num_frames);

      printf("Frame: %d / ~%d\n", int(video_frame), int(nvideo_frames));

      if(audio_real_time >= total_capture_time) {
        last = -1;
      }
  
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

