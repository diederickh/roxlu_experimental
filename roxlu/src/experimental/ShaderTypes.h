#ifndef ROXLU_SHADERTYPESH
#define ROXLU_SHADERTYPESH

#include <string>
#include "Shader.h"

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

} // namespace roxlu

#endif