#ifndef ROXLU_BILLBOARDH
#define ROXLU_BILLBOARDH

#include "Roxlu.h"

const std::string BILLBOARD_VS = " \
	uniform mat4 u_projection_matrix; \
	uniform mat4 u_view_matrix; \
	uniform mat4 u_model_matrix; \
	attribute vec2 a_tex; \
	attribute vec4 a_pos; \
	varying vec2 v_tex; \
	\
	void main() { \
		v_tex = a_tex; \
		gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * a_pos; \
	}";
	
const std::string BILLBOARD_FS = " \
	uniform float u_alpha; \
	uniform sampler2D u_texture; \
	varying vec2 v_tex; \
	void main() { \
		vec4 texcol = texture2D(u_texture, v_tex); \
		gl_FragColor = texcol;\
		gl_FragColor.a = texcol.a * u_alpha; \
	}";

class Billboard {
public:	
	Billboard();
	void setTexture(const GLuint& id);
	void start(const Mat4& pm, const Mat4& vm, const Vec3& right, const Vec3& up);
	void draw(const Vec3& position, const float& scale, const float& rotationDegrees, float alpha = 1.0);
	void stop(); // or disable() ?
private:
	GLuint tex;
	const Vec3* right;
	const Vec3* up;
	const Texture* texture;
	static bool created;
	static Shader shader;
	static GLuint vbo;
	static GLuint vao;
};

#endif