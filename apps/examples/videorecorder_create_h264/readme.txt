Create raw h264 video file
--------------------------
You can use this example to write a raw h264 file. You won't get much more leightweight then this.
You can use avconv to convert this raw h264 into e.g. mov/mp4 etc.. Use the example below to 
convert:


Convert raw h264 to mp4
-----------------------
avconv -i io_output.h264 -vcodec copy out.mp4
