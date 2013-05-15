#include <assert.h>
#include <roxlu/core/Utils.h>
#include <webm/IVFWriter.h>

void ivf_writer_write_pkt(const vpx_codec_cx_pkt_t* pkt, int64_t inpts, void* user) {
  IVFWriter* w = static_cast<IVFWriter*>(user);
  assert(w->fp);

  char header[12];
  vpx_codec_pts_t pts = pkt->data.frame.pts;
  rx_put_le_u32(header, pkt->data.frame.sz);
  rx_put_le_u32(header+4, pts & 0xFFFFFFFF);
  rx_put_le_u32(header+8, pts >> 32);

  int r = 0;
  r = fwrite(header, 1, sizeof(header), w->fp);
  r = fwrite(pkt->data.frame.buf, 1, pkt->data.frame.sz, w->fp); 
}

// --------------------------------------------

IVFWriter::IVFWriter()
  :fp(NULL)
  ,time_started(0)
{
}

IVFWriter::~IVFWriter() {
  stop();
}

bool IVFWriter::setup(int inW,
                      int inH,
                      int outW,
                      int outH,
                      int fps
                      )
{

  settings.in_w = inW;
  settings.in_h = inH;
  settings.out_w = outW;
  settings.out_h = outH;
  settings.fps = fps;
  settings.fmt = AV_PIX_FMT_UYVY422; /* this is used by the logitech cam on mac */
  settings.cb_write = ivf_writer_write_pkt;
  settings.cb_user = this;

  if(!encoder.setup(settings)) {
    return false;
  }

  return true;
}

bool IVFWriter::start(std::string filename, bool datapath) {
  if(!settings.in_w) {
    RX_ERROR(IVF_ERR_WNOT_SETUP);
    return false;
  }

  if(!encoder.initialize()) {
    RX_ERROR(IVF_ERR_WNOT_INIT);
    return false;
  }

  if(!openFile(filename, datapath)) {
    return false;
  }

  if(!writeHeader()) {
    return false;
  }

  time_started = rx_millis();

  return true;
}

bool IVFWriter::stop() {

  if(!closeFile()) {
    RX_ERROR(IVF_ERR_WCLOSE);
    return false;
  }

  if(!encoder.shutdown()) {
    RX_ERROR(IVF_ERR_WSHUTDOWN);
    return false;
  }

  return true;
}

bool IVFWriter::openFile(std::string filename, bool datapath) {

  if(fp) {
    RX_ERROR(IVF_ERR_WALREADY_OPEN);
    return false;
  }

  if(datapath) {
    filename = rx_to_data_path(filename);
  }

  fp = fopen(filename.c_str(), "wb");
  if(!fp) {
    RX_ERROR(IVF_ERR_WOPEN);
    return false;
  }

  return true;
}

bool IVFWriter::writeHeader() {
  if(!fp) {
    RX_ERROR(IVF_ERR_WNOT_OPEN);
    return false;
  }

  if(!settings.in_w) {
    RX_ERROR(IVF_ERR_WNOT_SETUP);
    return false;
  }

  char header[32];
  header[0] = 'D';
  header[1] = 'K';
  header[2] = 'I';
  header[3] = 'F';
  
  rx_put_le_u16(header+4, 0);                 /* version */
  rx_put_le_u16(header+6, 32);                /* header size */
  rx_put_le_u32(header+8, 0x30385056);        /* four cc */
  rx_put_le_u16(header+12, settings.out_w);   /* width */
  rx_put_le_u16(header+14, settings.out_h);   /* height */
  rx_put_le_u32(header+16, settings.fps);     /* rate */
  rx_put_le_u32(header+20, 1);                /* scale */
  rx_put_le_u32(header+24, 0);                /* frame count */
  rx_put_le_u32(header+28, 0);                /* unused */

  fwrite(header, 1, 32, fp);

  return true;
}

bool IVFWriter::closeFile() {
  if(!fp) {
    RX_ERROR(IVF_ERR_WNOT_OPEN);
    return false;
  }

  fclose(fp);
  fp = NULL;
  return true;
}

void IVFWriter::addFrame(unsigned char* data, size_t nbytes) { 
  int millis_per_frame = (1.0f / settings.fps) * 1000;
  int64_t curr_pts = (rx_millis() - time_started) / millis_per_frame;
  addFrameWithPTS(data, curr_pts, nbytes);
}

void IVFWriter::addFrameWithPTS(unsigned char* data, int64_t pts, size_t nbytes) {
  assert(fp);
  assert(settings.in_w);

  encoder.encode(data, pts);
}
