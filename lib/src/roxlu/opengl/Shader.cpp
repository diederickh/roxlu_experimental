#ifdef ROXLU_GL_WRAPPER

#include <roxlu/opengl/Shader.h>

namespace roxlu {

  Shader::Shader() 
    :name("")
    ,vert_id(0)
    ,frag_id(0)
    ,prog_id(0)
    ,enabled(false)
    ,verbose(false)
  {
  }

  Shader::Shader(const std::string& sName) 
    :name(sName)
  {
  }

  Shader& Shader::setName(const std::string& sName) {
    name = sName;
    return *this;
  }

  bool Shader::load(const std::string& sName) {
    name = sName;
    return load();
  }

  // load and create a shader
  bool Shader::load() {
    std::string vs_src = getVertexSource();
    std::string fs_src = getFragmentSource();
    return create(vs_src, fs_src);
  }

  string Shader::getVertexSource(bool inDataPath) {
    std::string vs_file = name +".vert";
    return File::getFileContents(vs_file, inDataPath);
  }

  string Shader::getFragmentSource(bool inDataPath) {
    std::string vs_file = name +".frag";
    return File::getFileContents(vs_file, inDataPath);
  }

  bool Shader::create(const std::string& vertexSource, const std::string& fragmentSource) {
    if(!vertexSource.length() || !fragmentSource.length()) {
      printf("No shader source given.\n");
      return false;
    }
	
    vert_shader_source = vertexSource;
    frag_shader_source = fragmentSource;
	
    const char* vss_ptr = vertexSource.c_str();
    const char* fss_ptr = fragmentSource.c_str();
	
    //std::cout << vertexSource << std::endl;
    //std::cout << fragmentSource << std::endl;
	
    // create shader.
    vert_id = glCreateShader(GL_VERTEX_SHADER); // eglGetError();
    frag_id = glCreateShader(GL_FRAGMENT_SHADER); // eglGetError();

    // set shader source.
    glShaderSource(vert_id, 1, &vss_ptr, NULL); // eglGetError();
    glShaderSource(frag_id, 1, &fss_ptr, NULL); // eglGetError();
	
    // compile
    glCompileShader(vert_id); eglGetShaderInfoLog(vert_id);
    glCompileShader(frag_id); eglGetShaderInfoLog(frag_id);
	
    // create program, attach and link.
    prog_id = glCreateProgram(); eglGetError();
    glAttachShader(prog_id, vert_id); eglGetError();
    glAttachShader(prog_id, frag_id); eglGetError();
    return true;
  }

  void Shader::link() {
    glLinkProgram(prog_id); eglGetError();
    glUseProgram(prog_id); eglGetError();
    disable();
  }

  Shader& Shader::a(const std::string& attribute) {
    return addAttribute(attribute);
  }

  // add an attribue, enable vertex attrib array and set index
  Shader& Shader::a(const std::string& name, GLuint index) {
    return addAttribute(name, index);
  }

  Shader& Shader::addAttribute(const std::string& name, GLuint index) {
    // check if it's already added.
    ShaderMap::iterator it = attributes.find(name);
    if(it != attributes.end()) {
      printf("Error: Attribute not found: %s\n", name.c_str());
      return *this;
    }

    // set the attribute location and enable the vertex attrib array.
    glBindAttribLocation(prog_id, index, name.c_str());

    attributes[name] = index;
    return *this;
  }

  Shader& Shader::addAttribute(const std::string& attribute) {
    ShaderMap::iterator it = attributes.find(attribute);
    if(it != attributes.end()) {
      return *this;
    }
	
    GLint attribute_id = glGetAttribLocation(prog_id, attribute.c_str()); eglGetError();
    VERBOSE_MSG("Added attribute: %s (%d) for prog: %d\n", attribute.c_str(), attribute_id, prog_id);
    attributes[attribute] = attribute_id;
    return *this;
  };

  Shader& Shader::u(const std::string& uniform) {
    return addUniform(uniform);
  }

  Shader& Shader::addUniform(const std::string& uniform) {
    GLint uni_loc = -1;
    uni_loc = glGetUniformLocation(prog_id, uniform.c_str()); eglGetError();
    VERBOSE_MSG("Added uniform: %s at %d for prog: %d\n", uniform.c_str(), uni_loc, prog_id);
    uniforms[uniform] = uni_loc;
    return *this;
  }

  Shader& Shader::enableVertexAttribArray(const std::string& attribute) {
    GLint attrib = getAttribute(attribute);
    if(attrib != -1) {
      glEnableVertexAttribArray(attrib);eglGetError();
    }
    return *this;
  }

  GLint Shader::getAttribute(const std::string& attribute) {
    ShaderMap::iterator it = attributes.find(attribute);
    if(it == attributes.end()) {
      printf("Shader: error while retrieving the attribute: %s\n", attribute.c_str());
      return -1;
    }
    return (it->second);
  }

  GLint Shader::getUniform(const std::string& uniform) {
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

  void Shader::enable() {
    glUseProgram(prog_id); eglGetError();
  }

  void Shader::disable() {
    glUseProgram(0); eglGetError();
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
    if(vert_id != 0) {
      glDeleteShader(vert_id); eglGetError();
      glDeleteShader(frag_id); eglGetError();
      glDeleteProgram(prog_id); eglGetError();
    }
    create(vertShader, fragShader);
    VERBOSE_MSG("recreated shader: %d, %d\n", vert_id, frag_id);
	
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
    //printf("> enabled: %d for: %s with id: %d\n", enabled, uniform.c_str(), getUniform(uniform));
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


  Shader& Shader::uniformMat2fv(std::string uniform, const GLfloat* matrix, bool transpose) {
    glUniformMatrix2fv(getUniform(uniform), 1, transpose, matrix); eglGetError();
    return *this;
  }


  Shader& Shader::uniformMat3fv(std::string uniform, const GLfloat* matrix, bool transpose) {
    glUniformMatrix3fv(getUniform(uniform), 1, transpose, matrix); eglGetError();
    return *this;
  }


  Shader& Shader::uniformMat4fv(std::string uniform, const GLfloat* matrix, bool transpose) {
    //printf("> enabled: %d for: %s with id: %d\n", enabled, uniform.c_str(), getUniform(uniform));
    glUniformMatrix4fv(getUniform(uniform), 1, transpose, matrix); eglGetError();
    return *this;
  }

  Shader& Shader::uniform1fv(std::string uniform, const GLfloat* value, int count) {
    glUniform1fv(getUniform(uniform), count, value); eglGetError();
    return *this;
  }

  Shader& Shader::uniform2fv(std::string uniform, const GLfloat* value, int count) {
    glUniform2fv(getUniform(uniform), count, value); eglGetError();
    return *this;
  }

  Shader& Shader::uniform3fv(std::string uniform, const GLfloat* value, int count) {
    glUniform3fv(getUniform(uniform), count, value); eglGetError();
    return *this;
  }

  Shader& Shader::uniform4fv(std::string uniform, const GLfloat* value, int count) {
    glUniform4fv(getUniform(uniform), count, value); eglGetError();
    return *this;
  }

  Shader& Shader::setTextureUnit(
                                 std::string uniform
                                 ,GLuint textureID
                                 ,int num
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

  Shader& Shader::uniform1fv(GLint position, const GLfloat* value, int count) {
    glUniform1fv(position, count, value); eglGetError();
    return *this;
  }

  Shader& Shader::uniform2fv(GLint position, const GLfloat* value, int count) {
    glUniform2fv(position, count, value); eglGetError();
    return *this;
  }

  Shader& Shader::uniform3fv(GLint position, const GLfloat* value, int count) {
    glUniform3fv(position, count, value); eglGetError();
    return *this;
  }

  Shader& Shader::uniform4fv(GLint position, const GLfloat* value, int count) {
    glUniform4fv(position, count, value); eglGetError();
    return *this;
  }

  Shader& Shader::setTextureUnit(
                                 GLuint nUniformID
                                 ,GLuint nTextureID
                                 ,int num
                                 ,GLuint nTextureType) 
  {
    enable();
    glEnable(nTextureType); eglGetError();
	
    glActiveTexture(GL_TEXTURE0+num); eglGetError();
    glBindTexture(nTextureType, nTextureID); eglGetError();
    uniform1i(nUniformID, num); eglGetError();

    //glDisable(nTextureType); eglGetError();
    //glActiveTexture(GL_TEXTURE0); eglGetError();
    return *this;
  }

} // roxlu

#endif // ROXLU_GL_WRAPPER
