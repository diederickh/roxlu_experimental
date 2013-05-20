# LibAv

This addons is a wrapper around the awesome [libav](http://www.libav.org) library to encode,
decode, mux and demux media. This addon provides an encoder, decoder and player, all highly
optimized and taking advantage of multi core cpus. Where possible hardware acceleration is 
used by libav.



## AVEncoder && AVEncoderThreaded

These classes are used to encode raw input data to a media stream and mux it into a file 
(I might add network features some day). 

- `AVEncoder::listSupportedVideoCodecPixelFormats()`: Video encoders typically support
  multiple different input pixel formats. If you can provide a pixel format that is supported
  by the codec (so, if it exists in the output of `listSupportedVideoCodecPixelFormats()`), then
  we don't need to convert the input data to the correct pixel format. Set the `AVEncoderSettings.in_pixel_format`
  to the input format you can provide. 
- The `AVEncoder` is still work in progress and for now mostly tested to generate .flv, .mov and .mp4
  using h264 (libx264). In `AVEncoder::open()` we select the ultrafast preset, this makes no sense when 
  encoding with e.g. ogg. I want to add a couple of custom presets like `QUALITY_MEDIUM`,`QUALITY_GOOD` 
  and `QUALITY_BEST` which will automatically set the correct settings for the encoder.

## AVDecoder

The `AVDecoder` class is used to demux and decode a media stream. We use the `AVFormatContext`
which initialized and detects all media streams in the given file. 



## AVPlayer

A wrapper around AVDecoder which implements a player and draws to a openGL texture. The 
AVPlayer is capable of converting the default pixel format of the decoder to an arbitrary
format, although for now we only support the `AV_PIX_FMT_UYVY422` on Mac. 
*NOTE: WE NEED TO UPDATE THIS TEXT WHEN WE ADD SUPPORT FOR OTHER FORMAT OR EVEN CALLBACKS*





## Things to know about libav

### Demuxing

See `AVDecoder.cpp`, some things which you might want to know:

 - `avformat_find_stream_info()` finds streams and initializes video codec contexts. You 
   can use these `AVCodecContext` members of the `AVStreams` which are found in the 
   `AVFormatContext`, but it's better to allocate your own `AVCodecContext` with `avcodec_alloc_context3()`
   and then copy all the parameters from the `AVStream.codec` using `avcodec_copy_context`. 
   This will copy all the parameters; eg. h264 has some extra data in it's header that needs
   to be copied. *update: this results in a memory leak, so we're not using `avcodec_copy_context`
   but allocating it ourself. I sent a mail to the libav-devel mailing list asking about this issue.
   https://gist.github.com/roxlu/47a268ec1ccf6e3be4db*





# TODO

 -  Add support for audio for the encoder (we can only encode video now)
 -  Add support for audio for the decoder (we can only decode video now)
 -  Cleanup all members and test for memory leaks in the `AVEncoder` and `AVEncoderThreaded`
 -  When you play()/stop()/play()/stop()/etc.. a movie the texture buffer shows the last frame; The `VideoCaptureGLSurface` needs to clean the texture with e.g. black.
 -  Look into hardware acceleration with libavcodec
 -  Test if the time base / frame rate of the `AVEncoderSettings` that is passed to `AVEncoder::setup()` is supported (see `AVCodec.supported_framerates`)   
 -  In `AVEncoder.cpp` I want to free the `video_frame_in` and `video_frame_out` members when you call `AVEncoder::stop()`, but `av_frame_free` does not free all memory that was allocated. 
 -  In `AVEncoder.cpp` clean up audio memory + add a check if the sample format is supported by the audio encoder. See [here](http://libav.org/doxygen/master/samplefmt_8h.html#af9a51ca15301871723577c730b5865c5a35eaaad9da207aa4e63fa02fd67fae68) for a list of formats