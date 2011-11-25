#include "Shader.h"
#include "Error.h"
#include "experimental/File.h"

namespace roxlu {

Shader::Shader() 
:name("")
{
}

Shader::Shader(std::string sName) 
:name(sName)
{
}

Shader& Shader::setName(std::string sName) {
	name = sName;
	return *this;
}

Shader& Shader::load(std::string sName) {
	name = sName;
	return load();
}

// load and create a shader
Shader& Shader::load() {
	std::string vs_src = getVertexSource();
	std::string fs_src = getFragmentSource();
	create(vs_src, fs_src);
	return *this;
}

string Shader::getVertexSource(bool inDataPath) {
	std::string vs_file = name +".vert";
	std::string vs_file_path = File::toDataPath(vs_file);
	return File::getFileContents(vs_file_path, inDataPath);
}

string Shader::getFragmentSource(bool inDataPath) {
	std::string vs_file = name +".frag";
	std::string vs_file_path = File::toDataPath(vs_file);
	return File::getFileContents(vs_file_path, inDataPath);
}

void Shader::create(std::string& rVertexSource, std::string& rFragmentSource) {
	vert_shader_source = rVertexSource;
	frag_shader_source = rFragmentSource;
	
	const char* vss_ptr = rVertexSource.c_str();
	const char* fss_ptr = rFragmentSource.c_str();
	
	//std::cout << rVertexSource << std::endl;
	//std::cout << rFragmentSource << std::endl;
	
	// create shader.
	vert_id = glCreateShader(GL_VERTEX_SHADER); eglGetError();
	frag_id = glCreateShader(GL_FRAGMENT_SHADER); eglGetError();

	// set shader source.
	glShaderSource(vert_id, 1, &vss_ptr, NULL); eglGetError();
	glShaderSource(frag_id, 1, &fss_ptr, NULL); eglGetError();
	
	// compile
	glCompileShader(vert_id); eglGetShaderInfoLog(vert_id);
	glCompileShader(frag_id); eglGetShaderInfoLog(frag_id);
	
	// create program, attach and link.
	prog_id = glCreateProgram(); eglGetError();
	glAttachShader(prog_id, vert_id); eglGetError();
	glAttachShader(prog_id, frag_id); eglGetError();
	glLinkProgram(prog_id); eglGetError();
	glUseProgram(prog_id); eglGetError();
	disable();

}

Shader& Shader::addAttribute(std::string attribute) {
	enable();
	ShaderMap::iterator it = attributes.find(attribute);
	if(it != attributes.end()) {
		return *this;
	}
	
	GLuint attribute_id = glGetAttribLocation(prog_id, attribute.c_str()); eglGetError();
	printf("Added attribute: %s(%d) for prog: %d\n", attribute.c_str(), attribute_id, prog_id);
	attributes[attribute] = attribute_id;
	disable();
	return *this;
};

Shader& Shader::addUniform(std::string uniform) {
	enable();
	GLuint uni_loc = -1;
	uni_loc = glGetUniformLocation(prog_id, uniform.c_str()); eglGetError();
	printf("Added uniform: %s at %d for prog: %d\n", uniform.c_str(), uni_loc, prog_id);
	uniforms[uniform] = uni_loc;
	disable();
	return *this;
}

Shader& Shader::enableVertexAttribArray(std::string attribute) {
	enable();
	GLuint attrib = getAttribute(attribute);
	if(attrib != -1) {
		glEnableVertexAttribArray(attrib);eglGetError();
	}
	disable();
	return *this;
}

GLuint Shader::getAttribute(std::string attribute) {
	ShaderMap::iterator it = attributes.find(attribute);
	if(it == attributes.end()) {
		printf("Shader: error while retrieving the attribute: %s\n", attribute.c_str());
		return -1;
	}
	return (it->second);
}

GLuint Shader::getUniform(std::string uniform) {
	// find and return directly.
	ShaderMap::iterator it = uniforms.find(uniform);
	if(it != uniforms.end()) {
		return it->second;
	}
	
	// if not found, try to load it.
	if(it == uniforms.end()) {
		addUniform(uniform);
	}
	
	// and find again.... 
	it = uniforms.find(uniform);
	if(it == uniforms.end()) {
		printf("Shader: error while retrieve the uniform: %s\n", uniform.c_str());
		return -1;
	}
	return it->second;
}

Shader& Shader::enable() {
	glUseProgram(prog_id); eglGetError();
	return *this;
}

Shader& Shader::disable() {
	glUseProgram(0); eglGetError();
	return *this;
}

std::string Shader::readFile(std::string path) {
	std::string result = "";
	std::string line = "";
	std::ifstream ifs(path.c_str());
	if(!ifs.is_open()) {
		return result;
	}
	while(getline(ifs,line)) {
		result += line +"\n";
	}
	return result;
}

// --------- Kind of experimental; reloading and recompiling shaders -----------

// reload vertex/fragment shader using the shader name 
void Shader::recreate(std::string& vertShader, std::string& fragShader) {
	glDeleteShader(vert_id); eglGetError();
	glDeleteShader(frag_id); eglGetError();
	glDeleteProgram(prog_id); eglGetError();
	create(vertShader, fragShader);
	printf("recreated shader: %d, %d\n", vert_id, frag_id);
	
}

// +++++++++++++++++++++++++++ DATA TRANSFER +++++++++++++++++++++++++++++++++++

// ----------------------- slower, but handier data transfers ------------------
Shader& Shader::uniform1i(std::string uniform, GLint x) {
	glUniform1i(getUniform(uniform), x); eglGetError();
	return *this;
}

Shader& Shader::uniform2i(std::string uniform, GLint x, GLint y) {
	glUniform2i(getUniform(uniform), x, y); eglGetError();
	return *this;
}

Shader& Shader::uniform3i(std::string uniform, GLint x, GLint y, GLint z) {
	glUniform3i(getUniform(uniform), x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4i(std::string uniform, GLint x, GLint y, GLint z, GLint w) {
	glUniform4i(getUniform(uniform), x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::uniform1f(std::string uniform, GLfloat x) {
	glUniform1f(getUniform(uniform), x); eglGetError();
	return *this;
}

Shader& Shader::uniform2f(std::string uniform, GLfloat x, GLfloat y) {
	glUniform2f(getUniform(uniform), x, y); eglGetError();
	return *this;
}


Shader& Shader::uniform3f(std::string uniform, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(getUniform(uniform), x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4f(std::string uniform, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(getUniform(uniform), x, y, z, w); eglGetError();
	return *this;
}


Shader& Shader::uniformMat2fv(std::string uniform, GLfloat* matrix, bool transpose) {
	glUniformMatrix2fv(getUniform(uniform), 1, transpose, matrix); eglGetError();
	return *this;
}


Shader& Shader::uniformMat3fv(std::string uniform, GLfloat* matrix, bool transpose) {
	glUniformMatrix3fv(getUniform(uniform), 1, transpose, matrix); eglGetError();
	return *this;
}


Shader& Shader::uniformMat4fv(std::string uniform, GLfloat* matrix, bool transpose) {
	glUniformMatrix4fv(getUniform(uniform), 1, transpose, matrix); eglGetError();
	return *this;
}

Shader& Shader::uniform1fv(std::string uniform, GLfloat* value, int count) {
	glUniform1fv(getUniform(uniform), count, value); eglGetError();
	return *this;
}

Shader& Shader::uniform2fv(std::string uniform, GLfloat* value, int count) {
	glUniform2fv(getUniform(uniform), count, value); eglGetError();
	return *this;
}

Shader& Shader::uniform3fv(std::string uniform, GLfloat* value, int count) {
	glUniform3fv(getUniform(uniform), count, value); eglGetError();
	return *this;
}

Shader& Shader::uniform4fv(std::string uniform, GLfloat* value, int count) {
	glUniform4fv(getUniform(uniform), count, value); eglGetError();
	return *this;
}

Shader& Shader::setTextureUnit(
	 std::string uniform
	,GLuint textureID
	,GLuint num
	,GLuint textureType
) 
{
	return setTextureUnit(getUniform(uniform), textureID, num, textureType);
}	

// ----------------------- fast data transfers ---------------------------------
Shader& Shader::uniform1i(GLint position, GLint x) {
	glUniform1i(position, x); eglGetError();
	return *this;
}

Shader& Shader::uniform2i(GLint position, GLint x, GLint y) {
	glUniform2i(position, x, y); eglGetError();
	return *this;
}

Shader& Shader::uniform3i(GLint position, GLint x, GLint y, GLint z) {
	glUniform3i(position, x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4i(GLint position, GLint x, GLint y, GLint z, GLint w) {
	glUniform4i(position, x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::uniform1f(GLint position, GLfloat x) {
	glUniform1f(position, x); eglGetError();
	return *this;
}

Shader& Shader::uniform2f(GLint position, GLfloat x, GLfloat y) {
	glUniform2f(position, x, y); eglGetError();
	return *this;
}

Shader& Shader::uniform3f(GLint position, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(position, x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4f(GLint position, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(position, x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::uniform1fv(GLint position, GLfloat* value, int count) {
	glUniform1fv(position, count, value); eglGetError();
	return *this;
}

Shader& Shader::uniform2fv(GLint position, GLfloat* value, int count) {
	glUniform2fv(position, count, value); eglGetError();
	return *this;
}

Shader& Shader::uniform3fv(GLint position, GLfloat* value, int count) {
	glUniform3fv(position, count, value); eglGetError();
	return *this;
}

Shader& Shader::uniform4fv(GLint position, GLfloat* value, int count) {
	glUniform4fv(position, count, value); eglGetError();
	return *this;
}

Shader& Shader::setTextureUnit(
	 GLuint nUniformID
	,GLuint nTextureID
	,GLuint nNum
	,GLuint nTextureType) 
{
	enable();
	glActiveTexture(GL_TEXTURE0 +nNum); eglGetError();
	glEnable(nTextureType); eglGetError();
	glBindTexture(nTextureType, nTextureID); eglGetError();
	uniform1i(nUniformID, nNum); eglGetError();

	glDisable(nTextureType); eglGetError();
	//glActiveTexture(GL_TEXTURE0); eglGetError();
	return *this;
}

} // roxlu