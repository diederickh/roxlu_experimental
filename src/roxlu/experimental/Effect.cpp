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
	,texunit(GL_TEXTURE0)
{
}

Effect::~Effect() {
	
}


void Effect::createShader(string& vertShader, string& fragShader) {
	printf("create shader\n");
	/*
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
	vert << "varying vec3 eye_position;" << endl;
	frag << "varying vec3 eye_position;" << endl;
	
	if(hasTextures()) {
		vert << "attribute vec2 tex;" << endl;
		vert << "varying vec2 texcoord;" << endl;
		frag << "varying vec2 texcoord;" << endl;
	}
	
	if(hasDiffuseTexture()) {
		frag << "uniform sampler2D diffuse_texture; " << endl;
	}
	
	if(hasNormalTexture()) {
		vert 	<< endl
				<< "attribute vec4 tan;" << endl
				<< "varying vec3 tangent;" << endl
				<< "varying vec3 binormal; " << endl 
				<< endl;
		
		frag 	<< endl
				<< "uniform sampler2D normal_texture; " << endl
				<< "varying vec3 tangent;" << endl
				<< "varying vec3 binormal;" << endl
				<< endl;
				
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
	vert << "\t" << "eye_position = modelview_position.xyz;" << endl;
	
	
	if(hasTextures()) {
		vert << "\t" << "texcoord = tex;" << endl;
	}
	
	if(hasNormals()) {
		vert << "\t" << "normal = normalize(vec3(modelview * vec4(norm,1.0)));" << endl;
		
		// for light calculations we need to have a normal in the same view space
		// and therefore we multiply it by the modelview matrix. Note that this
		// will work in most cases but not when the modelview has non-uniform
		// operations. Need to implement this: http://www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/
		vert << "\t" << "eye_normal = normalize(vec3(modelview * vec4(normal, 0.0)));" << endl;
		//frag << "\t" << "texel_color.rgb += normal;" << endl;
	}
	
	if(hasNormalTexture()) {
		vert << "\t" << "tangent = normalize(tan.xyz);" << endl;
		vert << "\t" << "binormal = cross(normal, tangent) * tan.w;" << endl;
		vert << "\t" << "binormal = normalize(binormal);" << endl; 
		
		
		frag << "\t" << "vec3 normal_color = texture2D(normal_texture, texcoord).xyz * 2.0 - 1.0;" << endl; 
		frag << "\t" << "normal_color = normalize(normal_color);" << endl;
		frag << "\t" << "mat3 tangent_matrix = mat3(tangent, binormal, normal);" << endl;
		frag << "\t" << "vec3 final_normal = normalize(tangent_matrix * normal_color);";
	}
	

		
	if(hasDiffuseTexture()) {
		frag << "\t" << "vec4 diffuse_color = texture2D(diffuse_texture, texcoord);" << endl;
		frag << "\t" << "texel_color += diffuse_color;" << endl;
		frag << "\t" << "texel_color.rgb = vec3(0.3,0.3,0.3);" << endl;
	}
	
	if(hasLights()) {
		vert 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t" << "vec3 lp = vec3(modelview * vec4(lights[i].position,1.0));"<< endl
				<< "\t\t"	<< "light_directions[i] = normalize(lp-eye_position); " << endl
				//<< "\t\t"	<< "light_directions[i] = normalize(lights[i].position-view_position); " << endl 
				<< "\t" << "}" << endl;
		
		frag 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t"	<< "float n_dot_l = max(dot(final_normal, light_directions[i]), 0.0); " << endl 
				<< "\t\t"	<< "if(n_dot_l > 0.0) {" << endl
				<< "\t\t\t"		<< "texel_color += (n_dot_l * lights[i].diffuse_color);" << endl

				<< "\t\t\t"		<< "vec3 reflection = normalize(reflect(-normalize(light_directions[i]), final_normal));" << endl
				<< "\t\t\t" 	<< "float spec = max(0.0, dot(final_normal, reflection)); " << endl
				<< "\t\t\t"		<< "float fspec = pow(spec, 428.0);" << endl
				//<< "\t\t\t"		<< "texel_color.rgb += vec3(fspec, fspec, fspec);" << endl
				<< "\t\t\t"		<< "texel_color += fspec * lights[i].specular_color;" << endl
				<< "\t\t"	<< "}" << endl
				<< "\t" << "}" << endl;
	}
	

	frag << "\t" << "gl_FragColor = texel_color;" << endl;
//	frag << "\t" << "gl_FragColor.rgb = normal_color;" << endl;
	//frag << "\tgl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);" << endl;
			
	// close shader.
	vert << "}" << endl; // main();
	
	frag << "}" << endl; // main()
	string vert_shader;
	string frag_shader;

	vertShader = vert.str();
	fragShader = frag.str();
	*/
	createVertexShader(vertShader);
	createFragmentShader(fragShader);
	
}
void Effect::createVertexShader(string& vertShader) {
	stringstream vert;

	vert << "#define LIGHT_COUNT " << getNumberOfLights() << endl << endl;
	vert << "attribute vec4 pos; " << endl;
	
	if(hasTextures()) {
		vert << "attribute vec2 tex;" << endl;
	}

	if(hasNormals()) {
		vert << "attribute vec3 norm;" << endl;
	}

	if(hasNormalTexture()) {
		vert << "attribute vec4 tan;" << endl;
	}
	
	vert << endl; 
	vert << "uniform mat4 modelview;" << endl;
	vert << "uniform mat4 projection;" << endl;
	vert << "uniform mat4 modelview_projection;" << endl;
	
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
				
		lights << "uniform Light lights[LIGHT_COUNT];" << endl;
		vert << lights.str();
	}	
	
	vert << endl;
	
	if(hasTextures()) {
		vert << "varying vec2 texcoord;" << endl;
	}

	if(hasNormals()) {
		vert << "varying vec3 normal;" << endl;
	
	}
	
	if(hasNormalTexture()) {
		vert << "varying vec3 tangent;" << endl
			 << "varying vec3 binormal; " << endl; 
	}
	
	if(hasLights()) {
		vert << "varying vec3 light_directions[LIGHT_COUNT];" << endl;
	}

	vert << "varying vec3 eye_position;" << endl;
	
	if(hasNormals()) {
		vert << "varying vec3 eye_normal; " << endl; 
	}
	
	// -------------------------------------------------------------------------
	vert << endl;
	vert << "void main() {" << endl;
	vert << "\t" << "gl_Position = modelview_projection * pos;" << endl;
	vert << "\t" << "eye_position = vec3(modelview * pos);" << endl;
		
	if(hasTextures()) {
		vert << "\t" << "texcoord = tex;" << endl;
	}
	
	if(hasNormals()) {
		vert << "\t" << "normal = norm;" << endl;
		vert << "\t" << "eye_normal = normalize(vec3(modelview * vec4(normal,0.0)));" << endl;
	}
	
	if(hasNormalTexture()) {
		vert << "\t" << "tangent = normalize(tan.xyz);" << endl;
		vert << "\t" << "binormal = cross(eye_normal, tangent) * tan.w;" << endl;
		vert << "\t" << "binormal = normalize(binormal);" << endl; 
		vert << "\t" << "mat3 tangent_space = mat3(tangent, binormal, eye_normal); " << endl;
	}
	
	if(hasLights()) {
		vert << endl;
		vert 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
			//	<< "\t\t" << "vec3 lp = (lights[i].position);"<< endl;
				<< "\t\t" << "vec3 lp = (vec3(modelview * vec4(lights[i].position, 0.0)));"<< endl;
				
		if(hasNormalTexture()) {
			vert << "\t\t"	<< "light_directions[i] = normalize(tangent_space * (lp - eye_position)); " << endl;
		}
		else {
			vert << "\t\t"	<< "light_directions[i] = normalize(lp-eye_position); " << endl;
		}
				
		vert << "\t" << "}" << endl;
		
	}
	vert << "}" << endl; // close main
	vertShader = vert.str();
}



void Effect::createFragmentShader(string& fragShader) {
	stringstream frag;
	
	frag	<< "#define LIGHT_COUNT " << getNumberOfLights() << endl << endl;
	
	if(hasDiffuseTexture()) {
		frag << "uniform sampler2D diffuse_texture; " << endl;
	}
	
	if(hasNormalTexture()) {
		frag << "uniform sampler2D normal_texture; " << endl; 
				
	}
	
	if(hasLights()) {
		frag 	<< endl
				<< "struct Light {" << endl
				<< "\t" << "vec3 position;" << endl
				<< "\t" << "vec4 diffuse_color;" << endl
				<< "\t" << "vec4 specular_color;" << endl
				<< "};" 
				<< endl
				<< "uniform Light lights[LIGHT_COUNT];" << endl;
	}	

	frag << endl;
	frag << "varying vec3 eye_position;" << endl;
		

	if(hasNormals()) {
		frag << "varying vec3 normal;" << endl
			 << "varying vec3 eye_normal;" << endl;
	}

	if(hasTextures()) {
		frag << "varying vec2 texcoord;" << endl;
	}
	
	if(hasNormalTexture()) {
		frag << "varying vec3 tangent;" << endl
			 << "varying vec3 binormal;" << endl;
	}	

	if(hasLights()) {
		frag << "varying vec3 light_directions[LIGHT_COUNT];" << endl;
	}	
	
	// -------------------------------------------------------------------------
	// open shader.
	frag << endl;
	frag << "void main() {" << endl;

	frag << "\t" << "vec4 texel_color = vec4(0.0, 0.0, 0.0, 1.0);" << endl;
	frag << "\t" << "vec3 final_normal = normalize(eye_normal);" << endl;
	
	if(hasNormalTexture()) {
		frag << "\t" << "vec3 normal_color = texture2D(normal_texture, texcoord).xyz * 2.0 - 1.0;" << endl; 
		frag << "\t" << "normal_color = normalize(normal_color);" << endl;
		// @todo do I need to normalize the tangent, binormal here?
		frag << "\t" << "mat3 tangent_matrix = mat3(tangent, binormal, final_normal);" << endl;
		frag << "\t" << "final_normal = normalize(tangent_matrix * normal_color);" << endl;
	}
			
	if(hasDiffuseTexture()) {
		frag << "\t" << "vec4 diffuse_color = texture2D(diffuse_texture, texcoord);" << endl;
		frag << "\t" << "texel_color += diffuse_color;" << endl;
//		frag << "\t" << "texel_color.rgb = vec3(0.0,0.0,0.0);" << endl;
	}
	
	if(hasLights()) {
		
		frag 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t"	<< "float n_dot_l = max(dot(normalize(final_normal), light_directions[i]), 0.0); " << endl 
				
				<< "\t\t"	<< "if(n_dot_l > 0.0) {" << endl
				<< "\t\t\t"		<< "texel_color += ( 0.4*(n_dot_l * lights[i].diffuse_color));" << endl
				<< "\t\t\t"		<< "vec3 reflection = normalize(reflect(-normalize(light_directions[i]), final_normal));" << endl
				<< "\t\t\t" 	<< "float spec = max(0.0, dot(final_normal, reflection)); " << endl
				<< "\t\t\t"		<< "float fspec = pow(spec, 352.0);" << endl
				//<< "\t\t\t"		<< "texel_color.rgb += vec3(fspec, fspec, fspec);" << endl
				//<< "\t\t\t"		<< "texel_color += (fspec * lights[i].specular_color)  ;" << endl
				<< "\t\t\t"		<< "texel_color += (fspec * lights[i].specular_color)  ;" << endl
				<< "\t\t"	<< "}" << endl
				<< "\t" << "}" << endl;
	}
	

	frag << "\t" << "gl_FragColor = texel_color;" << endl;
//	frag << "\t" << "gl_FragColor = diffuse_color;" << endl;
//	frag << "\t" << "gl_FragColor.rgb = normal_color;" << endl;
//	frag << "\t" << "gl_FragColor.rgb = final_normal;" << endl;			
//	frag << "\t" << "gl_FragColor.rgb = normal;" << endl;			
	// close shader.
	frag << "}" << endl; // main()
	
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
	if(hasDiffuseTexture()) {
		shader.addUniform("diffuse_texture");
	}
	if(hasNormalTexture()) {
		shader.addUniform("normal_texture");
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
	if(necessary_vertex_attribs & VERT_TAN) {
		shader.addAttribute("tan");
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
	if(necessary_vertex_attribs & VERT_TAN) {
		shader.enableVertexAttribArray("tan");
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
	int tan_offset = 0;
	

	
	if(necessary_vertex_attribs == VBO_TYPE_VERTEX_P) {
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
	else if(necessary_vertex_attribs == VBO_TYPE_VERTEX_PN) {	
		vbo.setVertexData(
			 vd.getVertexPN()
			,vd.getNumVertices()
		);
		pos_offset = offsetof(VertexPN, pos);
		norm_offset = offsetof(VertexPN, norm);
		stride = sizeof(VertexPN);
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
	else if(necessary_vertex_attribs == VBO_TYPE_VERTEX_PTNT) {
		vbo.setVertexData(
			 vd.getVertexPTNT()
			,vd.getNumVertices()
		);
		pos_offset = offsetof(VertexPTNT, pos);
		tex_offset = offsetof(VertexPTNT, tex);
		norm_offset = offsetof(VertexPTNT, norm);
		tan_offset = offsetof(VertexPTNT, tan);
		
		stride = sizeof(VertexPTNT);
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
	
	if(necessary_vertex_attribs & VERT_TAN) {
		glVertexAttribPointer(
				shader.getAttribute("tan")
				,4
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)tan_offset
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
//		printf("Update lits: %f, %f, %f\n", l.getPosition().x, l.getPosition().y, l.getPosition().z);
		shader.uniform3fv(varname.str() +".position", l.getPosition().getPtr());
		shader.uniform4fv(varname.str() +".diffuse_color", l.getDiffuseColor().getPtr());
		shader.uniform4fv(varname.str() +".specular_color", l.getSpecularColor().getPtr());
	}
}



}; // roxlu