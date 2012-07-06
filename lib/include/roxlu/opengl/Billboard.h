#ifndef ROXLU_BILLBOARDH
#define ROXLU_BILLBOARDH

//#include "Roxlu.h"

#include <roxlu/math/Mat4.h>
#include <roxlu/math/Vec3.h>
#include <roxlu/math/Quat.h>
#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/opengl/Shader.h>
#include <roxlu/opengl/Texture.h>
#include <roxlu/opengl/VAO.h>

const std::string BILLBOARD_VS = " \
	uniform mat4 u_projection_matrix; \
	uniform mat4 u_view_matrix; \
	uniform mat4 u_model_matrix; \
	attribute vec3 a_pos; \
	attribute vec2 a_tex; \
	varying vec2 v_tex; \
	\
	void main() { \
		v_tex = a_tex; \
		gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(a_pos, 1.0); \
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

namespace roxlu {

struct BillboardVertex {
	float pos[3];
	float tex[2];
	
	void set(const float& x, const float& y, const float& s, const float& t) {
		pos[0] = x;
		pos[1] = y;
		pos[2] = 0.0;
		tex[0] = s;
		tex[1] = t;
	}
};

enum BillboardDrawModes {
	 BILLBOARD_NONE // this billboard class is also used to draw simple quadds
	,BILLBOARD_PERSPECTIVE
};

class Billboard {
public:	
	Billboard();

	void setTexture(const GLuint& id);
	void bind(const float* pm, const float* vm, const Vec3& right, const Vec3& up);
	void bind(const float* pm, const float* vm);
	void draw(const Vec3& position, const float scale, const float rotationDegrees, float alpha = 1.0);
	void unbind(); 

private:
	void initBillboard();
	
	int mode;
	bool texture_set;
	GLuint tex;
	const Vec3* right;
	const Vec3* up;
	const float* pm;
	const float* vm;
	const Texture* texture;
	static bool created;
	static Shader shader;
	static GLuint vbo;
	static VAO vao;
};

} // roxlu

#endif