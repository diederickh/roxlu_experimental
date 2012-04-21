#include "ShaderTypes.h"

namespace roxlu {

ShaderPT::ShaderPT() {
	printf("> ShaderPT!\n");
	shader.create(SHADER_PT_VS, SHADER_PT_FS);
	
	shader	.addAttribute("pos")
			.addAttribute("tex");
	
	shader	.addUniform("modelview_matrix")
			.addUniform("projection_matrix")
			.addUniform("texture");
}

ShaderPT::~ShaderPT() {
}






} // namespace roxlu