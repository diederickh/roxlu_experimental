#include "Billboard.h"

Shader Billboard::shader = Shader();
bool Billboard::created = false;
GLuint Billboard::vbo = 0;
GLuint Billboard::vao = 0;

Billboard::Billboard()
	:texture(NULL)
{
	if(!created) {
		printf("Billboard needs to be created\n");
		
		glGenVertexArraysAPPLE(1, &vao); eglGetError();
		glBindVertexArrayAPPLE(vao); eglGetError();
		glGenBuffers(1, &vbo); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();		
		
		VerticesPT verts;
		verts.add(VertexPT(-1, 1, 0, 0,0));
		verts.add(VertexPT( 1, 1, 0, 1,0));
		verts.add(VertexPT( 1,-1, 0, 1,1));
		verts.add(VertexPT(-1,-1, 0, 0,1));
		
		glBufferData(GL_ARRAY_BUFFER, verts.numBytes(), verts.getPtr(), GL_STATIC_DRAW); eglGetError();
		
		shader.create(BILLBOARD_VS, BILLBOARD_FS);
		shader.a("a_pos", 0).a("a_tex", 1);
		shader.link();
		shader.u("u_projection_matrix").u("u_view_matrix").u("u_model_matrix").u("u_texture").u("u_alpha");
		
		glEnableVertexAttribArray(0); eglGetError();
		glEnableVertexAttribArray(1); eglGetError();
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)offsetof(VertexPT, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)offsetof(VertexPT, tex));
		
		// and disable.
		glBindVertexArrayAPPLE(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shader.disable();
	}
}

void Billboard::setup(const Mat4& pm, const Mat4& vm, const Vec3& right, const Vec3& up) {
	this->right = &right;
	this->up = &up;
	
	shader.enable();
	shader.uniformMat4fv("u_projection_matrix", pm.getPtr());
	shader.uniformMat4fv("u_view_matrix", vm.getPtr());
}

void Billboard::setTexture(const GLuint& id) {
	shader.enable();
	shader.uniform1i("u_texture",2);
	glActiveTexture(GL_TEXTURE2); eglGetError();
	glBindTexture(GL_TEXTURE_2D, id); eglGetError();
}


void Billboard::draw(const Vec3& position, const float& scale, const float& rotationDegrees, float alpha) {	
	Mat4 mm;
	mm.setBillboard(*this->right, *this->up);
	mm.setPosition(position);
	mm.scale(scale);
	
	Quat q;
	q.rotate(rotationDegrees * DEG_TO_RAD,0,0,1);
	mm = mm * q.getMat4();
	
	shader.uniform1f("u_alpha", alpha);
	shader.uniformMat4fv("u_model_matrix", mm.getPtr());
	
	glBindVertexArrayAPPLE(vao); eglGetError();
	glDrawArrays(GL_QUADS, 0, 4);
}

void Billboard::unbind() {
	glBindVertexArrayAPPLE(0);
	shader.disable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
