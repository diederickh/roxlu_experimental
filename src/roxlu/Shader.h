#ifndef SHADERH
#define SHADERH

#include "OpenGL.h"

#include <string>
#include <map>
#include <fstream>

using std::string;
typedef std::map<std::string, GLuint> ShaderMap;

class Shader {
	public:
		Shader();
		Shader(std::string sName);
		Shader& setName(std::string sName);
		Shader& load(std::string sName);
		Shader& load();
		void create(std::string& rVertexSource, std::string& rFragmentSource); 
		Shader& addAttribute(std::string sName);
		Shader& addUniform(std::string sName);
		Shader& enableVertexAttribArray(std::string sName);
		GLuint getAttribute(std::string sName);
		GLuint getUniform(std::string sName);

		string getVertexSource();
		string getFragmentSource();
				
		Shader& enable();
		Shader& disable();
		
		std::string readFile(std::string sFile);
	
		// Slower versions.....	
		Shader& uniform1i(std::string sName, GLint x);
		Shader& uniform2i(std::string sName, GLint x, GLint y);
		Shader& uniform3i(std::string sName, GLint x, GLint y, GLint z);
		Shader& uniform4i(std::string sName, GLint x, GLint y, GLint z, GLint w);
		
		Shader& uniform1f(std::string sName, GLfloat x);
		Shader& uniform2f(std::string sName, GLfloat x, GLfloat y);
		Shader& uniform3f(std::string sName, GLfloat x, GLfloat y, GLfloat z);
		Shader& uniform4f(std::string sName, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		Shader& uniformMat4f(std::string sName, GLfloat* pMatrix, bool bTranspose = false);

		// Faster versions...
		Shader& uniform1f(GLuint nPosition, GLfloat x);
		Shader& uniform2f(GLuint nPosition, GLfloat x, GLfloat y);
		Shader& uniform3f(GLuint nPosition, GLfloat x, GLfloat y, GLfloat z);
		Shader& uniform4f(GLuint nPosition, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	

		Shader& uniform1i(GLuint nPosition, GLint x);
		Shader& uniform2i(GLuint nPosition, GLint x, GLint y);
		Shader& uniform3i(GLuint nPosition, GLint x, GLint y, GLint z);
		Shader& uniform4i(GLuint nPosition, GLint x, GLint y, GLint z, GLint w);


		Shader& setTextureUnit(GLuint nUniformID, GLuint nTextureID, GLuint nNum, GLuint nTextureType = GL_TEXTURE_2D);
		Shader& setTextureUnit(std::string sUniform, GLuint nTextureID, GLuint nNum, GLuint nTextureType = GL_TEXTURE_2D);	


		ShaderMap uniforms;
		ShaderMap attributes;
		bool enabled;
		std::string name;
		GLuint vert_id;
		GLuint frag_id;
		GLuint prog_id;
};
#endif