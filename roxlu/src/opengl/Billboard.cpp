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
		
		shader.create(BILLBOARD_VS, BILLBOARD_FS);
		shader.a("a_pos", 0).a("a_tex", 1);
		shader.link();
		shader.u("u_projection_matrix").u("u_view_matrix").u("u_model_matrix").u("u_texture").u("u_alpha");
		
		
		glGenVertexArraysAPPLE(1, &vao); eglGetError();
		glBindVertexArrayAPPLE(vao); eglGetError();
		glGenBuffers(1, &vbo); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();		
		
		BillboardVertex vertices[6];
		float s = 0.5f;
		vertices[0].set(-s, -s, 0.0f, 0.0f);
		vertices[1].set(s, -s, 1.0f, 0.0f);
		vertices[2].set(s, s, 1.0f, 1.0f);
		
		vertices[3].set(-s, -s, 0.0f, 0.0f);
		vertices[4].set(s, s, 1.0f, 1.0f);
		vertices[5].set(-s, s, 0.0f, 1.0f);

		
		glBufferData(GL_ARRAY_BUFFER, sizeof(BillboardVertex) * 6, &(vertices[0].pos[0]), GL_STATIC_DRAW);
	
		
		glEnableVertexAttribArray(0); eglGetError();
		glEnableVertexAttribArray(1); eglGetError();
	//	printf("%zu, %zu, %zu, %zu\n", (GLvoid*)offsetof(BillboardVertex, pos), (GLvoid*)offsetof(BillboardVertex, tex), 0, sizeof(float) * 3);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BillboardVertex), (GLvoid*)offsetof(BillboardVertex, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BillboardVertex), (GLvoid*)offsetof(BillboardVertex, tex));

		glBindVertexArrayAPPLE(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		shader.disable();
		created = true;
	}
}

void Billboard::start(const Mat4& pm, const Mat4& vm, const Vec3& right, const Vec3& up) {
	this->right = &right;
	this->up = &up;
	this->pm = pm.getPtr();
	this->vm = vm.getPtr();
	
	shader.enable();
	shader.uniformMat4fv("u_projection_matrix", pm.getPtr());
	shader.uniformMat4fv("u_view_matrix", vm.getPtr());
	shader.uniform1i("u_texture",2);

	glActiveTexture(GL_TEXTURE2); eglGetError();
	glBindTexture(GL_TEXTURE_2D, tex); eglGetError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Billboard::setTexture(const GLuint& id) {
	tex = id;
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
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Billboard::stop() {
	glBindVertexArrayAPPLE(0);
	shader.disable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
