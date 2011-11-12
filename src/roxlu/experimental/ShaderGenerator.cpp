#include "ShaderGenerator.h"
#include <sstream>

using namespace std;

namespace roxlu {

ShaderGenerator::ShaderGenerator()
	:features(SHADER_FEATURE_NONE)
	,number_of_lights(0)
{
}

ShaderGenerator::~ShaderGenerator() {
}

void ShaderGenerator::createShader(string& vertShader, string& fragShader) {
	printf("create shader\n");
	// create vertex shader
	stringstream vert;
	stringstream frag;

	// declare variables.
	// -------------------------------------------------------------------------

	
	// we always assume the view, projection matrices and pos are present.
	vert << "uniform mat4 modelview;" << endl;
	vert << "uniform mat4 projection;" << endl;
	vert << "uniform mat4 modelview_projection;" << endl;
	
	if(hasTextures()) {
		vert << "attribute vec2 tex;" << endl;
		vert << "varying vec2 texcoord;" << endl;
		frag << "varying vec2 texcoord;" << endl;
	}
	
	if(hasDiffuseTexture()) {
		frag << "uniform sampler2D diffuse_texture; " << endl;
	}

	if(hasLights()) {
		stringstream lights;
		lights 	<< endl
				<< "struct Light {" << endl
				<< "\t" << "vec3 position;" << endl
				<< "\t" << "vec4 diffuse_color;" << endl
				<< "\t" << "vec4 specular_color;" << endl
				<< "};" 
				<< endl 
				<< endl;
				
		lights << "uniform Light lights[" << number_of_lights << "];" << endl;
		lights << "varying vec3 light_directions;" << endl << endl;
		
		vert << lights.str();
		frag << lights.str();
	}	
	
	// -------------------------------------------------------------------------
	// open shader.
	vert << "void main() {" << endl;
	frag << "void main() {" << endl;
	
	vert << "\tgl_Position = modelview_projection * pos;" << endl;
	if(hasTextures()) {
		vert << "\ttexcoord = tex;" << endl;
	}
	
	if(hasDiffuseTexture()) {
		frag << "\tvec4 diffuse_color = texture2D(diffuse_texture, texcoord);" << endl;
		frag << "\tgl_FragColor = diffuse_color;" << endl;
	}
		
	
	// close shader.
	vert << "}" << endl; // main();
	frag << "}" << endl; // main()
	
	vertShader = vert.str();
	fragShader = frag.str();
	
}

}; // roxlu
