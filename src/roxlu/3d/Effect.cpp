#include "Effect.h"
#include "Light.h"
#include "VBO.h"
#include "VAO.h"
#include "VertexData.h"
#include "Material.h"
#include "File.h"
#include <sstream>
#include <cstdlib>

using std::endl;

namespace roxlu {

Effect::Effect()
	:features(EFFECT_FEATURE_NONE)
	,shader_created(false)
	,necessary_vertex_attribs(VERT_POS)
	,reload_shader_enabled(false)
	,reload_shader_last_modified_vert(0)
	,reload_shader_name("")
{
}

Effect::~Effect() {
	if(reload_shader_name != "") {
		reload_timer.stop();
	}
}

void Effect::createShaders(string& vertShader, string& fragShader) {
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
	vert << "uniform mat4 viewmatrix;" << endl;
	vert << "uniform mat4 modelview;" << endl;
	vert << "uniform mat4 projection;" << endl;
	vert << "uniform mat4 modelview_projection;" << endl;
	vert << "uniform mat3 normalmatrix;" << endl;
	
	if(hasLights()) {
		stringstream lights;
		
		lights 	<< endl
				<< "struct Light {" << endl
				<< "\t" << "vec3 position;" << endl
				<< "\t" << "vec4 ambient_color;" << endl
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
		vert << "\t" << "eye_normal = normalmatrix * normal;" << endl; 
	}
	
	if(hasNormalTexture()) {
		vert << "\t" << "tangent = normalize(tan.xyz);" << endl;
		vert << "\t" << "binormal = cross(eye_normal, tangent) * tan.w;" << endl;
		vert << "\t" << "binormal = normalize(binormal);" << endl; 
		vert << "\t" << "mat3 tangent_space = mat3(tangent, binormal, normalize(eye_normal)); " << endl;
	}
	
	if(hasLights()) {
		vert << endl;
		vert 	<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t" << "vec3 lp = (vec3(viewmatrix * vec4(lights[i].position,1.0)));"<< endl; 
//				<< "\t\t" << "vec3 lp = tangent_space * lights[i].position;"<< endl; 	

		// it looks like we don't want our light direction in tangent space
//		if(hasNormalTexture()) {
//			vert << "\t\t"	<< "light_directions[i] = normalize(tangent_space * (lp - eye_position)); " << endl;
//		}
//		else {
			vert << "\t\t"	<< "light_directions[i] = normalize(lp-eye_position); " << endl;
//		}
				
		vert << "\t" << "}" << endl;
		
	}
	vert << "}" << endl; // close main
	vertShader = vert.str();
}



void Effect::createFragmentShader(string& fragShader) {
	stringstream frag;
	
	frag << "#define LIGHT_COUNT " << getNumberOfLights() << endl << endl;
	frag << "uniform vec4 diffuse_color; " << endl; // color of the scene item
	
	if(hasDiffuseTexture()) {
		frag << "uniform sampler2D diffuse_texture; " << endl;
	}
	
	if(hasNormalTexture()) {
		frag << "uniform sampler2D normal_texture; " << endl; 
				
	}
	
	if(hasLights()) {
		frag 	<< "" << "uniform float specularity;" << endl; // specularity of the scene item.
		frag 	<< "" << "uniform vec3 attenuation;" << endl; 
		
		frag 	<< endl
				<< "struct Light {" << endl
				<< "\t" << "vec3 position;" << endl
				<< "\t" << "vec4 ambient_color;" << endl
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
	frag << "\t" << "vec4 texel_color = diffuse_color;" << endl;
	
	if(hasNormals()) {
		frag << "\t" << "vec3 final_normal = normalize(eye_normal);" << endl;
	}
	
	if(hasNormalTexture()) {
		frag << "\t" << "vec3 normal_color = texture2D(normal_texture, texcoord).xyz * 2.0 - 1.0;" << endl; 
		frag << "\t" << "normal_color = normalize(normal_color);" << endl;
		frag << "\t" << "mat3 tangent_matrix = mat3(tangent, binormal, final_normal);" << endl;
		frag << "\t" << "final_normal = normalize(tangent_matrix * normal_color);" << endl;
	}
			
	if(hasDiffuseTexture()) {
		frag << "\t" << "texel_color += texture2D(diffuse_texture, texcoord);;" << endl;
	}
	
	if(hasLights()) {
		frag	<< "\t"	<< "vec4 lights_ambient = vec4(0.0, 0.0, 0.0, 1.0);" << endl
				<< "\t" << "vec4 lights_diffuse = vec4(0.0, 0.0, 0.0, 1.0);" << endl
				<< "\t" << "vec4 lights_specular = vec4(0.0, 0.0, 0.0, 1.0);" << endl;
				
		frag 	<< endl
				<< "\t" << "for(int i = 0; i < LIGHT_COUNT; ++i) { " << endl
				<< "\t\t"	<< "lights_ambient += lights[i].ambient_color; " << endl
				<< "\t\t"	<< "float n_dot_l = max(dot(normalize(final_normal), light_directions[i]), 0.0); " << endl 
				
				<< "\t\t"	<< "if(n_dot_l > 0.0) {" << endl
				<< "\t\t\t"		<< "lights_diffuse += ((n_dot_l * lights[i].diffuse_color));" << endl
				<< "\t\t\t"		<< "vec3 reflection = normalize(reflect(-normalize(light_directions[i]), final_normal));" << endl
				<< "\t\t\t" 	<< "float spec = max(0.0, dot(final_normal, reflection)); " << endl
				<< "\t\t\t"		<< "float fspec = pow(spec, specularity);" << endl
				<< "\t\t\t"		<< "lights_specular += (fspec * lights[i].specular_color)  ;" << endl
				<< "\t\t"	<< "}" << endl
				<< "\t" << "}" << endl;
	}
	
	// frag color
	if(hasLights()) {
		frag << "\t"  << "gl_FragColor = texel_color * attenuation.x  + (lights_diffuse * attenuation.y )+ (lights_specular * attenuation.z);" << endl;
//		frag << "gl_FragColor = ((texel_color+lights_diffuse) * 0.9) + lights_specular * 0.9;";
	}
	else {
		frag << "\t" << "gl_FragColor = texel_color;" << endl;
	}
	
	// close shader.
	frag << "}" << endl; // main()
	fragShader = frag.str();
	
}

// sets uniforms (this is a separate function because both setup and loadShaders need to use this).
void Effect::setupShaderAttributesAndUniforms() {
	shader.enable();	
	
	// default uniforms.
	shader.addUniform("viewmatrix");
	shader.addUniform("modelview");
	shader.addUniform("projection");
	shader.addUniform("modelview_projection");
	shader.addUniform("normalmatrix");
	
	shader.addUniform("diffuse_color");
	shader.addUniform("specularity");
	shader.addUniform("attenuation");
		
	// added uniforms
	for(int i = 0; i < uniforms.size(); ++i) {
		shader.addUniform(uniforms[i]);
	}
	
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
		shader.addUniform(var.str() +".ambient_color");
		shader.addUniform(var.str() +".diffuse_color");
		shader.addUniform(var.str() +".specular_color");
	}
	shader.disable();

}

void Effect::setupShader() {
	if(shader_created) {
		return;
	}
	
	if(reload_shader_enabled) {
		updateShaders();
	}
	else {
		string vert, frag;
		createShaders(vert, frag);
		shader.create(vert,frag);
		setupShaderAttributesAndUniforms();
		cout << vert << "\n\n---------------\n\n" << frag << "\n\n";
	}
	shader_created = true;

}


void Effect::setupBuffer(VAO& vao, VBO& vbo, VertexData& vd) {
	// make sure we have a shader for this effect.
	if(!shader_created) {
		setupShader();
		shader_created = true;
	}
	
	vao.bind(); // keeps also state of the enable vertex attrib arrays.
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
	
	// now after possibly setting indices, we can bind the vbo
	vbo.bind();

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
		//printf("Effect: set vertex position as attribute.\n");
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

void Effect::bindMaterial(Material& m) {
	shader.enable();
	int n = 0;
	if(m.hasDiffuseTexture()) {
		Texture* tex = m.getDiffuseTexture();
		shader.setTextureUnit("diffuse_texture", tex->getTextureID(), n, GL_TEXTURE_2D);
		tex->bind();
		n++;
	}
	
	if(m.hasNormalTexture()) {
		Texture* tex = m.getNormalTexture();
		shader.setTextureUnit("normal_texture", tex->getTextureID(), n, GL_TEXTURE_2D);
		tex->bind();
		n++;
	}
}

void Effect::updateShaders() {
	if(reload_shader_enabled) {
		shader.setName(reload_shader_name);
		string vs_src = File::getFileContents(reload_shader_name +".vert", reload_shader_datapath);
		string fs_src = File::getFileContents(reload_shader_name +".frag", reload_shader_datapath);
		shader.recreate(vs_src, fs_src);
		setupShaderAttributesAndUniforms();
		reload_shader_enabled = false;
	}
}

void Effect::updateLights() {
	shader.enable();
	for(int i = 0; i < lights.size(); ++i) {
		Light& l = *lights[i];
		stringstream varname;
		varname << "lights[" << i << "]";
		shader.uniform3fv(varname.str() +".position", l.getPosition().getPtr());
		shader.uniform4fv(varname.str() +".ambient_color", l.getAmbientColor().getPtr());
		shader.uniform4fv(varname.str() +".diffuse_color", l.getDiffuseColor().getPtr());
		shader.uniform4fv(varname.str() +".specular_color", l.getSpecularColor().getPtr());
	}
}


// RELOADING/RECOMPILING SHADERS
// -----------------------------------------------------------------------------
// When you call "autoReloadShader()" we will automatically reload the vertex
// and fragment shader with the name name[.vert|.frag] each 200ms. 
// -----------------------------------------------------------------------------
// Save vertex and fragment shader to a file
void Effect::saveShaders(string name, bool inDataPath) {
	string vs, fs;
	createShaders(vs, fs);
	File::putFileContents(name +".vert", vs, inDataPath);
	File::putFileContents(name +".frag", fs, inDataPath);
}

void Effect::autoReloadShaders(string name, bool inDataPath) {
	reload_shader_enabled = true;
	reload_shader_datapath = inDataPath;
	reload_shader_name = name;
	reload_timer.start(*this, 1500,200);
}

// Timer used to check if we need to update the shader
void Effect::onTimer() {
	reloadShaders(reload_shader_name, reload_shader_datapath);
}


// Use custom shader files.
void Effect::reloadShaders(string name, bool inDataPath) {
	string vs = name +".vert";
	string fs = name +".frag";

	bool reload = false;
	time_t new_fs_time = roxlu::File::getTimeModified(fs,inDataPath);
	if(new_fs_time > reload_shader_last_modified_frag) {
		reload = true;
		reload_shader_last_modified_frag = new_fs_time;
	}
	
	time_t new_vs_time = roxlu::File::getTimeModified(vs,inDataPath);
	if(!reload && new_vs_time > reload_shader_last_modified_vert) {
		reload = true;
		reload_shader_last_modified_vert = new_vs_time;
	}
	
	if(reload) {
		reload_shader_last_modified_vert = new_vs_time;
		reload_shader_enabled = true;
		reload_shader_datapath = inDataPath;
		reload_shader_name = name;
	}
}


}; // roxlu