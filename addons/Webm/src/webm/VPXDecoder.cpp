#include <webm/VPXDecoder.h>

VPXDecoder::VPXDecoder() 
  :millis_per_frame(0)
  ,frame_timeout(0)
  ,pixels(NULL)
  ,num_frames_encoded(0)
  ,num_frames_decoded(0)
  ,sws(NULL)
{
}

VPXDecoder::~VPXDecoder() {
  if(pixels) {
    delete[] pixels;
  }
  pixels = NULL;

  if(vpx_codec_destroy(&codec)) {
    die();
  }

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  vpx_img_free(&img_out);

  millis_per_frame = 0;
  frame_timeout = 0;
  num_frames_encoded = 0;
  num_frames_decoded = 0;

}

bool VPXDecoder::setup(VPXDecoderSettings cfg) {
  settings = cfg;
  bool result = false;

  if(settings.fmt != VPX_IMG_FMT_RGB24) {
    RX_ERROR(("For now we only support the VPX_IMG_FMT_RGB24 type"));
    return false;
  }

  result = initializeDecoder();
  if(!result) {
    RX_ERROR(("Cannot initialize decoder"));
    return false;
  }

  result = initializeSWS();
  if(!result) {
    return false;
  }

  if(!vpx_img_alloc(&img_out, VPX_IMG_FMT_RGB24, settings.out_w, settings.out_h, 1)) {
    RX_ERROR(("Cannot allocate the out image"));
    return false;
  }

  num_bytes_per_frame = settings.in_w * settings.in_h * 3;
  pixels = new unsigned char[num_bytes_per_frame];
  memset(pixels, 0, num_bytes_per_frame);

  if(!pixels) {
    RX_ERROR(("Out of memory.. can't allocate buffer for read buffer"));
    return false;
  }

  millis_per_frame = (1.0f/settings.fps) * 1000;
  frame_timeout = rx_millis() + millis_per_frame;
  
  return true;
}

void VPXDecoder::decodeFrame(unsigned char* data, int nbytes) {
  RX_WARNING(("decode: %d", nbytes));
  vpx_codec_iter_t iter = NULL;
  vpx_image_t* img;
  
  if(vpx_codec_decode(&codec, data, nbytes, NULL, 0)) {
    die();
  }

  while((img = vpx_codec_get_frame(&codec, &iter))) {
    rescale(img, &img_out);
  }

  ++num_frames_decoded;
}

void VPXDecoder::rescale(vpx_image_t* in, vpx_image_t* out) {
  int h = sws_scale(sws, in->planes, in->stride, 0, settings.in_h,
                    out->planes, out->stride);


  /*
  char filename[512];
  sprintf(filename, "img_%d.png", rx_millis());

  Image img;
  img.copyPixels(out->planes[0], settings.out_w, settings.out_h, 3);
  img.save(rx_to_data_path(filename));
  */
}


bool VPXDecoder::initializeDecoder() {
  RX_VERBOSE(("VPXDecoder usees decoder: %s", vpx_codec_iface_name(decode_interface) ));

  int flags = 0;
  if(vpx_codec_dec_init(&codec, decode_interface, NULL, flags)) {
    die();
    return false;
  }

  return true;
}

bool VPXDecoder::initializeSWS() {
  if(sws != NULL) {
    RX_WARNING(("SWS already initialized."));
    return false;
  }
  
  sws = sws_getContext(settings.in_w, settings.in_h, AV_PIX_FMT_YUV420P,
                       settings.out_w, settings.out_h, avformat(VPX_IMG_FMT_RGB24),
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);

  if(!sws) {
    RX_ERROR(("Cannot initialize sws decoder."));
    return false;
  }

  return true;
}

AVPixelFormat VPXDecoder::avformat(vpx_img_fmt f) {
  switch(f) {
    case VPX_IMG_FMT_RGB24: return AV_PIX_FMT_RGB24; 
    case VPX_IMG_FMT_ARGB: return AV_PIX_FMT_ARGB; 
    case VPX_IMG_FMT_I420: return AV_PIX_FMT_YUV420P;
    default: { return AV_PIX_FMT_NONE; }
  }
}


void VPXDecoder::die() {
  const char* msg = vpx_codec_error_detail(&codec);
  RX_WARNING(("%s, %s", msg, vpx_codec_error(&codec)));
  ::exit(EXIT_FAILURE);
}
