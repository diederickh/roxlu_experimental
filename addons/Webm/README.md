# Webm

Wrapper around the libvpx library to create and playback vpx streams. Goal of this
addon is to create a fully functional encoder and decoder using libvpx so you can use
the great non-patented libvpx library.

## IVF and IVFPlayer

IVF is a fileformat to store raw vpx data. It's a basic format with a file header
describing the width, height, time base etc.. of the video. Each frame has it's own
header that contains the frame size in bytes and the presentation timestamp. The 
`IVF` class parses an IVF file and can be used to playback. The IVF class will make
sure that the frames are played back at the correct timestamps. The `IVFPlayer` class 
is used to display the contents of an IVF file.

## IVFWriter and IVFWriterThreaded

The `IVFWriter` and `IVFWriterThreaded` classes are used to encode raw YUV (I420)
data into encoded data using the libvpx codec. The `IVFWriterThreaded` is the same
ans `IVFWriter` but it encodes frames in a separate thread.

_Basic usage_
````c++
IVFWriterThreaded ivf;

// once
ivf.setup(320, 240, 320, 240, 30); // input and output size + fps (= 30).

// right before you start calling addFrame(), make sure to start the encoder
ivf.start();

// whenever you want to add a new frame to the encoder, do:
ivf.addFrame(webcam.getPixels(), webcam.getNumBytes());

// and when you're ready and want to stop the recording
ivf.stop();
````

## Creating IVF files

To create IVF files you can use avconv. See some examples below:

_Convert from h264 to vpx (only do first 400 frames)_

````bash
./avconv -i big_buck_bunny_720p_surround.avi -vcodec libvpx -frames 400 output.ivf
````

_Convert an image sequence range_

````bash
./avconv -stats -f image2 -i image_%02.bmp output.ivf
````

