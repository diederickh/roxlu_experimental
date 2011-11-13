#include "Effect.h"
#include "Light.h"
#include "VBO.h"
#include "VAO.h"
#include "VertexData.h"
#include <sstream>
#include <cstdlib>

using std::endl;

namespace roxlu {

Effect::Effect()
	:features(EFFECT_FEATURE_NONE)
	,shader_created(false)
	,necessary_vertex_attribs(VERT_POS)
{
}

Effect::~Effect() {
	
}


void Effect::createShader(string& vertShader, string& fragShader) {
	printf("create shader\n");
	// create vertex shader
	stringstream vert;
	stringstream frag;

	// declare variables.
	// -------------------------------------------------------------------------
	
	// we always assume the view, projection matrices and pos are present.
	vert << "attribute vec4 pos; " << endl;
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
	
	if(hasNormals()) {
		vert << "attribute vec3 norm;" << endl;
		vert 	<< "varying vec3 normal;" << endl
				<< "varying vec3 eye_normal; " << endl; 
		frag 	<< "varying vec3 normal;" << endl
				<< "varying vec3 eye_normal;" << endl;
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
				<< "#define LIGHT_COUNT " << getNumberOfLights() << endl
				<< endl;
				
		lights << "uniform Light lights[LIGHT_COUNT];" << endl;
		lights << "varying vec3 light_directions[LIGHT_COUNT];" << endl << endl;
		
		vert << lights.str();
		frag << lights.str();
	}	
	
	// -------------------------------------------------------------------------
	// open shader.
	vert << "void main() {" << endl;
	frag << "void main() {" << endl;

	frag << "\t" << "vec4 texel_color = vec4(0.1, 0.1, 0.1, 1.0);" << endl;
	vert << "\t" << "gl_Position = modelview_projection * pos;" << endl;
	vert << "\t" << "vec4 modelview_position = modelview * pos;" << endl;
	vert << "\t" << "vec3 eye_position = modelview_position.xyz;" << endl;

	
	if(hasTextures()) {
		vert << "\t" << "texcoord = tex;" << endl;
	}
	
	if(hasNormals()) {
		vert << "\t" << "normal = normalize(norm);" << endl;
		
		// for light calculations we need to have a normal in the same view space
		// and therefore we multiply it by the modelview matrix. Note that this
		// will work in most cases but not when the modelview has non-uniform
		// operations. Need to implement this: http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
		vert << "\t" << "eye_normal = vec3(modelview * vec4(normal, 0.0));" << endl;
		//frag << "\t" << "texel_color.rgb += normal;" << endl;
	}
		
	if(hasDiffuseTexture()) {
		frag << "\t" << "vec4 diffuse_color = texture2D(diffuse_texture, texcoord);" << endl;
		frag << "\t" << "texel_color += diffuse_color;" << endl;
	}
	
	if(hasLights()) {
		vert 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t"	<< "light_directions[i] = normalize(lights[i].position-eye_position); " << endl 
				<< "\t" << "}" << endl;
		
		frag 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t"	<< "float n_dot_l = max(dot(eye_normal, light_directions[i]), 0.0); " << endl 
				<< "\t\t"	<< "if(n_dot_l > 0.0) {" << endl
				<< "\t\t\t"		<< "texel_color += (n_dot_l * lights[i].diffuse_color);" << endl
				<< "\t\t"	<< "}" << endl
				<< "\t" << "}" << endl;
	}
	

	frag << "\tgl_FragColor = texel_color;" << endl;
	//frag << "\tgl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);" << endl;
			
	// close shader.
	vert << "}" << endl; // main();
	frag << "}" << endl; // main()
	
	vertShader = vert.str();
	fragShader = frag.str();
	
}

void Effect::setupShader() {
	if(shader_created) {
		return;
	}

	string vert, frag;
	createShader(vert, frag);
	
	printf("---------------------------->>>>>>>>>\n");	
	cout << vert << endl;
	cout << "++++++++++++++++++\n" << endl;
	cout << frag << endl;
	cout << "++++++++++++++++++\n" << endl;	
	
	shader.create(vert,frag);
	
	shader.enable();	
	
	// default uniforms.
	shader.addUniform("modelview");
	shader.addUniform("projection");
	shader.addUniform("modelview_projection");
	
	// add other uniforms
	if(hasTextures()) {
		shader.addUniform("texcoord");
	}
	if(hasDiffuseTexture()) {
		shader.addUniform("diffuse_texture");
	}
	
	// add attributes we need to use.
	if(necessary_vertex_attribs & VERT_POS) {
		shader.addAttribute("pos");
	}	
	if(necessary_vertex_attribs & VERT_TEX) {
		shader.addAttribute("tex");
	}
	if(necessary_vertex_attribs & VERT_NORM) {
		shader.addAttribute("norm");
	}
	
	// add lights.
	for(int i = 0; i < lights.size(); ++i) {
		Light& light = *lights[i];
		stringstream var;
		var << "lights[" << i << "]";
		shader.addUniform(var.str() +".position");
		shader.addUniform(var.str() +".diffuse_color");
		shader.addUniform(var.str() +".specular_color");
	}
	
	shader.disable();
	shader_created = true;
}

void Effect::setupBuffer(VAO& vao, VBO& vbo, VertexData& vd) {
	// make sure we have a shader for this effect.
	printf("Effect: setup buffer\n");
	
	if(!shader_created) {
		setupShader();
		shader_created = true;
	}

	vao.bind(); // keeps also state of the enable vertex attrib arrays.
	vbo.bind();
	shader.enable();
	
	// enable necessary vertex attributes (which get bound to the vao)
	// --------------------------------------------------------------
	if(necessary_vertex_attribs & VERT_POS) {
		shader.enableVertexAttribArray("pos");
	}	
	if(necessary_vertex_attribs & VERT_TEX) {
		shader.enableVertexAttribArray("tex");
	}
	if(necessary_vertex_attribs & VERT_NORM) {
		shader.enableVertexAttribArray("norm");
	}
	
	// set data and set offsets
	// ------------------------------------------
	if(vd.getNumIndices() > 0) {
		vbo.setIndices(vd.getIndicesPtr(), vd.getNumIndices());
	}

	size_t stride = 0;
	int pos_offset = 0;
	int tex_offset = 0;
	int norm_offset = 0;
	
	if(necessary_vertex_attribs == VBO_TYPE_VERTEX_P) {
		printf("Effect: fill vbo with vertex positions.\n");
		vbo.setVertexData(
			 vd.getVertexP()
			,vd.getNumVertices()
		);
		pos_offset = offsetof(VertexP, pos);
		stride = sizeof(VertexP);
	}
	else if(necessary_vertex_attribs == VBO_TYPE_VERTEX_PT) {
		vbo.setVertexData(
			 vd.getVertexPT()
			,vd.getNumVertices()
		);
		pos_offset = offsetof(VertexPT, pos);
		tex_offset = offsetof(VertexPT, tex);
		stride = sizeof(VertexPT);
	}
	else if(necessary_vertex_attribs == VBO_TYPE_VERTEX_PTN) {
		vbo.setVertexData(
			 vd.getVertexPTN()
			,vd.getNumVertices()
		);
		pos_offset = offsetof(VertexPTN, pos);
		tex_offset = offsetof(VertexPTN, tex);
		norm_offset = offsetof(VertexPTN, norm);
		stride = sizeof(VertexPTN);
	}


	// attribute: position
	if(necessary_vertex_attribs & VERT_POS) {
		printf("Effect: set vertex position as attribute.\n");
		glVertexAttribPointer(
				shader.getAttribute("pos")
				,3
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)pos_offset
		); eglGetError();
		
	}	
	
	// attribute: texture
	if(necessary_vertex_attribs & VERT_TEX) {
		glVertexAttribPointer(
				shader.getAttribute("tex")
				,2
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)tex_offset
		); eglGetError();
		
	}
	
	// attirbute: normals
	if(necessary_vertex_attribs & VERT_NORM) {
		glVertexAttribPointer(
				shader.getAttribute("norm")
				,3
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)norm_offset
		); eglGetError();
	}
	vao.unbind();
	shader.disable();
	vbo.unbind();
}


void Effect::updateLights() {
	shader.enable();
	for(int i = 0; i < lights.size(); ++i) {
		Light& l = *lights[i];
		stringstream varname;
		varname << "lights[" << i << "]";
		shader.uniform3fv(varname.str() +".position", l.getPosition().getPtr());
		shader.uniform4fv(varname.str() +".diffuse_color", l.getDiffuseColor().getPtr());
	}
}



}; // roxlu