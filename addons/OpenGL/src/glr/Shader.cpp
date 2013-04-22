#include <glr/Shader.h>
#include <glr/Texture.h>
#include <roxlu/opengl/Error.h>

namespace gl {

  Shader::Shader()
    :prog(0)
    ,vert_id(0)
    ,frag_id(0)
    ,u_pm(-1)
    ,u_vm(-1)
    ,u_tex0(-1)
    ,u_tex1(-1)
    ,u_tex2(-1)
    ,u_tex3(-1)
  {

  }

  Shader::~Shader() {

    if(prog) {
      glDeleteProgram(prog);
      prog = 0;
    }

    u_pm = -1;
    u_vm = -1;
    u_tex0 = -1;
    u_tex1 = -1;
    u_tex2 = -1;
    u_tex3 = -1;
    vert_id = 0;
    frag_id = 0;
  }

  bool Shader::load(std::string vs, std::string fs, bool datapath) {

    std::string vss = rx_get_file_contents(vs, datapath);
    if(!vss.size()) {
      RX_ERROR(ERR_GL_SH_LOAD_FILE, vs.c_str());
      return false;
    }

    std::string fss = rx_get_file_contents(fs, datapath);
    if(!fss.size()) {
      RX_ERROR(ERR_GL_SH_LOAD_FILE, fs.c_str());
      return false;
    }

    return create(vss, fss);
  }

  bool Shader::create(std::string vs, std::string fs) {

    if(prog) {
      RX_ERROR(ERR_GL_SH_ALREADY_CREATED);
      return false;
    }

    vert_id = glCreateShader(GL_VERTEX_SHADER);
    frag_id = glCreateShader(GL_FRAGMENT_SHADER);

    if(!vert_id) {
      RX_ERROR(ERR_GL_SH_CANNOT_CREATE_VERT);
      glDeleteShader(vert_id);
      glDeleteShader(frag_id);
      return false;
    }

    if(!frag_id) {
      RX_ERROR(ERR_GL_SH_CANNOT_CREATE_FRAG);
      glDeleteShader(vert_id);
      glDeleteShader(frag_id);
      return false;
    }

    const char* vss = vs.c_str();
    const char* fss = fs.c_str();
    
    glShaderSource(vert_id, 1, &vss, NULL);
    glShaderSource(frag_id, 1, &fss, NULL);
    
    glCompileShader(vert_id); eglGetShaderInfoLog(vert_id);
    glCompileShader(frag_id); eglGetShaderInfoLog(frag_id);

    prog = glCreateProgram();

    if(!prog) {
      RX_ERROR(ERR_GL_SH_CANNOT_CREATE_PROG);
      glDeleteShader(vert_id);
      glDeleteShader(frag_id);
      glDeleteProgram(prog);
      return false;
    }

    glAttachShader(prog, vert_id);
    glAttachShader(prog, frag_id);
    

    return true;
  }

  void Shader::bindAttribLocation(std::string attrib, GLuint index) {
    attributes[attrib] = index;
  }
  
  GLint Shader::getUniformLocation(std::string uni) {
    return glGetUniformLocation(prog, uni.c_str());
  }

  bool Shader::link() {
    if(!vert_id || !frag_id) {
      RX_ERROR(ERR_GL_SH_NO_VERT_OR_FRAG);
      return false;
    }

    if(!prog) {
      RX_ERROR(ERR_GL_SH_PROG_NOT_CREATED);
      return false;
    }

    for(std::map<std::string, GLuint>::iterator it = attributes.begin(); it != attributes.end(); ++it) {
      glBindAttribLocation(prog, it->second, it->first.c_str());
    }
    
    glLinkProgram(prog); eglGetShaderLinkLog(prog);

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);

    use();

    u_pm = glGetUniformLocation(prog, "u_pm");
    u_vm = glGetUniformLocation(prog, "u_vm");
    u_mm = glGetUniformLocation(prog, "u_mm");
    u_tex0 = glGetUniformLocation(prog, "u_tex0");
    u_tex1 = glGetUniformLocation(prog, "u_tex1");
    u_tex2 = glGetUniformLocation(prog, "u_tex2");
    u_tex3 = glGetUniformLocation(prog, "u_tex3");

    unuse();

    return true;
  }


  void Shader::activeTexture(Texture& tex, GLenum unit) {
    assert(prog);

    if(unit == GL_TEXTURE0 && u_tex0 == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_TEX, 0);
      return;
    }
    else if(unit == GL_TEXTURE1 && u_tex1 == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_TEX, 1);
      return;
    }
    else if(unit == GL_TEXTURE2 && u_tex2 == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_TEX, 2);
      return;
    }
    else if(unit == GL_TEXTURE3 && u_tex3 == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_TEX, 3);
      return;
    }

    use();
    
    glActiveTexture(unit);
    tex.bind();

    if(unit == GL_TEXTURE0) {
      glUniform1i(u_tex0, 0);
    }
    else if(unit == GL_TEXTURE1) {
      glUniform1i(u_tex1, 1);
    }
    else if(unit == GL_TEXTURE2) {
      glUniform1i(u_tex2, 2);
    }
    else if(unit == GL_TEXTURE3) {
      glUniform1i(u_tex3, 3);
    }
  }
  
  void Shader::activeTexture(Texture& tex, GLenum unit, GLint uniform) {
    assert(prog);
      
    glActiveTexture(unit);
    tex.bind();

    switch(unit) {
      case GL_TEXTURE0: glUniform1i(uniform, 0); break;
      case GL_TEXTURE1: glUniform1i(uniform, 1); break;
      case GL_TEXTURE2: glUniform1i(uniform, 2); break;
      case GL_TEXTURE3: glUniform1i(uniform, 3); break;
      case GL_TEXTURE4: glUniform1i(uniform, 4); break;
      case GL_TEXTURE5: glUniform1i(uniform, 5); break;
      case GL_TEXTURE6: glUniform1i(uniform, 6); break;
      default: {
        RX_ERROR(ERR_GL_SH_UNSUPPORTED_TEXUNIT);
        break;
      }
    }
  }

  void Shader::print() {
    RX_VERBOSE("Shader.prog: %d", prog);
    RX_VERBOSE("Shader.u_pm: %d", u_pm);
    RX_VERBOSE("Shader.u_vm: %d", u_vm);
    RX_VERBOSE("Shader.u_mm: %d", u_mm);
    RX_VERBOSE("Shader.u_tex0: %d", u_tex0);
    RX_VERBOSE("Shader.u_tex1: %d", u_tex1);
    RX_VERBOSE("Shader.u_tex2: %d", u_tex2);
    RX_VERBOSE("Shader.u_tex3: %d", u_tex3);
  }
};
