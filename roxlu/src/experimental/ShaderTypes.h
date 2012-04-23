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

// gl_FragColor.r = 1.0; \
//
//static Shader& shader_pt() {
//	static Shader pt;
//	static bool created = false;
//	if(!created) {
//		pt.create(SHADER_PT_VS, SHADER_PT_FS);
//		pt.addUniform("projection_matrix");
//		pt.addUniform("modelview_matrix");
//		pt.addUniform("texture");
//		pt.addAttribute("pos");
//		pt.addAttribute("tex");
//		printf("SHADER PT CREATED+++++++++++++++++++++++++++++\n");
//		created = true;
//	}
//	return pt;
//}

} // namespace roxlu

#endif