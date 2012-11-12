#ifndef ROXLU_DITHERH
#define ROXLU_DITHERH

/*

  Roxlu:
  ------
  This code is based on the excellent code from:
  Retrieved from: http://en.literateprograms.org/Floyd-Steinberg_dithering_(C)?oldid=14630

  Some nice articles and dithering techniques
  http://bisqwit.iki.fi/story/howto/dither/jy/#Appendix%201GammaCorrection
  http://pngnq.sourceforge.net/pngnqsamples.html
  http://members.ozemail.com.au/~dekker/NEUQUANT.HTML
  http://webstaff.itn.liu.se/~sasgo/Digital_Halftoning/Halftoning_Papers/color-dithering-with-n(ny).pdf


  Original license
  ----------------
  Copyright (c) 2012 the authors listed at the following URL, and/or
  the authors of referenced articles or incorporated external code:
  http://en.literateprograms.org/Floyd-Steinberg_dithering_(C)?action=history&offset=20080916082812

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Retrieved from: http://en.literateprograms.org/Floyd-Steinberg_dithering_(C)?oldid=14630

*/

#include <vector>

#define plus_truncate_uchar(a, b)               \
  if (((int)(a)) + (b) < 0)                     \
    (a) = 0;                                    \
  else if (((int)(a)) + (b) > 255)              \
    (a) = 255;                                  \
  else                                          \
    (a) += (b);

#define DXD(i,j,w) ( ((j)*(w)*3) + ((i)*3)  )


class Dither {

 public:
	
  void dither(
              unsigned int w
              ,unsigned int h
              ,unsigned char* input
              ,unsigned char* output
              ,std::vector<MCPoint>& palette
              );

 private:

  unsigned int findNearestColor(unsigned char* pix, std::vector<MCPoint>& palette);

  void disperse(
                unsigned int i
                ,unsigned int j
                ,unsigned int w
                ,unsigned int h
                ,unsigned char* input
                ,MCPoint& pallete
                ,unsigned int channel
                );
};


// Simple color distance.
// -----------------------------------------------------------------------------
inline unsigned int Dither::findNearestColor(unsigned char* pix, std::vector<MCPoint>& palette) {
  std::vector<MCPoint>::iterator it = palette.begin();
  int dist_sq = 0;
  int min_dist_sq = 255*255 + 255*255 + 255*255 + 1;
  int rd, gd, bd = 0;
  int dx = 0;
  int best_index = 0;
  while(it != palette.end()) {
    MCPoint& pt = *it;
    rd = ((int)pix[0]) - pt.x[0];
    gd = ((int)pix[1]) - pt.x[1];
    bd = ((int)pix[2]) - pt.x[2];
    dist_sq = (rd*rd) + (gd*gd) + (bd*bd);
    if(dist_sq < min_dist_sq) {
      min_dist_sq = dist_sq;
      best_index = dx;
    }
    ++dx;
    ++it;
  }
  return best_index;
}


// Floyd–Steinberg dithering
// -----------------------------------------------------------------------------
inline void Dither::disperse(
                             unsigned int i
                             ,unsigned int j
                             ,unsigned int w
                             ,unsigned int h
                             ,unsigned char* input
                             ,MCPoint& palette
                             ,unsigned int channel
                             )
{
  int error = ((int)input[DXD(i,j,w)+channel]) - palette.x[channel];
  if(i + 1 < w) {
    plus_truncate_uchar(input[DXD(i+1,j,w)+channel], (error*7) >> 4);
  }
  if(j + 1 < h) {
    if(i - 1 > 0) {
      plus_truncate_uchar(input[DXD(i-1,j+1,w)+channel], (error*3) >> 4);
    }
    plus_truncate_uchar(input[DXD(i,j+1,w)+channel], (error*5) >> 4);
    if(i + 1 < w) {
      plus_truncate_uchar(input[DXD(i+1,j+1,w)+channel], (error*1) >> 4);
    }
  }
}

inline void Dither::dither(
                           unsigned int w
                           ,unsigned int h
                           ,unsigned char* input
                           ,unsigned char* output  // w * h  (not: w * h * 3), this is the indexed result
                           ,std::vector<MCPoint>& palette
                           ) 
{
  unsigned char* curr_pix;
  int error = 0;
  int dx = 0;
  MCPoint palette_pix;

  unsigned char new_index = 0;
  for(int i = 0; i < w; ++i) {
    for(int j = 0; j < h; ++j) {
		
      // find best palette color.
      int dx = (j * w * 3) + i * 3;
      curr_pix = input+dx;
      new_index = findNearestColor(curr_pix, palette);
      output[j*w+i] = new_index;
      palette_pix = palette[new_index];

      // and disperpse a la http://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
      disperse(i,j,w,h,input,palette_pix, 0);
      disperse(i,j,w,h,input,palette_pix, 1);
      disperse(i,j,w,h,input,palette_pix, 2);
    }
  }
}
		

#endif
