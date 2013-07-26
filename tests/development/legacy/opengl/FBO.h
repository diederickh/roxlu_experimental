#ifndef ROXLU_FBOH
#define ROXLU_FBOH

#include <roxlu/opengl/GL.h>
#include <map>
#include <utility>
#include <stdio.h>

using std::map;

class FBO {
public:
 FBO();
 ~FBO();
 FBO& setup(int nWidth, int nHeight);
 FBO& addTexture(int nAttachmentPoint); 
 GLuint getTextureID(int nAttachmentPoint);
 void bindTexture(int nAttachmentPoint);
 
 void begin();
 void end();
 void bind();
 void unbind();
 int width;
 int height;
 
 GLuint fbo_id;
 map<int, GLuint> textures; // <attachment_point, texture_id>
};

#endif
