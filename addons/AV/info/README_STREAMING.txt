Log of streaming info
-----------------------
Tools:
- gstreamer
- vlc
- ffmpeg
- rtmpdump
- crtmpserver


OUTCOME OF RESEARCH
---------------------
- For testing use C++ RTMP Server
- Proof of concept code / crtmpserver settings / html: https://gist.github.com/410d53ca1b89283659ed
- Testing from command line with avconv (from libav): ./avconv -re -i f.mp4 -acodec copy -vcodec copy  -f flv -metadata streamName=test2 "tcp://127.0.0.1:6666"
- Testing avconv, when compiled with libmp3lame enabled (see configure), with sound works too: 
  ./avconv -re -i f.mp4 -b 500000 -s 320x180 -acodec libmp3lame -ab 96000 -ar 44100 -ac 2 -strict experimental -vcodec copy -g 25 -me_method zero -f flv -metadata streamName=video_test "tcp://127.0.0.1:6666" 
- Working stream to influxis:
	./avconv -re -i c.avi -b 500000 -s 320x180 -acodec libmp3lame -ab 96000 -ar 44100 -ac 2 -strict experimental -vcodec libx264 -g 25 -me_method zero -f flv -metadata streamName=livefeed "rtmp://gethinlewis.rtmphost.com/event/_definst_"
- @BBB (Dutch guy,  from #libav/#libav-devel helped a lot! Also @__tim
- For generated images/visuals, use the "flashsv", (Flash ScreenVideo) codec. ( http://www.mplayerhq.hu/DOCS/HTML/en/menc-feat-enc-libavcodec.html )
- For low latency you need an encoder with a low frame_size (AVCodecContext frame_size). i.e. Speex
- Speex is implemented in FLV with 1 channel, 16Khz. ( http://en.wikipedia.org/wiki/Speex )
- Mac hardware accel: http://developer.apple.com/library/mac/#technotes/tn2267/_index.html 
- Nellymoser audio codec, optimized for streaming: ./avconv -re -i c.avi -b 100000 -s 312x176 -acodec nellymoser -ab 32000 -ar 16000 -ac 1 -strict experimental -vcodec libx264 -g 25 -me_method zero -f flv -metadata streamName=video_test "tcp://127.0.0.1:6666"
- Convert video to a smaller size: ./avconv -i test.mp4 -acodec copy -vf "scale=320:180" -vcodec libx264  small_video.mp4



Gstreamer
----------
- appsrc
- gst-launch -v -e videotestsrc ! ffenc_flv ! flvmux ! filesink location="test.flv"
- gst-launch -v videotestsrc ! theoraenc ! oggmux ! tcpserversink host=192.168.1.35 port=8080

- http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-bad-plugins/html/gst-plugins-bad-plugins-rtmpsink.html
- gst-inspect rtmpsink
- run with lots of output: ./appsrctest --gst_debug-level=5 --gst-debug=*:8 &>err.log   
- show message:s: ./appsrctest --gst_debug-level=5 --gst-debug=*:3
- plugins are in the "ext" subdir
- good article on gstreamer (pads, how the pipeline syntax works, queue ! etc..): http://www.cin.ufpe.br/~cinlug/wiki/index.php/Introducing_GStreamer

COMPILING
---------
-- build: the machine you are building on
-- host: the machine you are building for
-- target: the machine that gcc will product code for
-- when you only want to compile one plugin (i.e. png from the good plugins) first make sure to configure with the 
correct settings (ldflags, cflags, etc..), then go into "ext/plugin-name" directory and type: make && make install



Code
-----
- Example of streaming RGB data into gstreamer: https://gist.github.com/725122/16ceee88aafae389bab207818e0661328921e1ab
- RGB instream, goom http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gst-plugins-good-plugins/html/gst-plugins-good-plugins-goom.html
- Stack overflow question: http://stackoverflow.com/questions/5314562/pushing-images-into-a-gstreamer-pipeline 
- Gstreamer Formats: http://gstreamer.freedesktop.org/data/doc/gstreamer/head/pwg/html/section-types-definitions.html
- Streaming to web: http://bitsearch.blogspot.nl/2011/03/streaming-to-html5-web-page.html
- Ogg streaming to web: http://planet.xiph.org/ 
- Writing appsrc data to video file: http://gstreamer-devel.966125.n4.nabble.com/appsrc-to-filesink-td970865.html 
- Example I wrote which creates an appsrc and uses the pull model to send video data: https://gist.github.com/56672069fcf6057c7492
- Example with app settings, blocking, timestamp: http://users.design.ucla.edu/~acolubri/test/gstreamer/appsrc_filesink.c
- Good info on the glib mainloop and how it works for the push model of an app src: http://amarghosh.blogspot.nl/
- GStreamer based live-stream: http://landell.holoscopio.com/
- Mac hardware encoding: http://developer.apple.com/library/mac/#technotes/tn2267/_index.html
- MPEG TS over TCP to rtmpserver https://lists.libav.org/pipermail/libav-api/2011-October/000219.html 
- gstreamer -> c++ rtmp server, something wrong with rtmp sink: https://groups.google.com/forum/?fromgroups#!topic/c-rtmp-server/f1dmcMyjpro
- swscale-example: http://svn.tribler.org/ffmpeg/trunk/libswscale/swscale-example.c

FFMPEG:
--------
- How to live stream to (good article): http://wiki.rtmpd.com/tutorial_live_stream_file
- Working command which streams a file to crtmpserver (though still hangs a bit)
	ffmpeg -i f.mp4 -re -vcodec libx264 -vpre default -vpre baseline -b 500000 -g 25 -me_method zero -acodec aac -ab 9600 -ar 48000 -ac 2 -vbsf h264_mp4toannexb -f mpegts tcp://127.0.0.1:9999
- Streaming segmented video to web: http://www.ioncannon.net/programming/452/iphone-http-streaming-with-ffmpeg-and-an-open-source-segmenter/


LibAV
------
- Using avconv to create a rtmp stream from mp4 to crtmpserver: ./avconv -re -i f.mp4 -f flv -metadata streamName="test" tcp://localhost:6666
- Playback using: https://github.com/revmischa/simplestream 
- Flash livestream playback is different then standard playback
- Nice article how libav is structured and how to use it (mpegts): http://plagatux.es/2011/07/using-libav-library/ 
- Doxygen libav: http://libav.org/doxygen/master/structAVIOContext.html
- LibAV format conversion with filter: https://gist.github.com/1834778
- Post on RGB->YUV -> RTSP http://libav-users.943685.n4.nabble.com/av-write-frame-not-returning-td3760977.html
- Nice post on libav (using avi as input), example of swscale: http://blog.tomaka17.com/2012/03/libavcodeclibavformat-tutorial/
- Has converted RGB-> YUV http://www.afiestas.org/:
- AFiestas: https://twitter.com/afiestas, on KDE freenode
- Swscale with AVFrame: http://libav-users.943685.n4.nabble.com/sws-scale-crashes-td2309626.html
- VBV buffer (MPEG) http://www.bretl.com/mpeghtml/VBV.HTM
- To check if a particular video ENCODER is supported, use the "bin/avconv -codec" list.
- Information about PTS (presentation timestamp): http://libav-users.943685.n4.nabble.com/libx264-xxx-non-strictly-monotonic-PTS-td3275701.html#a3297282
- Transcoding images to video: https://github.com/krieger-od/imgs2video
- Info on PTS and DTS: http://dranger.com/ffmpeg/tutorial05.html
- Setting h264 specs: http://libav-users.943685.n4.nabble.com/coded-frame-gt-pts-not-being-set-by-libx264-after-call-to-avcodec-encode-video-td946931.html
- IRC log of Ruggles (audio dude of libav): https://gist.github.com/599c487e9c3362ae1710 
- Some info an libav / h264 encoding settings: http://www.mplayerhq.hu/DOCS/HTML/en/menc-feat-enc-libavcodec.html 
- Good tutorial on muxers / how structures work in libav: http://libav.org/doxygen/master/group__lavf__decoding.html

Video Codecs:
--------------
- You have I, B, and P frames.
	- I = intra frame = full image
	- P = predicted frame = like an delta/diff, looks back for info.
	- B = bidirectional frame = sames as P, but look back and forward for info!
- To view frames correctly there are PTS and DTS. 
	- PTS: presentation time stamp
	- DTS: decoding timestamp
- Frames can be stored in a different order then that they should be played, 
  i.e. When we have: I B B P, this is stored as I P B B.  The B B need the 
  information from P, so we decode this first, but B is shown before P. 


CRTMP Server
-------------
- Documentation: http://wiki.rtmpd.com/documentation
- Not sure if it's avconv, but when transcoding using libmp3lame sound works! (/avconv -re -i f.mp4 -b 500000 -s 320x180 -acodec libmp3lame -ab 96000 -ar 44100 -ac 2 -strict experimental -vcodec copy -g 25 -me_method zero -f flv -metadata streamName=video_test "tcp://127.0.0.1:6666")

LibAV coding info
------------------
- av_register_all() must be called before you can use "any" of av.
- av_register_all() also initializes the codecs
- Getting this error in crtmpserver: /inboundliveflvprotocol.cpp:102 Frame too large: 16246266 
- when you create a stream with a specific codec id, you don't have to allocate the codeccontext. ( http://libav.org/doxygen/master/group__lavf__core.html#ga5747bd011dd73be7963a7d24da96f77c )
- A muxer only sees streams
- For a audio codec context you must set the format member (sample_fmt)
- AVPacket - contains compressed data
- AVFrame - contains uncompressed data

	LibAV: dts and pts
	------------------
	dts: decoding timestamp
	pts: presentation timestamp

	Because video encoding using the concepts of B, I and P frames which are interleaved with audio
	it can happen that frame 3 is decoded before frame 1. Therefore there is a separate timestamp
	for decoding and presentation in the codec. These timestamps are related to the codec (<--- is this correct).

	Then the muxer/container has a pts/dts for both audio/video. 

	* AVFrame.pts is in units of codec->time_base
	* AVFrame.pts must be set by the user when encoding. 
	* AVPacket.pts/dts  is in units of stream->time_base
	

	-- from: ( http://libav-users.943685.n4.nabble.com/PTS-and-DTS-in-a-packet-aren-t-in-timescale-units-td943755.html#a943760 )
		There's: 
			- p_format_ctx_->streams[video_stream_]->codec->time_base 
			- p_format_ctx_->streams[video_stream_]->time_base 
		The latter is what I use to convert my PTS values, and it seems to work fine. 

	--  pkt->pts can be AV_NOPTS_VALUE if the video format has B frames, so it is better to rely on pkt->dts if you do not decompress the payload.
		(from: http://dranger.com/ffmpeg/functions.html#av_read_frame )

	-- when you are encoding, you set pts on the AVFrame you send to  the encoder
	the encoder will properly set pts and dts on the AVPacket it gives you back.

	-- the time_base is a rational number (numerator/denumerator). The timestamp uses this
	to calculate the time to play back the audio/video. When the time_base is 1/25, aka
	25 frames per second, the pts/dts increment like: 1,2,3,4,5,6,7 etc.. the "real values"
	are then: 1*(1/25), 2*(1/25), 3*(1/25) ... 7*(1/25) etc..

	- @elenril: the muxer timebase is chosen by the muxer
	- @elenril: you need to rescale AVPacket values from encoder timebase to the muxer timebase manually
	- @elenril: but strictly speaking the API requires you to set the timebase yourself before opening the encoder
	- @elenril: i suppose you'll use some microsecond or nanosecond clock, so just set your timebase to microseconds or nanoseconds, i.e. 1/1E6 or 1/1E9

- AVOutputFormat: is just a "hint", it's a static variable
- When debugging you can use tools like this to inspect the bytes:
	- wireshark
	- flv meta: http://code.google.com/p/flvmeta/
	- avprope (show packets), which is part of libav
- Set level with: av_log_set_level (see libavutil/log.h)
- Set the correct stream index of a AVPacket after you've encoded the frame and passing it to interleave_write
- Check if an output format has global headers -> set codec flags if needed.
- H264 variable bitrate: http://stackoverflow.com/questions/10016761/encapsulating-h-264-streams-variable-framerate-in-mpeg2-transport-stream 
- Use this to get the bytes a audio buffer needs to encode data: 
	- int out_size = av_samples_get_buffer_size(&out_line, 1, c->frame_size, AV_SAMPLE_FMT_S16, 0); // we can use this to get the total size
	

HTML 5 streaming standard:
---------------------------
- Standard for web streaming:  http://tools.ietf.org/html/draft-pantos-http-live-streaming-07
- http://tools.ietf.org/html/draft-pantos-http-live-streaming-01


RAW ENCODING:
--------------
- flv standard: http://download.macromedia.com/f4v/video_file_format_spec_v10_1.pdf
- x264 info: http://en.wikipedia.org/wiki/H.264/AVC
- x264 NAL units: http://en.wikipedia.org/wiki/Network_Abstraction_Layer#NAL_Units_in_Byte-Stream_Format_Use
- x264 options: http://mewiki.project357.com/wiki/X264_Settings
- gob size: This option sets the minimum and maximum number of frames before a key frame has to be inserted by x264.


RTMPDUMP
--------
 	- 	replace -soname with -dylib_install_name (in ./rtmpdump/librtmp/Makefile)
 		Make sure that there are no spaces between the commas:
 		SO_LDFLAGS_posix=-shared -Wl,-dylib_install_name,$@

 	- 	Add this on top of BOTH makefiles:
		DESTDIR=/Users/diederickhuijbers/c++/__PLAYGROUND__/libav/build/

	- 	Compile with (on mac)
		make sys=darwin
