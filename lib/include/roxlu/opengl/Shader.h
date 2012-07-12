#ifndef ROXLU_SHADERH
#define ROXLU_SHADERH

//#include "OpenGL.h"
#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/io/File.h>

#include <string>
#include <map>
#include <fstream>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

typedef std::map<std::string, GLint> ShaderMap;


namespace roxlu {

class Shader {
	public:
		Shader();
		Shader(const std::string& name);
		Shader& setName(const std::string& name);
		bool load(const std::string& name);
		bool load();
		void link();
		bool create(const std::string& vertexSource, const std::string& fragmentSource); 
		Shader& addAttribute(const std::string& name);
		Shader& addAttribute(const std::string& name, GLuint index); // add attribute, enable vertex attrib array and set index
		Shader& addUniform(const std::string& name);
		Shader& enableVertexAttribArray(const std::string& name);
		GLint getAttribute(const std::string& name);
		GLint getUniform(const std::string& name);
		GLuint getProgID();

		// short hand functions. 
		Shader& u(const std::string& name); // add an uniform
		Shader& a(const std::string& name); // add an attribute
		Shader& a(const std::string& name, GLuint index); // add an attribue, enable vertex attrib array and set index
		
		string getVertexSource(bool inDataPath = true);
		string getFragmentSource(bool inDataPath = true);
				
		void enable();
		void disable();
		
		
		// deletes,  reloads the vertex and fragment shaders from file
		void recreate(std::string& vertShader, std::string& fragShader);
		
		std::string readFile(std::string sFile);
	
		// Slower versions.....	
		Shader& uniform1i(std::string name, GLint x);
		Shader& uniform2i(std::string name, GLint x, GLint y);
		Shader& uniform3i(std::string name, GLint x, GLint y, GLint z);
		Shader& uniform4i(std::string name, GLint x, GLint y, GLint z, GLint w);
		
		Shader& uniform1f(std::string name, GLfloat x);
		Shader& uniform2f(std::string name, GLfloat x, GLfloat y);
		Shader& uniform3f(std::string name, GLfloat x, GLfloat y, GLfloat z);
		Shader& uniform4f(std::string name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

		Shader& uniformMat2fv(std::string name, const GLfloat* matrix, bool transpose = false);
		Shader& uniformMat3fv(std::string name, const GLfloat* matrix, bool transpose = false);
		Shader& uniformMat4fv(std::string name, const GLfloat* matrix, bool transpose = false);	
		
		Shader& uniform1fv(std::string name, const GLfloat* value, int count = 1);
		Shader& uniform2fv(std::string name, const GLfloat* value, int count = 1);
		Shader& uniform3fv(std::string name, const GLfloat* value, int count = 1);
		Shader& uniform4fv(std::string name, const GLfloat* value, int count = 1);

		// Faster versions...
		Shader& uniform1f(GLint position, GLfloat x);
		Shader& uniform2f(GLint position, GLfloat x, GLfloat y);
		Shader& uniform3f(GLint position, GLfloat x, GLfloat y, GLfloat z);
		Shader& uniform4f(GLint position, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	
		Shader& uniform1i(GLint position, GLint x);
		Shader& uniform2i(GLint position, GLint x, GLint y);
		Shader& uniform3i(GLint position, GLint x, GLint y, GLint z);
		Shader& uniform4i(GLint position, GLint x, GLint y, GLint z, GLint w);

		Shader& uniform1fv(GLint position, const GLfloat* value, int count = 1);
		Shader& uniform2fv(GLint position, const GLfloat* value, int count = 1);
		Shader& uniform3fv(GLint position, const GLfloat* value, int count = 1);
		Shader& uniform4fv(GLint position, const GLfloat* value, int count = 1);

		Shader& setTextureUnit(GLuint nUniformID, GLuint nTextureID, int num, GLuint nTextureType = GL_TEXTURE_2D);
		Shader& setTextureUnit(std::string sUniform, GLuint nTextureID, int num, GLuint nTextureType = GL_TEXTURE_2D);	


		ShaderMap uniforms;
		ShaderMap attributes;
		bool enabled;

		std::string name;
		std::string vert_shader_source;
		std::string frag_shader_source;
		GLuint vert_id;
		GLuint frag_id;
		GLuint prog_id;
};

inline GLuint Shader::getProgID() {
	return prog_id;
}


} // roxlu
#endif