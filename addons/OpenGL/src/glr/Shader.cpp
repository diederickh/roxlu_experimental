#include <glr/Shader.h>

namespace gl {

  Shader::Shader()
    :prog(0)
    ,vertex_type(VERTEX_NONE)
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

    vertex_type = VERTEX_NONE;
    u_pm = -1;
    u_vm = -1;
    u_tex0 = -1;
    u_tex1 = -1;
    u_tex2 = -1;
    u_tex3 = -1;
  }

  bool Shader::load(std::string vs, std::string fs, unsigned int vertexType, bool datapath) {

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

    return create(vss, fss, vertexType);
  }

  bool Shader::create(std::string vs, std::string fs, unsigned int vertexType) {

    if(prog) {
      RX_ERROR(ERR_GL_SH_ALREADY_CREATED);
      return false;
    }

    this->vertex_type = vertexType;

    GLuint vert_id = 0;
    GLuint frag_id = 0;

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
    
    if(vertex_type == VERTEX_P) {
      glBindAttribLocation(prog, 0, "a_pos");
    }
    else if(vertex_type == VERTEX_PT) {
      glBindAttribLocation(prog, 0, "a_pos");
      glBindAttribLocation(prog, 1, "a_tex");
    }
    else {
      RX_WARNING(WARN_GL_SH_UNKNOWN_TYPE);
    }
    
    glLinkProgram(prog);

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

  void Shader::print() {
    RX_VERBOSE("Shader.prog: %d", prog);
    RX_VERBOSE("Shader.u_pm: %d", u_pm);
    RX_VERBOSE("Shader.u_vm: %d", u_vm);
    RX_VERBOSE("Shader.u_mm: %d", u_mm);
    RX_VERBOSE("Shader.u_tex0: %d", u_tex0);
    RX_VERBOSE("Shader.u_tex1: %d", u_tex1)
    RX_VERBOSE("Shader.u_tex2: %d", u_tex2);
    RX_VERBOSE("Shader.u_tex3: %d", u_tex3);
  }
};
