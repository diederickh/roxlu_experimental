#ifndef STB_FONTCHAR__TYPEDEF
#define STB_FONTCHAR__TYPEDEF

#include <cstddef>
#include <iomanip>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <roxlu/core/Log.h>
#include <roxlu/opengl/Error.h>


namespace gl {

  typedef struct {
    // coordinates if using integer positioning
    float s0,t0,s1,t1;
    signed short x0,y0,x1,y1;
    int   advance_int;
    // coordinates if using floating positioning
    float s0f,t0f,s1f,t1f;
    float x0f,y0f,x1f,y1f;
    float advance;
  } stb_fontchar;

#endif

#ifndef ROXLU_OPENGL_FONT_H
#define ROXLU_OPENGL_FONT_H


#define ERR_FONT_UNI_PM "Cannot find a valid u_pm uniform"
#define ERR_FONT_UNI_TEX "Cannot find a valid u_tex uniform"
#define ERR_FONT_UNI_COL "Cannot find a valid u_col uniform"

#define STB_FONT_consolas_14_usascii_BITMAP_WIDTH         128
#define STB_FONT_consolas_14_usascii_BITMAP_HEIGHT         68
#define STB_FONT_consolas_14_usascii_BITMAP_HEIGHT_POW2   128
    
#define STB_FONT_consolas_14_usascii_FIRST_CHAR            32
#define STB_FONT_consolas_14_usascii_NUM_CHARS             95
    
#define STB_FONT_consolas_14_usascii_LINE_SPACING           9
    
  static unsigned int stb__consolas_14_usascii_pixels[]={
    0x03000011,0xeee83b62,0x5c407b51,0xeeb8003d,0x006e7704,0x400001a8,
    0x5c1b912b,0x16c0d4c1,0x3c16a01d,0x45d00cd8,0x9ae881e9,0x27fe445b,
    0x2adb00f2,0x7703e005,0x213ff220,0x2ab7793e,0x21fe7d45,0x3e616e2d,
    0x03bfb221,0x2e17403f,0xb0742d84,0x1e49d319,0x2a003e00,0x19b03c85,
    0x5b3e89d1,0x3d81d1d1,0x1f42d8b7,0x3b79d0fa,0x0ba00f81,0x33169897,
    0x1f31e677,0x3bbb20f2,0x87a00f24,0x33eeefeb,0x4fa0f98f,0xd83cbdea,
    0x4c2d87b4,0x73f11aa7,0x85d01e07,0x6f4ddc5a,0x3f0b73ad,0x326620f2,
    0x8d301e25,0x7099cd98,0x31ec3f0b,0x3be60d33,0xb1f82d84,0xa85bdd09,
    0x2217404c,0x75b3b1be,0x3e172959,0x45b80791,0x640f602d,0x21f85c83,
    0xd901e82d,0x416c911f,0x3ee07d3d,0xd00f6e2e,0x23a59905,0xb73abc8f,
    0x700f23f0,0x0f106a8b,0x7c2dc1e4,0xf7990b60,0x6aecb54b,0x175320b6,
    0x0787f7d0,0x1d16a174,0x3d4b276f,0x5c03c9e6,0x04b81e85,0xf31ea132,
    0x5a3a6098,0x27f4992d,0x0f67d05b,0x00f8d778,0x9e8970ba,0xa7c6cebe,
    0x700f23e8,0x01f0970b,0x8a7c0754,0x13d0dc3e,0xb8754b2d,0x47e0b63f,
    0x4f4cc45b,0x5d007c4e,0x5cd6c4b8,0x7ff4c1b8,0x16e01e44,0x805a83e2,
    0x3fa64ffd,0x4974f84f,0x77fe46ed,0x6a85b1fb,0xffe883e6,0x3a01e83e,
    0x012e25c2,0x403c83d8,0x16c7904c,0x00000000,0x5f116c00,0x204a85c8,
    0x974cc1dc,0x67c42ea8,0x37e60288,0x5413993d,0x45102fa9,0x00000002,
    0x6c5b0000,0x01987d05,0xeee836e2,0x32207b51,0x154402de,0x6f6c9dd7,
    0x00000002,0xd8000000,0x00388122,0x00000000,0x00000000,0x00000000,
    0x00005900,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x172a8000,0x00002f20,0x00000000,0x30cda880,
    0x7ec4598b,0x21640eee,0x440ffffc,0x00171fec,0x2a03d4c8,0xffea82fc,
    0xa81ffe44,0xfff74ffe,0x0dffd885,0x207fff62,0x8be0a9dd,0x2f8a6c5c,
    0x4ce644f8,0xd0cdf441,0x3d4c8003,0x3ea0fa80,0x2376288a,0x4575c2e8,
    0xd07d4128,0x0ce93a29,0x3201f101,0x745d07e5,0x203c8082,0x7003d03d,
    0x36ba9ffd,0x3e1f80df,0x3641f102,0x83e003f1,0x0fc4f62e,0x8f981e60,
    0xfa9dc869,0xb90fbae0,0x3553f035,0x6427ed74,0x5fd4d89d,0x335cc9b1,
    0xff934c0f,0x2dc40354,0xd83aebea,0xefeea82e,0x84cbe85e,0x4c41cdcd,
    0x77ee544c,0x7f97df10,0xf07a27fa,0x2a2ea643,0x3d417f36,0xf19d1d50,
    0x26ff8adc,0xffc83ffc,0x222efb83,0xa8199fa9,0x19af80fe,0x27f01320,
    0x175aa0f8,0x5323cc99,0x434da61e,0x5ff300f9,0x34c6a9e2,0x51b91f44,
    0x30fea07f,0x360bf00f,0x4c83feee,0x4c3f26c0,0xf12643d7,0x2a1ea6c1,
    0x07f10515,0x266a87e2,0xa641a60f,0xb81ae0f8,0x3e203cc5,0x1ea0f881,
    0x21f98132,0x90f5364d,0x29f73e89,0x1265d12e,0x36185fe4,0x0dd85d14,
    0x7c5d30d3,0xb1045762,0xd3107989,0x262be609,0x7ec4c86c,0x7ff441ef,
    0x22643d0d,0xf94bcffa,0x807dc7ff,0x3624fffc,0xffc883ff,0x85dffd4f,
    0xf30dfffa,0x0f309fff,0xdb500f6e,0x7fff419d,0x7400024f,0x00001321,
    0x02000000,0x08802002,0x00000000,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x2e7dfd70,0x00b32a01,0xfd9d93a0,0x2ffff90d,
    0x83ebcc79,0x7d41f53d,0xd85dffd0,0xfff53e85,0x93e21fff,0x3ec5f369,
    0xb90fff62,0x80f69b13,0x7ecc4da9,0xdd2fc42f,0x26629b15,0x7798f37a,
    0x9360f88d,0x3ea2ba2e,0x26b87e61,0x219ce998,0x26d30ff8,0x3e8fd15e,
    0xc87e23f1,0x899007b4,0xe801f52c,0x4de61aa2,0x20f8bcc7,0x2e4fc46a,
    0x2e43e8ba,0x220f605f,0x32ad33df,0x1e67bb8f,0x2991e699,0x1320f63d,
    0x21e805d0,0x8f379869,0x83d8fa79,0x99740feb,0x2dd7c41f,0xf3f107b0,
    0x75e655a6,0x5a8f30f8,0x27b4d8f1,0x4409905d,0xb87a041f,0x98f37985,
    0x880f9ae7,0x1effe85f,0x6c03fd70,0xd37d7883,0x07d7a72e,0x7cbd75bd,
    0x36f69f72,0x5c813204,0x74cba5f8,0x98f37982,0x41aa3e27,0x45741fec,
    0xb013a02e,0x27aaf107,0x07cba976,0xfa92eb2a,0x1fbb4ccf,0x0ec82640,
    0x3fffa7d8,0x51e6f503,0x983dbe8d,0xa9746b8f,0x7b00f607,0xbb674f10,
    0x0b601e84,0x1d77b4c8,0x03ae1320,0x2a03d2e4,0x44fe5d88,0x407edc4d,
    0x22e93a3e,0xd807b03e,0x5bea7883,0x4c41e83c,0x3da6405d,0xf91320d9,
    0x3b621333,0x7fe407a0,0x13ffa60d,0x31ae1be2,0x06a8ba3f,0x883d807b,
    0xe83cb747,0x200fbf61,0xd2ec7b3b,0x3fe9ffff,0xb0316fff,0x00000003,
    0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x83540000,
    0xffe9361f,0x3ffffa3e,0x225b0793,0xf96fffff,0x3ff27fff,0xbdff33ff,
    0xebcc7983,0x6f883dff,0xff31baa0,0xd2c8dfff,0x0d500c41,0x27a2e47e,
    0x21ec4ea8,0x2661d83c,0x33b93f99,0xbcc13213,0x31e62fa9,0x07a627af,
    0xbdb01fb7,0x9933330d,0x3f98f1d8,0x2a7e0d50,0x8d70f40e,0x0f425c3d,
    0x641326b8,0x26b8f304,0x21ebcc79,0x41ed6c3d,0x1f880d71,0x39f9bf93,
    0x83542744,0x21e81fbf,0x52e1ec6c,0x3f301d2d,0x99dc8264,0x4cf31e61,
    0x7d7a999f,0x47c43d53,0x701ae06b,0x754fcc4b,0x506ec409,0x7417fe0d,
    0x1ec2fb9b,0x80fafd6a,0x7fffe43e,0x23dddf91,0xf987c479,0xddd7eeee,
    0xf8adc1bf,0x1f80d700,0x7cc0b378,0x7d7c1aa2,0x81ceee82,0x7e56d43d,
    0x4c86b878,0x51e60990,0x2f31e61f,0x1ecbb11e,0xb81ae07d,0x6e57dcc5,
    0x3542fa82,0x741f99f8,0x3a61ec01,0x7c47bc9d,0x09904c81,0x3ccd91e6,
    0x2d70f5e6,0x06fffff8,0x260fc0d7,0x5c2ddaec,0x3f06a81e,0x401e83ae,
    0xeab7443d,0x8cccef46,0x2609904c,0x3cc3f937,0x4e98f5e6,0x220f88d5,
    0x35c09ec9,0xd98594a8,0xbffff500,0x07a5d87e,0x89fffff4,0xff99be4f,
    0x20997fff,0xf33ffffc,0x47983bff,0x47dffd79,0x3f63e83d,0x817c6fff,
    0x00005b4b,0x00000000,0x00000000,0x00000000,0x30000000,0x0ec500df,
    0xfd927477,0x01efd98d,0x227ff540,0xd904eec8,0x6f5d5c7d,0x23541a81,
    0x4b20b269,0xec96e2e8,0xdbd02eee,0x43d0ed80,0x36ebf14c,0x4c57cc10,
    0x23303d80,0x263362f9,0x3e8ae44e,0x4cb919d9,0x87e099b8,0x3d04ee4c,
    0x107cc7d4,0xb701f511,0xd13a20d5,0x1eb82643,0x03d804d8,0xd51e6990,
    0x2e56a0f8,0x6e6ed491,0x2a1f9320,0xb903e7b4,0x20ba205d,0xe986a8f8,
    0x81321e83,0x40ba2efc,0x419bd998,0xd54feec9,0x3ffe2788,0xa80796ff,
    0x6abcc1ef,0x87b746cc,0x40f605f8,0x1db0d53d,0x402643d0,0x2a0992fa,
    0x20eeefee,0x23cd322d,0x6407e269,0x326ef203,0x221ed742,0xc86aebae,
    0x505b81fd,0x0ec8354b,0x3a4faa64,0x5f33e814,0x1f81ec04,0x7b1f49d3,
    0x03c80dd8,0x1fb701c8,0x8bd71df0,0x2a0fa8fa,0xffffb00e,0x43f505ff,
    0xf14bdfe9,0x513fff6b,0x07b05ffb,0x99337fee,0xfd884ffe,0x100793fe,
    0x6e82fc40,0x917c49f1,0x27ffff4d,0x0e980d50,0x80002000,0x0000001b,
    0x00000001,0x20000000,0x0000006a,0x00000000,0x00000000,0x00000000,
    0xfb3b0000,0x74eedd49,0x307d0083,0xa8b20819,0x1912aaaa,0x4cc00895,
    0x33333319,0x00000013,0xee800000,0xb3f51e89,0x44f88f1b,0x0ea80ebb,
    0xcc8fabf1,0x71f13ccc,0x0c97fa2d,0x3babddd1,0x003eeeee,0x00000000,
    0x2a791740,0x9ba1e795,0xf100b568,0x88888f6b,0x556a3e20,0x000f7d95,
    0x00000000,0x21e80000,0x3cb2b54b,0x16c59264,0xfff86a00,0x894ce5ff,
    0x000072a0,0x00000000,0x5c3d0000,0x33cb2b54,0x0a21c1db,0x00000000,
    0x00000000,0x80000000,0xcad52e1e,0x0000c4f2,0x00000000,0x00000000,
    0x40000000,0xcad52e1e,0x000000f2,0x00000000,0x00000000,0x00000000,
    0x00000000,0x00000000,0x00000000,0x00000000,
  };
    
  static signed short stb__consolas_14_usascii_x[95]={ 0,2,1,0,0,0,0,3,2,1,1,0,1,1,
                                                       2,0,0,0,1,1,0,1,0,0,0,0,2,1,0,0,1,2,0,0,1,0,0,1,1,0,0,1,1,1,
                                                       1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,2,1,1,0,0,0,0,1,1,0,0,0,0,1,1,
                                                       0,1,1,0,1,0,1,0,1,1,0,1,0,0,0,0,1,1,3,1,0, };
  static signed short stb__consolas_14_usascii_y[95]={ 10,0,0,1,-1,0,0,0,-1,-1,0,2,7,5,
                                                       7,0,0,1,0,0,1,1,1,1,0,0,3,3,2,4,2,0,0,1,1,0,1,1,1,0,1,1,1,1,
                                                       1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,0,0,0,1,11,0,3,0,3,0,3,0,3,0,0,
                                                       0,0,0,3,3,3,3,3,3,3,0,3,3,3,3,3,3,0,-2,0,4, };
  static unsigned short stb__consolas_14_usascii_w[95]={ 0,3,5,8,7,8,8,2,4,5,6,8,4,5,
                                                         3,7,8,7,6,6,8,6,7,7,7,7,3,5,7,7,6,5,8,8,6,7,8,6,6,7,7,6,5,7,
                                                         6,8,7,8,6,8,7,7,8,7,8,8,8,8,7,4,6,5,7,8,5,7,6,6,7,7,8,8,6,6,
                                                         6,7,6,8,6,8,6,7,7,6,7,6,8,8,8,8,6,6,2,6,8, };
  static unsigned short stb__consolas_14_usascii_h[95]={ 0,11,4,9,13,11,11,4,14,14,7,8,6,2,
                                                         4,12,11,9,10,11,9,10,10,9,11,10,8,10,9,4,9,11,13,9,9,11,9,9,9,11,9,9,10,9,
                                                         9,9,9,11,9,13,9,11,9,10,9,9,9,9,9,13,12,13,5,2,4,8,11,8,11,8,10,10,10,10,
                                                         13,10,10,7,7,8,10,10,7,8,11,8,7,7,7,10,7,13,15,13,4, };
  static unsigned short stb__consolas_14_usascii_s[95]={ 127,105,68,110,15,109,118,57,10,4,85,
                                                         43,34,83,53,80,30,94,32,47,1,104,111,102,54,1,25,39,10,60,119,
                                                         16,43,85,78,22,61,54,47,39,70,23,52,8,1,118,110,96,16,52,84,
                                                         62,101,58,66,30,75,92,39,61,73,30,39,89,47,52,9,36,1,69,70,
                                                         88,119,97,66,17,25,25,18,60,45,9,77,29,88,18,92,101,110,79,119,
                                                         23,1,36,74, };
  static unsigned short stb__consolas_14_usascii_t[95]={ 1,1,59,40,1,1,1,59,1,1,50,
                                                         50,59,59,59,1,17,40,29,17,50,17,17,40,17,29,50,29,50,59,40,
                                                         17,1,40,40,17,40,40,40,17,40,40,29,40,40,29,29,1,40,1,29,
                                                         17,29,29,29,40,29,29,40,1,1,1,59,59,59,50,17,50,17,50,17,
                                                         17,17,17,1,29,29,59,59,50,29,29,50,50,1,50,50,50,50,17,50,
                                                         1,1,1,59, };
  static unsigned short stb__consolas_14_usascii_a[95]={ 123,123,123,123,123,123,123,123,
                                                         123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
                                                         123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
                                                         123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
                                                         123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
                                                         123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,
                                                         123,123,123,123,123,123,123, };
    
  // Call this function with
  //    font: NULL or array length
  //    data: NULL or specified size
  //    height: STB_FONT_consolas_14_usascii_BITMAP_HEIGHT or STB_FONT_consolas_14_usascii_BITMAP_HEIGHT_POW2
  //    return value: spacing between lines
  static void stb_font_consolas_14_usascii(gl::stb_fontchar font[STB_FONT_consolas_14_usascii_NUM_CHARS],
                                           unsigned char data[STB_FONT_consolas_14_usascii_BITMAP_HEIGHT][STB_FONT_consolas_14_usascii_BITMAP_WIDTH],
                                           int height)
  {
    int i,j;
    if (data != 0) {
      unsigned int *bits = stb__consolas_14_usascii_pixels;
      unsigned int bitpack = *bits++, numbits = 32;
      for (i=0; i < STB_FONT_consolas_14_usascii_BITMAP_WIDTH*height; ++i)
        data[0][i] = 0;  // zero entire bitmap
      for (j=1; j < STB_FONT_consolas_14_usascii_BITMAP_HEIGHT-1; ++j) {
        for (i=1; i < STB_FONT_consolas_14_usascii_BITMAP_WIDTH-1; ++i) {
          unsigned int value;
          if (numbits==0) bitpack = *bits++, numbits=32;
          value = bitpack & 1;
          bitpack >>= 1, --numbits;
          if (value) {
            if (numbits < 3) bitpack = *bits++, numbits = 32;
            data[j][i] = (bitpack & 7) * 0x20 + 0x1f;
            bitpack >>= 3, numbits -= 3;
          } else {
            data[j][i] = 0;
          }
        }
      }
    }
        
    // build font description
    if (font != 0) {
      float recip_width = 1.0f / STB_FONT_consolas_14_usascii_BITMAP_WIDTH;
      float recip_height = 1.0f / height;
      for (i=0; i < STB_FONT_consolas_14_usascii_NUM_CHARS; ++i) {
        // pad characters so they bilerp from empty space around each character
        font[i].s0 = (stb__consolas_14_usascii_s[i]) * recip_width;
        font[i].t0 = (stb__consolas_14_usascii_t[i]) * recip_height;
        font[i].s1 = (stb__consolas_14_usascii_s[i] + stb__consolas_14_usascii_w[i]) * recip_width;
        font[i].t1 = (stb__consolas_14_usascii_t[i] + stb__consolas_14_usascii_h[i]) * recip_height;
        font[i].x0 = stb__consolas_14_usascii_x[i];
        font[i].y0 = stb__consolas_14_usascii_y[i];
        font[i].x1 = stb__consolas_14_usascii_x[i] + stb__consolas_14_usascii_w[i];
        font[i].y1 = stb__consolas_14_usascii_y[i] + stb__consolas_14_usascii_h[i];
        font[i].advance_int = (stb__consolas_14_usascii_a[i]+8)>>4;
        font[i].s0f = (stb__consolas_14_usascii_s[i] - 0.5f) * recip_width;
        font[i].t0f = (stb__consolas_14_usascii_t[i] - 0.5f) * recip_height;
        font[i].s1f = (stb__consolas_14_usascii_s[i] + stb__consolas_14_usascii_w[i] + 0.5f) * recip_width;
        font[i].t1f = (stb__consolas_14_usascii_t[i] + stb__consolas_14_usascii_h[i] + 0.5f) * recip_height;
        font[i].x0f = stb__consolas_14_usascii_x[i] - 0.5f;
        font[i].y0f = stb__consolas_14_usascii_y[i] - 0.5f;
        font[i].x1f = stb__consolas_14_usascii_x[i] + stb__consolas_14_usascii_w[i] + 0.5f;
        font[i].y1f = stb__consolas_14_usascii_y[i] + stb__consolas_14_usascii_h[i] + 0.5f;
        font[i].advance = stb__consolas_14_usascii_a[i]/16.0f;
      }
    }
  }
    
#ifndef STB_SOMEFONT_CREATE
#define STB_SOMEFONT_CREATE              stb_font_consolas_14_usascii
#define STB_SOMEFONT_BITMAP_WIDTH        STB_FONT_consolas_14_usascii_BITMAP_WIDTH
#define STB_SOMEFONT_BITMAP_HEIGHT       STB_FONT_consolas_14_usascii_BITMAP_HEIGHT
#define STB_SOMEFONT_BITMAP_HEIGHT_POW2  STB_FONT_consolas_14_usascii_BITMAP_HEIGHT_POW2
#define STB_SOMEFONT_FIRST_CHAR          STB_FONT_consolas_14_usascii_FIRST_CHAR
#define STB_SOMEFONT_NUM_CHARS           STB_FONT_consolas_14_usascii_NUM_CHARS
#define STB_SOMEFONT_LINE_SPACING        STB_FONT_consolas_14_usascii_LINE_SPACING
#endif
    
  // ---------------------------------------------------------

#include <roxlu/opengl/GL.h>
#include <string>

#if defined(ROXLU_GL_CORE3)
  static const char* FONT_VS = GLSL(150, 
                                    uniform mat4 u_pm;
                                    in vec4 a_pos;
                                    in vec2 a_tex;
                                    out vec2 v_tex; 
                                    void main() {
                                      gl_Position = u_pm * a_pos; 
                                      v_tex = a_tex;
                                    }
  );

  static const char* FONT_FS = GLSL(150, 
                                    uniform sampler2D u_tex;
                                    uniform vec3 u_col;
                                    in vec2 v_tex; 
                                    out vec4 frag_color;

                                    void main() {
                                      vec4 col = texture(u_tex, v_tex);
                                      frag_color.rgb = u_col * col.r ;
                                      frag_color.a = col.a;
                                    }
  );

#else
  static const char* FONT_VS = GLSL(120, 
                                    uniform mat4 u_pm;
                                    attribute vec4 a_pos;
                                    attribute vec2 a_tex;
                                    varying vec2 v_tex; 
                                    void main() {
                                      gl_Position = u_pm * a_pos; 
                                      v_tex = a_tex;
                                    }
  );

  static const char* FONT_FS = GLSL(120, 
                                    uniform sampler2D u_tex;
                                    uniform vec3 u_col;
                                    varying vec2 v_tex; 
                                    void main() {
                                      vec4 col = texture2D(u_tex, v_tex);
                                      gl_FragColor.rgb = u_col ;
                                      gl_FragColor.a = col.a;
                                    }
  );
#endif

  struct Font {
    void init();
  
    GLuint vao;
    GLuint vbo;
    GLuint prog;
    GLuint tex;
    GLint u_pm;
    GLint u_tex;
    GLint u_col;
    size_t allocated;
    size_t num_vertices;
    stb_fontchar fontdata[STB_SOMEFONT_NUM_CHARS];
    unsigned char pixels[STB_SOMEFONT_BITMAP_HEIGHT][STB_SOMEFONT_BITMAP_WIDTH];
    float* buffer;
    float pm[16];
  };

  inline void Font::init() {
    STB_SOMEFONT_CREATE(fontdata, pixels, STB_SOMEFONT_BITMAP_HEIGHT);

    // Texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, STB_SOMEFONT_BITMAP_WIDTH, STB_SOMEFONT_BITMAP_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    // Shader 
    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vert_id, 1, &FONT_VS, NULL);
    glShaderSource(frag_id, 1, &FONT_FS, NULL);

    glCompileShader(vert_id); //eglGetShaderInfoLog(vert_id);
    glCompileShader(frag_id); //eglGetShaderInfoLog(frag_id);

    prog = glCreateProgram(); 

    glAttachShader(prog, vert_id);
    glAttachShader(prog, frag_id);

    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");

    glLinkProgram(prog);
    glUseProgram(prog);

    u_pm = glGetUniformLocation(prog, "u_pm");
    u_tex = glGetUniformLocation(prog, "u_tex");
    u_col = glGetUniformLocation(prog, "u_col");
    
    if(u_pm < 0) {
      RX_ERROR(ERR_FONT_UNI_PM);
      return;
    }
    if(u_tex < 0) {
      RX_ERROR(ERR_FONT_UNI_TEX);
      return;
    }
    if(u_col < 0) {
      RX_ERROR(ERR_FONT_UNI_COL);
      return;
    }
    printf("%d\n", vert_id);
    
    // vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(glGetAttribLocation(prog, "a_pos"));
    glEnableVertexAttribArray(glGetAttribLocation(prog, "a_tex"));
    glVertexAttribPointer(glGetAttribLocation(prog, "a_pos"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)0);
    glVertexAttribPointer(glGetAttribLocation(prog, "a_tex"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)8);

    // get width/height for the ortho matrix
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    // ortho pm
    float n = 0.0;
    float f = 1.0;
    float ww = vp[2]; 
    float hh = vp[3];
    float fmn = f - n;
    pm[1]  = 0.0f;
    pm[2]  = 0.0f;
    pm[3]  = 0.0f;
    pm[4]  = 0.0f;
    pm[6]  = 0.0f;
    pm[7]  = 0.0f;
    pm[8]  = 0.0f;
    pm[9]  = 0.0f;
    pm[11] = 0.0f;
    pm[15] = 1.0f;
    pm[0]  = 2.0f / ww;
    pm[5]  = 2.0f / -hh;
    pm[10] = -2.0f / fmn;
    pm[12] = -(ww)/ww;
    pm[13] = -(hh)/-hh;
    pm[14] = -(f+n)/fmn;
  }

  void glr_draw_string(const ::std::string& str, float x, float y, float r = 1.0, float g = 1.0, float b = 1.0);

  extern Font glr_font;

} // gl
#endif
