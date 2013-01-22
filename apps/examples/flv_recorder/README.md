
 FLV RECORDER:
 -------------
 * The screen is upside down; this is correct for now as this was a test to 
   create a FLV file; I did not focus on the 'visuals'. You can record whatever
   you want and probably want a OS specific pixel buffer transfer to optimize
   downloading of frames from you GPU -> CPU.
 * Records audio + video into a FLV file in real time.
 * See Simulation::setupAudio() -> make sure that you have a working audio in (see console output)
 * See Simulation::setupCapture() -> make sure that your camera input device works (see console output)
