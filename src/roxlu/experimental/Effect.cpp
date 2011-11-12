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
		vert << "varying vec3 normal;" << endl;
		frag << "varying vec3 normal;" << endl;
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
				
		lights << "uniform Light lights[" << getNumberOfLights() << "];" << endl;
		lights << "varying vec3 light_directions;" << endl << endl;
		
		vert << lights.str();
		frag << lights.str();
	}	
	
	// -------------------------------------------------------------------------
	// open shader.
	vert << "void main() {" << endl;
	frag << "void main() {" << endl;

	frag << "\t" << "vec4 texel_color = vec4(0.1, 0.1, 0.1, 1.0);" << endl;
	vert << "\tgl_Position = modelview_projection * pos;" << endl;

	if(hasTextures()) {
		vert << "\t" << "texcoord = tex;" << endl;
	}
	
	if(hasDiffuseTexture()) {
		frag << "\t" << "vec4 diffuse_color = texture2D(diffuse_texture, texcoord);" << endl;
		frag << "\t" << "texel_color += diffuse_color;" << endl;
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
	shader.create(vert,frag);

//	printf("---------------------------->>>>>>>>>\n");	
//	cout << vert << endl;
//	cout << "++++++++++++++++++\n" << endl;
//	cout << frag << endl;
//	cout << "++++++++++++++++++\n" << endl;	
	
	
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
	
	/*
	initialized = true;
	vao->bind();
	shader->enable();

	// create the VBO
	vbo = new VBO();
	Vertex*	vertex = NULL;
	size_t stride = 0;
	int pos_offset = 0;
	int tex_offset = 0;
	int norm_offset = 0;
	VertexData* vd = vertex_data;
	
	// when we have shared vertices (aka indices) set them.
	if(vd->getNumIndices() > 0) {
		vbo->setIndices(vd->getIndicesPtr(), vd->getNumIndices());
	}

	// set data and set offsets
	// ------------------------------------------
	if(vd->attribs == VBO_TYPE_VERTEX_P) {
		stride = sizeof(VertexP);
		vbo->setVertexData(
			 vd->getVertexP()
			,vd->getNumVertices()
		);
		pos_offset = offsetof(VertexP, pos);
	}
	else if(vd->attribs == VBO_TYPE_VERTEX_PT) {
		stride = sizeof(VertexPT);
		vbo->setVertexData(
			 vd->getVertexPT()
			,vd->getNumVertices()
		);
		pos_offset = offsetof(VertexPT, pos);
		tex_offset = offsetof(VertexPT, tex);
	}
	else if(vd->attribs == VBO_TYPE_VERTEX_PTN) {
		stride = sizeof(VertexPTN);
		vbo->setVertexData(
			 vd->getVertexPTN()
			,vd->getNumVertices()
		);
		pos_offset = offsetof(VertexPTN, pos);
		tex_offset = offsetof(VertexPTN, tex);
		norm_offset = offsetof(VertexPTN, norm);
	}
		
	// set attributes in shader
	// ------------------------------------------
	if(vd->attribs & VERT_POS) {
		GLuint pos_id = shader->addAttribute("pos").getAttribute("pos");
		if(pos_id != -1) {
			shader->enableVertexAttribArray("pos");
			glVertexAttribPointer(
				pos_id
				,3
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)pos_offset
			); eglGetError();
		}
	}	
	// attribute: texture
	if(vd->attribs & VERT_TEX) {
		GLuint tex_id = shader->addAttribute("tex").getAttribute("tex");
		if(tex_id != -1) {
			shader->enableVertexAttribArray("tex");
			glVertexAttribPointer(
				tex_id
				,2
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)tex_offset
			); eglGetError();
		}
	}
	
	// attirbute: normals
	if(vd->attribs & VERT_NORM) {
		GLuint norm_id = shader->addAttribute("norm").getAttribute("norm");
		if(norm_id != -1) {
			shader->enableVertexAttribArray("norm");
			glVertexAttribPointer(
				norm_id
				,3
				,GL_FLOAT
				,GL_FALSE
				,stride
				,(GLvoid*)norm_offset
			); eglGetError();
		}
	}
		
	// set diffuse texture
	if(material->hasDiffuseMaterial()) {
		shader->addUniform("diffuse_texture");
		Texture* tex = material->getDiffuseMaterial();
		shader->setTextureUnit("diffuse_texture", tex->getTextureID(), 0, GL_TEXTURE_2D);
	}	
		
	/
	shader->disable();

	*/
	
	// create attribs flag
	int attribs = 0;
	if(hasTexCoords()) {
		
	}
}




}; // roxlu