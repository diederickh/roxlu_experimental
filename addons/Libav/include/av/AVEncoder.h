/*

  AVEncoder
  ---------

  Generic video encoder based on libav: http://git.libav.org/?p=libav.git;a=blob;f=libavformat/output-example.c;hb=HEAD

 */
#ifndef ROXLU_LIBAV_AVENCODER_H
#define ROXLU_LIBAV_AVENCODER_H

#include <av/AVTypes.h>
#include <av/AVUtils.h>

#define ERR_AV_ALREADY_STARTED "Cannot start the AVEncoder twice"
#define ERR_AV_OUTPUT_FMT "Cannot guess the output format"
#define ERR_AV_FORMAT_CONTEXT "Cannot allocate the format_context member"
#define ERR_AV_NO_VIDEO_CODEC "Cannot find a video codec that is related to the given filename"
#define ERR_AV_VIDEO_ENC_NOT_FOUND "Cannot find the video encoder"
#define ERR_AV_VIDEO_STREAM "Cannot create a new video stream"
#define ERR_AV_OPEN_VIDEO_CODEC "Cannot open the video encoder"
#define ERR_AV_ALLOC_VIDEO_FRAME "Cannot allocate a video frame"
#define ERR_AV_ALLOC_AVFRAME "avcodec_alloc_frame fails"
#define ERR_AV_ALLOC_BUF_AVFRAME "Cannot av_malloc the buffer for the AVFrame"
#define ERR_AV_OPEN_FILE "Cannot open the output file (permissions correct?)"
#define ERR_AV_SWS "Cannot create a SWS context"
#define ERR_AV_SWS_SCALE "Cannot sws_scale(), returned incorrect result"
#define ERR_AV_ADD_VFRAME_SCALE "Adding a video frame cancelled because SWS_SCALE failed"
#define ERR_AV_WRITE_VIDEO "An error occured when writing a video frame"
#define ERR_AV_WRONG_MILLIS_FPS "Wrong timebase; we cannot calculate the millis per frame"
#define ERR_AV_LIST_PIX_FMT_CODEC_NOT_OPEN "Cannot list the video codec pixel formats because it's not yet opened"

class AVEncoder {
 public:
  AVEncoder();
  ~AVEncoder();
  bool setup(AVEncoderSettings settings);                              /* call once with the encoder settings */
  bool start(std::string filename, bool datapath = false);             /* call this when you want to start an encoding session */
  bool stop();                                                         /* call this when you want to stop the current encoding session */
  bool addVideoFrame(unsigned char* data, int64_t pts, size_t nbytes); /* add a raw frame. the format must be AVEncoderSettings.in_pixel_format. Pass the pts for the current frame, make sure it's in the timebase pts x */
  bool addVideoFrame(unsigned char* data, size_t nbytes);              /* add a video frame and let us determine the PTS */
 private:
  bool initializeSWS();
  bool rescale(unsigned char* data);
  bool addVideoStream(enum AVCodecID codecID);                         /* based on the current AVOutputFormat we will add a video stream for the given codec */
  AVFrame* allocVideoFrame(enum AVPixelFormat pixelFormat,             /* allocates a AVFrame for the given params */
                           int width, int height);
  bool openVideo();                                                    /* opens the video_codec_context, must be called after addVideoStream() */
  bool openAudio();      
  void closeVideo();
  
  /* debug info */
  void listSupportedVideoCodecPixelFormats();                          /* shows the names of the pixel formats that are supported by the video codec */

 public:
  AVEncoderSettings settings;

  /* muxer */
  AVOutputFormat* output_format;
  AVFormatContext* format_context;
  int64_t time_started;
  int64_t millis_per_frame;
  int64_t new_frame_timeout;          


  /* audio */
  AVCodecContext* audio_codec_context;
  AVCodec* audio_codec;
  AVStream* audio_stream;
  AVFrame* audio_frame;

  /* video */
  AVCodec* video_codec;
  AVCodecContext* video_codec_context;
  AVStream* video_stream;
  AVPicture video_frame_in;                                    /* the input video frame that will hold the pixels that need to be encoded */
  AVFrame* video_frame_out;                                    /* when the output format is not the same as the input format we need to convert the input->output */
  SwsContext* sws;                                             /* we use a SwsContext to convert the input when it's not the same size or pixel format as the output */
};


#endif
