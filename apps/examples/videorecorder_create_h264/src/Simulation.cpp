#include <Simulation.h>

Rec::Rec(int inW, int inH, int outW, int outH, int fps, const std::string filepath) 
  :in_width(inW)
  ,in_height(inH)
  ,out_width(outW)
  ,out_height(outH)
  ,fps(fps)
  ,filepath(filepath)
  ,num_frames(0)
{
  fp = fopen(filepath.c_str(), "w+b");
  if(!fp) {
    printf("ERROR: cannot open: %s\n", filepath.c_str());
  }
  printf("in_widht: %d\nin_height:%d\nout_width:%d\nout_height:%d\n",in_width,in_height,out_width,out_height);
  sws = sws_getContext(in_width,
                       in_height,
                       PIX_FMT_RGB24,
                       out_width,
                       out_height,
                       PIX_FMT_YUV420P,
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);
  if(!sws) {
    printf("ERROR: cannot init sws\n");
  }

  // set encoder params.
  x264_param_t* p = &params;
  x264_param_default_preset(p, "veryfast", "zerolatency");

  p->i_threads = 1;
  p->i_width = out_width;
  p->i_height = out_height;
  p->i_fps_num = fps;
  p->i_fps_den = 1;


  p->i_keyint_max = fps;
  p->b_intra_refresh = 1;

  p->rc.i_rc_method = X264_RC_CRF;
  p->rc.f_rf_constant = 25;
  p->rc.f_rf_constant_max = 35;


  // THIS IS REALLY IMPORTANT. 
  // WHEN YOU SET THESE TO 0, YOU CANT USE AVCONV TO CREATE
  // A FLV/MOV/MP4 ETC..  HAVEN'T GOT TIME TO LOOK INTO THIS
  // AS I WOULD EXPECT THAT 0 WOULD WORK
  p->b_annexb = 1;
  p->b_repeat_headers = 1;
    
  x264_param_apply_profile(p, "baseline");

  encoder = x264_encoder_open(&params);
  if(!encoder) {
    printf("ERROR: cannot open encoder\n");
  }

  x264_encoder_parameters(encoder, &params);

  printf("x264 timebase_num: %d\n", p->i_timebase_num);
  printf("x264 timebase_den: %d\n", p->i_timebase_den);
  printf("x264 i_fps_num: %d\n", p->i_fps_num);
  printf("x264 i_fps_den: %d\n", p->i_fps_den);


  x264_picture_alloc(&pic_in, X264_CSP_I420, out_width, out_height);

  // write headers
  int nheaders = 0;
  x264_encoder_headers(encoder, &nals, &nheaders);
  printf("%d\n", nheaders);
  x264_nal_t* n = nals;
  int size = n[0].i_payload + n[1].i_payload + n[2].i_payload;
  if(!fwrite(n[0].p_payload, size, 1, fp)) {
    printf("ERROR: cannot write headers.\n");
  }
  printf("WROTE HEADER!\n");
}
void Rec::addFrame(unsigned char* pixels) {
  printf("%d, %p\n", in_height, sws);
  int src_stride = in_width * 3;

  int h = sws_scale(
                    sws                            // context 
                    ,(const uint8_t* const*)&pixels    // src slice 
                    ,&src_stride                        // src stride
                    ,0                                  // src slice y
                    ,in_height                     // src slice h
                    ,pic_in.img.plane                  // dest
                    ,pic_in.img.i_stride               // dest stride
                    );

  printf("H: %d\n", h);
  /*
  int r = libyuv::RGB24ToI420(pixels
                              ,src_stride
                              ,pic_in.img.plane[0], pic_in.img.i_stride[0]
                              ,pic_in.img.plane[1], pic_in.img.i_stride[1]
                              ,pic_in.img.plane[2], pic_in.img.i_stride[2]
                              ,in_width, in_height);
  */
  pic_in.i_pts = num_frames;
  //  return;
  int num_nals = 0;
  int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
  if(!fwrite(nals[0].p_payload, frame_size, 1, fp)) {
    printf("OHH");
  }
  printf("Frame size: %d\n", frame_size);
  ++num_frames;
}
void Rec::close() {
  fclose(fp);
}

static void setcolor(unsigned char* pixels, int r, int g, int b, int w, int h) {
  for(int i = 0; i < w; ++i) {
    for(int j = 0; j < h; ++j) {
      int dx = (j * w * 3) + i * 3;
      pixels[dx + 0] = r;
      pixels[dx + 1] = g;
      pixels[dx + 2] = b;
    }
  }
}

Simulation::Simulation()
  :SimulationBase()
  ,vid(1024,768,640,480,25, true, false)
  ,rec(1024,768,1024,768,25,"bin/raw.264")
{

  // -----------------------------------
  // THIS IS CURRENTLY WORK IN PROGRESS!
  // -----------------------------------
  test_data = new unsigned char[vid.getInWidth() * vid.getInHeight() * 3];
  for(int j = 0; j < vid.getInHeight(); ++j) {
    for(int i = 0; i < vid.getInWidth(); ++i) {
      int dx = j * vid.getInWidth() * 3 + i * 3;
      test_data[dx + 0] = 0xFF;
      test_data[dx + 1] = 0xFF;
      test_data[dx + 2] = 0x00;
    }
  }
  vid.setIO(&vid_io);
  vid.openFile("bin/io_output.264");
  for(int i = 0; i < 54; ++i) { 
    if(i == 30) {
      setcolor(test_data, 255,0,0,vid.getInWidth(), vid.getInHeight());
    }
    rec.addFrame(test_data);
    vid.addVideoFrame(test_data);
  }
  rec.close();
}

void Simulation::setup() {
  setWindowTitle("H264 recorder example");
  glEnable(GL_DEPTH_TEST);
}

void Simulation::update() {

}

void Simulation::draw() {
  const float* pm = cam.pm().getPtr();
  const float* vm = cam.vm().getPtr();
  const float* nm = cam.nm().getPtr();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);

}

void Simulation::onMouseDown(int x, int y, int button) {
  cam.onMouseDown(x,y);
}

void Simulation::onMouseUp(int x, int y, int button) {

}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  cam.onMouseDragged(x,y);
}

void Simulation::onMouseMove(int x, int y) {

}

void Simulation::onKeyDown(char key) {

}

void Simulation::onKeyUp(char key) {
}
