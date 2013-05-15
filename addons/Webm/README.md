Webm
-----
Wrapper around the libvpx library to create and playback vpx streams. Goal of this
addon is to create a fully functional encoder and decoder using libvpx so you can use
the great non-patented libvpx library.

IVF and IVFPlayer
-----------------
IVF is a fileformat to store raw vpx data. It's a basic format which a file header
describing the width, height, time base etc.. of the video. Each fame has it's own
header that contains the frame size in bytes and the presentation timestamp. The 
`IVF` class parses an IVF file and can be used to playback. The IVF class will make
sure that the frames are played back at the correct timestamps. The `IVFPlayer` class 
is used to display the contents of an IVF file.

Creating IVF files
------------------
To create IVF files you can use avconv. See some examples below:

_Convert from h264 to vpx (only do first 400 frames)_

````bash
./avconv -i big_buck_bunny_720p_surround.avi -vcodec libvpx -frames 400 output.ivf
````

_Convert an image sequence range_

````bash
./avconv -stats -f image2 -i image_%02.bmp output.ivf
````

