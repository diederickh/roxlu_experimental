#include "Shader.h"
#include "Error.h"

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

string Shader::getVertexSource() {
	printf("ERROR SHADER: need to create/user a datapath function!");
	//std::string base_path = ofToDataPath("",true);
	std::string base_path = "./"; // @todo need to implement data path directory. (maybe a custom File object)
	std::string vs_file = base_path +name +".vert";
	/*
	ofFile vf(vs_file);
	if(!vf.exists()) {
		printf("File not found: '%s'", vs_file.c_str());
	}
	
	std::string vs_src = readFile(vs_file);
	return vs_src;
	*/
	return "";
}

string Shader::getFragmentSource() {
	printf("ERROR/SHADER: need to implement a custom file object!");
	/*
	std::string base_path = ofToDataPath("./",true);
	std::string fs_file = base_path +name +".frag";

	ofFile ff(fs_file);
	if(!ff.exists()) {
		printf("File not found: '%s'", fs_file.c_str());
	}
	
	std::string fs_src = readFile(fs_file);
	return fs_src;
	*/
	return "";
}

void Shader::create(std::string& rVertexSource, std::string& rFragmentSource) {
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

Shader& Shader::addAttribute(std::string sName) {
	enable();
	ShaderMap::iterator it = attributes.find(sName);
	if(it != attributes.end()) {
		return *this;
	}
	GLuint attribute_id = glGetAttribLocation(prog_id, sName.c_str()); eglGetError();
	printf("Added attribute: %d", attribute_id);
	attributes[sName] = attribute_id;
	disable();
	return *this;
};

Shader& Shader::addUniform(std::string sName) {
	enable();
	GLuint uni_loc = -1;
	uni_loc = glGetUniformLocation(prog_id, sName.c_str()); eglGetError();
	printf("Added uniform: %s at %d", sName.c_str(), uni_loc);
	uniforms[sName] = uni_loc;
	disable();
	return *this;
}

Shader& Shader::enableVertexAttribArray(std::string sName) {
	enable();
	GLuint attrib = getAttribute(sName);
	if(attrib != -1) {
		glEnableVertexAttribArray(attrib);eglGetError();
	}
	disable();
	return *this;
}

GLuint Shader::getAttribute(std::string sName) {
	ShaderMap::iterator it = attributes.find(sName);
	if(it == attributes.end()) {
		printf("Shader: error while retrieving the attribute: %s\n", sName.c_str());
		return -1;
	}
	return (it->second);
}

GLuint Shader::getUniform(std::string sName) {
	// find and return directly.
	ShaderMap::iterator it = uniforms.find(sName);
	if(it != uniforms.end()) {
		return it->second;
	}
	
	// if not found, try to load it.
	if(it == uniforms.end()) {
		addUniform(sName);
	}
	
	// and find again.... 
	it = uniforms.find(sName);
	if(it == uniforms.end()) {
		printf("Shader: error while retrieve the uniform: %s\n", sName.c_str());
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

std::string Shader::readFile(std::string sPath) {
	std::string result = "";
	std::string line = "";
	std::ifstream ifs(sPath.c_str());
	if(!ifs.is_open()) {
		return result;
	}
	while(getline(ifs,line)) {
		result += line +"\n";
	}
	return result;
}

// +++++++++++++++++++++++++++ DATA TRANSFER +++++++++++++++++++++++++++++++++++

// ----------------------- slower, but handier data transfers ------------------
Shader& Shader::uniform1i(std::string sName, GLint x) {
	glUniform1i(getUniform(sName), x); eglGetError();
	return *this;
}

Shader& Shader::uniform2i(std::string sName, GLint x, GLint y) {
	glUniform2i(getUniform(sName), x, y); eglGetError();
	return *this;
}

Shader& Shader::uniform3i(std::string sName, GLint x, GLint y, GLint z) {
	glUniform3i(getUniform(sName), x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4i(std::string sName, GLint x, GLint y, GLint z, GLint w) {
	glUniform4i(getUniform(sName), x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::uniform1f(std::string sName, GLfloat x) {
	glUniform1f(getUniform(sName), x); eglGetError();
	return *this;
}

Shader& Shader::uniform2f(std::string sName, GLfloat x, GLfloat y) {
	glUniform2f(getUniform(sName), x, y); eglGetError();
	return *this;
}


Shader& Shader::uniform3f(std::string sName, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(getUniform(sName), x, y, z); eglGetError();
	return *this;
}


Shader& Shader::uniform4f(std::string sName, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(getUniform(sName), x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::uniformMat4f(std::string sName, GLfloat* pMatrix, bool bTranspose) {
	glUniformMatrix4fv(getUniform(sName), 1, bTranspose, pMatrix); eglGetError();
	return *this;
/*

void glUniformMatrix4fv(	GLint  	location,
 	GLsizei  	count,
 	GLboolean  	transpose,
 	const GLfloat * 	value);
*/
}

Shader& Shader::setTextureUnit(
	 std::string sUniform
	,GLuint nTextureID
	,GLuint nNum
	,GLuint nTextureType
) 
{
	return setTextureUnit(getUniform(sUniform), nTextureID, nNum, nTextureType);
}	

// ----------------------- fast data transfers ---------------------------------
Shader& Shader::uniform1i(GLuint nPosition, GLint x) {
	glUniform1i(nPosition, x); eglGetError();
	return *this;
}

Shader& Shader::uniform2i(GLuint nPosition, GLint x, GLint y) {
	glUniform2i(nPosition, x, y); eglGetError();
	return *this;
}

Shader& Shader::uniform3i(GLuint nPosition, GLint x, GLint y, GLint z) {
	glUniform3i(nPosition, x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4i(GLuint nPosition, GLint x, GLint y, GLint z, GLint w) {
	glUniform4i(nPosition, x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::uniform1f(GLuint nPosition, GLfloat x) {
	glUniform1f(nPosition, x); eglGetError();
	return *this;
}

Shader& Shader::uniform2f(GLuint nPosition, GLfloat x, GLfloat y) {
	glUniform2f(nPosition, x, y); eglGetError();
	return *this;
}

Shader& Shader::uniform3f(GLuint nPosition, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(nPosition, x, y, z); eglGetError();
	return *this;
}

Shader& Shader::uniform4f(GLuint nPosition, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(nPosition, x, y, z, w); eglGetError();
	return *this;
}

Shader& Shader::setTextureUnit(
	 GLuint nUniformID
	,GLuint nTextureID
	,GLuint nNum
	,GLuint nTextureType) 
{
	glActiveTexture(GL_TEXTURE0 +nNum); eglGetError();
	glEnable(nTextureType); eglGetError();
	glBindTexture(nTextureType, nTextureID); eglGetError();
	uniform1i(nUniformID, nNum);

	glDisable(nTextureType);
	glActiveTexture(GL_TEXTURE0);
	return *this;
}