/*

  AVEncoder
  ---------

  Generic video encoder based on libav: 
  - http://git.libav.org/?p=libav.git;a=blob;f=libavformat/output-example.c;hb=HEAD
  - https://github.com/libav/libav/blob/master/libavformat/output-example.c


  When you use `addVideoFrame()`, `addAudioFrame`, `update()` from different threads
  make sure to synchronize these calls! The `AVEncoderThreaded` does this for you.

 */
#ifndef ROXLU_LIBAV_AVENCODER_H
#define ROXLU_LIBAV_AVENCODER_H

extern "C" {
#  include <libavutil/opt.h> 
#  include <libavfilter/avfilter.h>
#  include <libavfilter/avfiltergraph.h>
#  include <libavfilter/buffersrc.h>
#  include <libavfilter/buffersink.h>
}

#include <av/AVTypes.h>
#include <av/AVUtils.h>

#define ERR_AV_ALREADY_STARTED "Cannot start the AVEncoder twice"
#define ERR_AV_NOT_STARTED "Cannot stop() encoding because we haven't started yet."
#define ERR_AV_TRAILER "Canot write trailer: %s"
#define ERR_AV_OUTPUT_FMT "Cannot guess the output format"
#define ERR_AV_FORMAT_CONTEXT "Cannot allocate the format_context member"
#define ERR_AV_NO_VIDEO_CODEC "Cannot find a video codec that is related to the given filename"
#define ERR_AV_VIDEO_ENC_NOT_FOUND "Cannot find the video encoder"
#define ERR_AV_AUDIO_ENC_NOT_FOUND "Cannot find the audio encoder"
#define ERR_AV_VIDEO_STREAM "Cannot create a new video stream"
#define ERR_AV_AUDIO_STREAM "Cannot create a new audio stream"
#define ERR_AV_OPEN_VIDEO_CODEC "Cannot open the video encoder"
#define ERR_AV_OPEN_AUDIO_CODEC "Cannot open the audio encoder"
#define ERR_AV_ALLOC_CODEC_CONTEXT "Cannot allocate a codec context"
#define ERR_AV_ALLOC_VIDEO_FRAME "Cannot allocate a video frame"
#define ERR_AV_ALLOC_AUDIO_FRAME "Cannot allocate an audio frame"
#define ERR_AV_ALLOC_AVFRAME "avcodec_alloc_frame fails"
#define ERR_AV_ALLOC_BUF_AVFRAME "Cannot av_malloc the buffer for the AVFrame"
#define ERR_AV_OPEN_FILE "Cannot open the output file (permissions correct?)"
#define ERR_AV_SWS "Cannot create a SWS context"
#define ERR_AV_SWS_SCALE "Cannot sws_scale(), returned incorrect result"
#define ERR_AV_ADD_VFRAME_SCALE "Adding a video frame cancelled because SWS_SCALE failed"
#define ERR_AV_WRITE_VIDEO "An error occured when writing a video frame: %s"
#define ERR_AV_WRITE_AUDIO "An error occured when writing an audio frame"
#define ERR_AV_WRONG_MILLIS_FPS "Wrong timebase; we cannot calculate the millis per frame"
#define ERR_AV_LIST_PIX_FMT_CODEC_NOT_OPEN "Cannot list the video codec pixel formats because it's not yet opened"
#define ERR_AV_LIST_SMP_FMT_CODEC_NOT_OPEN "Cannot list the audio codec sample formats because it's not yet opened"
#define ERR_AV_OPEN_AUDIO "Cannot open the audio codec because we don't have a valid audio_codec_context. Did you call addAudioStream()"
#define ERR_AV_ENCODE_AUDIO "Error occured when encoding audio"
#define ERR_AV_FILL_AUDIO_FRAME "Error occured when filling the audio frame before we pass it to the audio encoder: %s"
#define ERR_AV_FILL_VIDEO_FRAME "Error occured when filling the video frame"
#define ERR_AV_VIDEO_FRAME_PIX_FMT "Cannot find the pixel format for the video_out_frame: %d"
#define ERR_AV_ADD_TO_SRC_FILTER "Cannot add the video_frame_out to the src_filter: %s"
#define V_AV_VIDEO_CODEC "> Using video codec: %s"
#define V_AV_AUDIO_CODEC "> Using audio codec: %s"

class AVEncoder {
 public:
  AVEncoder();
  ~AVEncoder();
  bool setup(AVEncoderSettings settings);                              /* call once with the encoder settings */
  bool start(std::string filename, bool datapath = false);             /* call this when you want to start an encoding session */
  bool update();                                                       /* call this as often as possible; this needs to be called at a higher FPS than your set `AVEncoderSettings.time_base_*` FPS value */
  bool stop();                                                         /* call this when you want to stop the current encoding session */
  bool addVideoFrame(unsigned char* data, int64_t pts, size_t nbytes); /* add a raw frame. the format must be AVEncoderSettings.in_pixel_format. Pass the pts for the current frame, make sure it's in the timebase pts x */
  bool addVideoFrame(unsigned char* data, size_t nbytes);              /* add a video frame and let us determine the PTS */
  bool addAudioFrame(uint8_t* data, int nsamples);                     /* add new audio samples */
  bool addAudioFrame(uint8_t* data, int nsamples, int64_t pts);        /* add new audio samples + give pts */
  bool isStarted();                                                    /* returns true when we've started encoding, we test if time_started > 0 */
  void print();                                                        /* prints some valuable info (codecs must be opened) */

  int getAudioInputFrameSizePerChannel(AVEncoderSettings cfg);         /* get the number of bytes the framesize for the audio encoder should be; this is the amount which must be passed into add audio frame. */
  int getAudioInputFrameSizePerChannel(enum AVCodecID codecID,         /* get the audio input framesize for the given audio codec and settings */
                                       enum AVSampleFormat sampleFormat,
                                       int bitrate,
                                       int samplerate);
     
 private:

  /* audio */
  bool addAudioStream(enum AVCodecID codecID);                         /* add an audio stream to the output */
  bool openAudio();                                                    /* opens the audio codec */
  void closeAudio();                                                   /* closes + frees audio codec/encoding related objects */

  /* video */
  bool addVideoStream(enum AVCodecID codecID);                         /* based on the current AVOutputFormat we will add a video stream for the given codec */
  bool openVideo();                                                    /* opens the video_codec_context, must be called after addVideoStream() */ 
  void closeVideo();                                                   /* closes + frees video codec/encoding related objects */

  /* video helpers */
  bool initializeSWS();
  bool rescale(unsigned char* data);
  AVFrame* allocVideoFrame(enum AVPixelFormat pixelFormat,             /* allocates a AVFrame for the given params */
                           int width, int height);
  bool isInputPixelFormatSupportedByCodec();                           /* returns if the given `AVEncoderSettings.in_pixel_format` is supported by the found encoder; this is used to create an sws context if we need to convert the incoming data */
  bool needsSWS();                                                     /* returns true if we need an SWS context. This either means that the input pixel format is not supported by the codec, or the width and height of the output video are not the same as the input */

  /* filter graph */
  bool openFilterGraph();                                              /* we use libavfilters' filter graph to make sure we have a steady stream of AVFRames with video. This is handy to get a cfr (constant frame rate) */
  bool closeFilterGraph();                                             /* closes + frees the filter graph */
  
  /* debug info */
  void listSupportedVideoCodecPixelFormats();                          /* shows the names of the pixel formats that are supported by the video codec */
  void listSupportedAudioCodecSampleFormats();                         /* shows the supported audio codec sample formats ;-) */

 public:
  AVEncoderSettings settings;                                          /* see AVTypes.h for more info. This is used to setup the encoder/output format */

  /* muxer */
  AVOutputFormat* output_format;                                       /* based on the given filename to `start()` we use av_guess_format() to find the appropriate AVOutputFormat. We don't need to deallocate this, but in `stop()` we set this to NULL */
  AVFormatContext* format_context;                                     /* the `AVFormatContext` which is used while muxing, see start() */
  int64_t time_started;                                                /* the time we started; used to make sure the encoding adds new video frames at the same pace as the FPS */
  int64_t millis_per_video_frame;                                      /* how much millis one video frame takes */

  /* audio */
  AVCodecContext* audio_codec_context;
  AVCodec* audio_codec;
  AVStream* audio_stream;
  AVFrame* audio_frame; /* NOT USED FOR NOW! */
  int audio_input_frame_size;                                         /* see example: https://github.com/libav/libav/blob/master/libavformat/output-example.c#L111-L117 */
  uint64_t added_audio_frames;

  /* video */
  AVCodec* video_codec;                                               /* a reference to the video codec in the video stream; we don't need to manage this memory */
  AVCodecContext* video_codec_context;                                /* a reference to the video codec context in the video stream; we don't need to manage this memory */
  AVStream* video_stream;                                             /* the video stream which has been added. we free the stream in `stop()` using the same code as in the example: https://github.com/libav/libav/blob/master/libavformat/output-example.c#L514-L516 */
  AVFrame* video_frame_in;                                            /* the input video frame that will hold the pixels that need to be encoded */
  AVFrame* video_frame_out;                                           /* when the output format is not the same as the input format we need to convert the input->output */
  SwsContext* sws;                                                    /* we use a SwsContext to convert the input when it's not the same size or pixel format as the output */

  /* filter graph */
  AVFilterGraph* filter_graph;                                        /* the filter graph that is used to get a fps filter; this might also be used with other, custom filters */
  AVFilterContext* src_filter;                                        /* the src filter, to which we add frames; this is the input fo the graph */
  AVFilterContext* sink_filter;                                       /* the sink filter, from which we read frames; this is the output of the graph */
  AVFilterContext* fps_filter;                                        /* the fps filter which makes sure we have a steady stream of frames (for real time encoding) */
};

inline bool AVEncoder::isStarted() {            
  return time_started;
}

inline int AVEncoder::getAudioInputFrameSizePerChannel(AVEncoderSettings cfg) {
  return getAudioInputFrameSizePerChannel(cfg.audio_codec, 
                                          cfg.sample_fmt, 
                                          cfg.audio_bit_rate, 
                                          cfg.sample_rate);
}

#endif
