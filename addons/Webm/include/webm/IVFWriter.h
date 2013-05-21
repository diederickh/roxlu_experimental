/*

 IVFWriter 
 ----------
 This class is used to create IVF files that contains libvpx encoded data. 
 To use this, first call setup, then start() once, and call addFrame with 
 raw YUV I420 data for each frame.
 
 When you're ready, call stop().

*/

#ifndef ROXLU_WEBM_IVF_WRITER_H
#define ROXLU_WEBM_IVF_WRITER_H

#include <webm/VPXEncoder.h>
#include <string>

#define IVF_ERR_WNOT_SETUP "IVFWriter cannot start because you didn't call setup"
#define IVF_ERR_WOPEN "Cannot open the file for writing"
#define IVF_ERR_WNOT_OPEN "Cannot close the file as it's not opened"
#define IVF_ERR_WALREADY_OPEN "The file is already open, first close before opening"
#define IVF_ERR_WCLOSE "Cannot close the ivf file"
#define IVF_ERR_WSHUTDOWN "Cannot shutdown the encoder"
#define IVF_ERR_WNOT_INIT "Cannot initialize the encoder"

void ivf_writer_write_pkt(const vpx_codec_cx_pkt_t* pkt, int64_t pts, void* user);

class IVFWriter {
 public:
  IVFWriter();
  ~IVFWriter();

  bool setup(int inW, int inH,                                                    /* setup, inW/inH is the size of the input image */
             int outW, int outH,                                                  /* outW, outH is the size of the encoded data/output */
             int fps);
  bool start(std::string filename, bool datapath = false);                        /* start recording to the given file */
  bool stop();                                                                    /* closes everything, frees mem, stops/cleans encoder */
  void addFrame(unsigned char* data, size_t nbytes);                              /* add a raw frame (YUV I420) */
  void addFrameWithPTS(unsigned char* data, int64_t pts, size_t nbytes);
 private:
  bool openFile(std::string filename, bool datapath);                             /* opens the given file for writing binary data */
  bool writeHeader();                                                             /* writes the IVF header */
  bool closeFile();                                                               /* closes the current file */

 public:
  FILE* fp;
  VPXSettings settings;
  VPXEncoder encoder;
  uint64_t time_started;
};

#endif
