#ifdef ROXLU_GL_WRAPPER

#include <roxlu/opengl/FBO.h>
#include <roxlu/opengl/Error.h>

FBO::FBO() 
:fbo_id(-1)
{

};

FBO::~FBO() {
	if(fbo_id != -1) {
		glDeleteFramebuffers(1, &fbo_id);		
	}
}

FBO& FBO::setup(int nWidth, int nHeight) {
	width = nWidth;
	height = nHeight;
	glGenFramebuffers(1, &fbo_id); eglGetError(); 
	return *this;
}

FBO& FBO::addTexture(int nAttachmentPoint) {
	bind();
	
	// we only use RGBA, 2D textures for now.
	GLuint tex_id;
	glGenTextures(1, &tex_id); eglGetError();
	glBindTexture(GL_TEXTURE_2D, tex_id); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); eglGetError(); // @todo from GL_CLAMP to GL_CLAMP_TO_EDGE while porting to ios
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); eglGetError(); // @todo from GL_CLAMP to GL_CLAMP_TO_EDGE while porting to ios
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); eglGetError();

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width , height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); eglGetError(); // @todo commented while porting to openpgl es
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width , height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); eglGetError(); // @todo opengl es -> http://stackoverflow.com/questions/4064706/ios-using-gl-rgba8
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 +nAttachmentPoint, GL_TEXTURE_2D, tex_id, 0); eglGetError();
	eglCheckFramebufferStatus();
	glBindTexture(GL_TEXTURE_2D,0); // unbind texture
	textures.insert(std::pair<int, GLuint>(nAttachmentPoint, tex_id));
	unbind();
	return *this;
}

GLuint FBO::getTextureID(int nAttachmentPoint) {
	map<int, GLuint>::iterator it = textures.find(nAttachmentPoint);
	if(it != textures.end()) {
		return it->second;
	}
	return -1;
}
void FBO::begin() {
	bind();
#if ROXLU_GL_MODE != ROXLU_GL_STRICT	
	glPushAttrib(GL_VIEWPORT_BIT); eglGetError();
#endif	
	glViewport(0,0, width, height); eglGetError();
	 // clear buffer and fill it with the currently set glClearColor
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FBO::end() {
#if ROXLU_GL_MODE != ROXLU_GL_STRICT	
	glPopAttrib(); eglGetError();
#endif	
	unbind();
}

void FBO::bindTexture(int nAttachmentPoint) {
	GLuint tex_id = getTextureID(nAttachmentPoint);
	if(tex_id != -1) {
		glBindTexture(GL_TEXTURE_2D, tex_id); eglGetError();
	}
	else {
		printf("Cannot bind texture with attachment point: '%d'", nAttachmentPoint);
	}
}



void FBO::bind() {	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_id); eglGetError();
	
}

void FBO::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0); eglGetError();
}

#endif // ROXLU_GL_WRAPPER
