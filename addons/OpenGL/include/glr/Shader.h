#ifndef ROXLU_OPENGL_SHADER_H
#define ROXLU_OPENGL_SHADER_H

#include <roxlu/core/Log.h>
#include <roxlu/opengl/GL.h>
#include <glr/Vertex.h>
#include <glr/Texture.h>
#include <string>

#define ERR_GL_SH_LOAD_FILE "Cannot load the shader file: `%s`"
#define ERR_GL_SH_ALREADY_CREATED "The shader is already created. Are you sure you want to create it again?"
#define ERR_GL_SH_NOT_CREATED "The shader was not yet created!"
#define ERR_GL_SH_CANNOT_CREATE_VERT "Cannot create the vertex shader. Are you sure you have a correct GL context setup?"
#define ERR_GL_SH_CANNOT_CREATE_FRAG "Cannot create the fragment shader. Are you sure you have a correct GL context setup?"
#define ERR_GL_SH_CANNOT_CREATE_PROG "Cannot create the program. Are you sure you have a correct GL context setup?"
#define ERR_GL_SH_NO_UNI_PM "Cannot find the projection matrix. Make sure the shader has a `mat4 u_pm`"
#define ERR_GL_SH_NO_UNI_VM "Cannot find the view matrix. Make sure the shader has a `mat4 u_vm`"
#define ERR_GL_SH_NO_UNI_MM "Cannot find the model matrix. Make sure the shader has a `mat4 u_mm`"
#define ERR_GL_SH_NO_UNI_TEX "Cannot find the texture sampler for texture unit: %d, make sure to call the samples `u_tex%d` in your shader"
#define WARN_GL_SH_UNKNOWN_TYPE "Unsupported VertexType, we cannot automatically setup attribute locations"

namespace gl {

  class Shader {
  public:

    Shader();
    ~Shader();

    bool load(std::string vs, std::string fs, unsigned int vertexType, bool datapath = false);    /* load a vs (vertex shader) and fs (fragment shader) from the given filepaths (vs/fs) */
    bool create(std::string vs, std::string fs, unsigned int vertexType);                         /* create the shader based on the given source strings and bind attribute locations for the the given type */
    void use();                                                                                   /* make this shader active */
    void unuse();                                                                                 /* deactivate all shaders; in a correct GL application, you shouldn't need to call this; */

    void setProjectionMatrix(const float* pm);
    void setViewMatrix(const float* pm);
    void setModelMatrix(const float* mm);

    void activateTexture(Texture& tex, GLenum unit);                                              /* activates the texture unit and test the correct uniform value */
    void print();

  public:
    GLuint prog;
    unsigned int vertex_type;

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

  inline void Shader::activateTexture(Texture& tex, GLenum unit) {
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

} // gl

#endif
