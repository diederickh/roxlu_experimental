#ifndef ROXLU_LIBAV_AVDECODER_H
#define ROXLU_LIBAV_AVDECODER_H


extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
#  include <libavcodec/avcodec.h>
#  include <libavutil/channel_layout.h>
#  include <libavutil/common.h>
#  include <libavutil/imgutils.h>
#  include <libavutil/mathematics.h>
#  include <libavutil/samplefmt.h>
#  include <uv.h>
}

#include <assert.h>
#include <string>
#include <vector>
#include <deque>
#include <av/AVTypes.h>
#include <av/AVUtils.h>

#define ERR_AVD_ALREADY_OPENED "Cannot open the AVDecoder twice. First close it before opening again"
#define ERR_AVD_ALLOC_FORMAT_CTX "Cannot allocate the avformat context"
#define ERR_AVD_OPEN_INPUT "Cannot open the given file: '%s', error: %s"
#define ERR_AVD_STREAM_INFO "Cannot find stream info."
#define ERR_AVD_FIND_VIDEO_DECODER "Cannot find the video decoder"
#define ERR_AVD_OPEN_VIDEO_CONTEXT "Cannot open the video context: %s"
#define ERR_AVD_ALLOC_VIDEO_CONTEXT "Cannot allocate video codec context"
#define ERR_AVD_NO_VIDEO_STREAM "Cannot find a video stream"
#define ERR_AVD_ALLOC_FRAME "Cannot allocate a avframe"
#define ERR_AVD_COPY_VIDEO_CONTEXT "Cannot copy the video context: %s"
#define V_AVD_EOF "We've read the whole file!"
#define V_AVD_STREAM_NOT_USED "The stream with index: %d is not used"

struct AVDecoderFrame {
  AVDecoderFrame();
  ~AVDecoderFrame();

  AVFrame* frame;
  int64_t pts;                                                   /* the time in millis when we need to present  this packet */
  int type;                                                      /* the frame type AV_TYPE_NONE, AV_TYPE_VIDEO, AV_TYPE_AUDIO */
};

class AVDecoder {
 public:
  AVDecoder();
  ~AVDecoder();
  bool open(AVFormatContext* context);                           /* open using a custom setup AVFormatContext; which might be handy if you need to do your own I/O */
  bool open(std::string filename, bool datapath = false);        /* open a video file */
  bool close();                                                  /* closes the video file, resetting everything that was opened by open() */
  AVDecoderFrame* decodeFrame(bool& isEOF);                      /* caller is responsilbe for freeing the returned frame. if no frame is found we return NULL.  */
  void print();                                                  /* print debug info */
  int getWidth();                                                /* get the width of the video stream */
  int getHeight();                                               /* get the height of the video stream */
  AVPixelFormat getPixelFormat();                                /* get the pixel format used by the video decoder */
  bool isOpen();                                                 /* returns true when the file has been opened correctly and not closed yet */
 private:

  /* muxer */
  AVFormatContext* format_context;

  /* video */
  AVStream* video_stream;                                      
  AVCodec* video_codec;                                        
  AVCodecContext* video_codec_context;                         
  double video_time_millis;                                      /* the video time base in millis, used to set the pts in decodeFrame */
};


inline int AVDecoder::getWidth() {
  assert(video_codec_context);
  assert(video_stream);
  return video_stream->codec->width;
}

inline int AVDecoder::getHeight() {
  assert(video_codec_context);
  assert(video_stream);
  return video_stream->codec->height;
}

inline AVPixelFormat AVDecoder::getPixelFormat() {
  assert(video_codec_context);
  assert(video_stream);
  return video_stream->codec->pix_fmt;
}

inline bool AVDecoder::isOpen() {
  return format_context; // when the format_context is set we assume the file has been opened. in close() we set this to NULL.
}


#endif

