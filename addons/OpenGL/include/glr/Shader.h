#ifndef ROXLU_OPENGL_SHADER_H
#define ROXLU_OPENGL_SHADER_H

#include <roxlu/core/Log.h>
#include <roxlu/opengl/GL.h>
#include <glr/Vertex.h>

#include <map>
#include <string>

#define ERR_GL_SH_LOAD_FILE "Cannot load the shader file: `%s`"
#define ERR_GL_SH_ALREADY_CREATED "The shader is already created. Are you sure you want to create it again?"
#define ERR_GL_SH_NOT_CREATED "The shader was not yet created!"
#define ERR_GL_SH_PROG_NOT_CREATED "The program object was not created; did you call `load()` or `create()` "
#define ERR_GL_SH_CANNOT_CREATE_VERT "Cannot create the vertex shader. Are you sure you have a correct GL context setup?"
#define ERR_GL_SH_CANNOT_CREATE_FRAG "Cannot create the fragment shader. Are you sure you have a correct GL context setup?"
#define ERR_GL_SH_CANNOT_CREATE_PROG "Cannot create the program. Are you sure you have a correct GL context setup?"
#define ERR_GL_SH_NO_UNI_PM "Cannot find the projection matrix. Make sure the shader has a `mat4 u_pm`"
#define ERR_GL_SH_NO_UNI_VM "Cannot find the view matrix. Make sure the shader has a `mat4 u_vm`"
#define ERR_GL_SH_NO_UNI_MM "Cannot find the model matrix. Make sure the shader has a `mat4 u_mm`"
#define ERR_GL_SH_NO_UNI_TEX "Cannot find the texture sampler for texture unit: %d, make sure to call the samples `u_tex%d` in your shader"
#define ERR_GL_SH_NO_VERT_OR_FRAG "Cannot find a correct vertex or fragment shader; did you call `load()` or `create()` "
#define ERR_GL_SH_UNSUPPORTED_TEXUNIT "The given texture unit id is not supported"

namespace gl {

  class Texture;

  class Shader {
  public:

    Shader();
    ~Shader();

    bool load(std::string vs, std::string fs, bool datapath = false);                             /* load a vs (vertex shader) and fs (fragment shader) from the given filepaths (vs/fs) */
    bool create(std::string vs, std::string fs);                                                  /* create the shader based on the given source strings */
    bool link();                                                                                  /* must be called after `load()` or `create()` */
    void use();                                                                                   /* make this shader active */
    void unuse();                                                                                 /* deactivate all shaders; in a correct GL application, you shouldn't need to call this; */

    void bindAttribLocation(std::string attrib, GLuint index);                                    /* bind attrib location, must be called before you call 'link' */
    GLint getUniformLocation(std::string uni);                                                    /* get the uniform location for the given uniform name, make sure that you have called `use()` before calling this */

    void setProjectionMatrix(const float* pm);
    void setViewMatrix(const float* pm);
    void setModelMatrix(const float* mm);

    void activeTexture(Texture& tex, GLenum unit);                                                /* you must follow the standards with this: activates the texture unit and test the correct uniform value; */
    void activeTexture(Texture& tex, GLenum unit, GLint uniform);                                 /* custom textures: activates the texture unit and for the given uniform id  */

    void print();

  public:
    std::map<std::string, GLuint> attributes;
    GLuint prog;
    GLuint vert_id;
    GLuint frag_id;

    GLint u_pm;
    GLint u_vm;
    GLint u_mm;

    GLint u_tex0;
    GLint u_tex1;
    GLint u_tex2;
    GLint u_tex3;
  };

  inline void Shader::use() {
    if(!prog) {
      RX_ERROR(ERR_GL_SH_NOT_CREATED);
      return;
    }
    
    glUseProgram(prog);
  }

  inline void Shader::unuse() {
    if(!prog) {
      RX_ERROR(ERR_GL_SH_NOT_CREATED);
      return;
    }

    glUseProgram(0);

  }
  
  inline void Shader::setProjectionMatrix(const float* pm) {
    if(u_pm == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_PM);
      return;
    }

    use();
    glUniformMatrix4fv(u_pm, 1, GL_FALSE, pm);
  }

  inline void Shader::setViewMatrix(const float* vm) {
    if(u_vm == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_VM);
      return;
    }

    use();
    glUniformMatrix4fv(u_vm, 1, GL_FALSE, vm);
  }

  inline void Shader::setModelMatrix(const float* mm) {
    if(u_mm == -1) {
      RX_ERROR(ERR_GL_SH_NO_UNI_MM);
      return;
    }

    use();

    glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm);
  }

} // gl

#endif
