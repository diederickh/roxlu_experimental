#ifndef ROXLU_SHADERTYPESH
#define ROXLU_SHADERTYPESH

#include <string>
#include "Shader.h"

// Created for the PotjePissen project; used for simple drawing of images
namespace roxlu {

const std::string SHADER_PT_VS = " \
	uniform mat4 modelview_matrix; \
	uniform mat4 projection_matrix; \
	attribute vec4 pos; \
	attribute vec2 tex; \
	varying vec2 vtex; \
	\
	void main() { \
		vtex = tex; \
		gl_Position = projection_matrix * modelview_matrix  * pos; \
	} \
";

const std::string SHADER_PT_FS = " \
	uniform sampler2D texture; \
	varying vec2 vtex; \
	void main() { \
		gl_FragColor = texture2D(texture,vtex);\
		\
	} \
";

class ShaderPT {
public:
	ShaderPT();
	~ShaderPT();	
	Shader shader;
	GLuint getTextureUniform();
	void enable();
	void disable();
};


static ShaderPT& shader_pt() {
	static ShaderPT pt;
	return pt;
}

inline GLuint ShaderPT::getTextureUniform() {
	return shader.getUniform("texture");
}

inline void ShaderPT::enable() {
	shader.enable();
}

inline void ShaderPT::disable() {
	shader.disable();
}

} // namespace roxlu

#endif