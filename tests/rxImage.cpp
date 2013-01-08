#include "rxImage.h"

// we assume a PT shader.
rxImage::rxImage(Shader& shader) 
	:shader(shader)
	,buffer_created(false)
	,prev_w(0)
	,prev_h(0)
{


}

rxImage::~rxImage() {
}

bool rxImage::load(const std::string& filename) {
//	std::string filepath = File::toDataPath(filename);
	WebP wp;
	if(!wp.load(filename)) {
		return false;
	}
	setPixels(wp.getPixels(), wp.getWidth(), wp.getHeight(), GL_RGBA);
	return true;
}

void rxImage::setPixels(unsigned char* pixels, const unsigned int& w, const unsigned int& h, GLenum format) {
	tex.setPixels(pixels, w, h, format);
	width = w;
	height = h;
}

void rxImage::draw(const float& x, const float& y) {
	draw(x,y,width, height);
}


void rxImage::draw(const float& x, const float& y, const float& w, const float& h) {
	bool vao_bound = false;

	if(!buffer_created) {
		vao_bound = true;
		glGenVertexArraysAPPLE(1, &vao); eglGetError();
		glBindVertexArrayAPPLE(vao); eglGetError();
		glGenBuffers(1, &vbo); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();

		vector<VertexPT> buffer(6);
		buffer[0].set(0, 0, 0, 0, 1);
		buffer[1].set(w, 0, 0, 1, 1);
		buffer[2].set(w, h, 0, 1, 0);
		buffer[3].set(w, h, 0, 1, 0);
		buffer[4].set(0, h, 0, 0, 0);
		buffer[5].set(0, 0, 0, 0, 1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPT) * buffer.size(), buffer[0].getPtr(), GL_DYNAMIC_DRAW); eglGetError();
		glEnableVertexAttribArray(shader.getAttribute("pos")); eglGetError();
		glEnableVertexAttribArray(shader.getAttribute("tex")); eglGetError();
		glVertexAttribPointer(shader.getAttribute("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)offsetof(VertexPT, pos)); eglGetError();
		glVertexAttribPointer(shader.getAttribute("tex"), 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)offsetof(VertexPT, tex)); eglGetError();
		buffer_created = true;
		prev_w = w;
		prev_h = h;
	}
	else if( (prev_w != 0 && prev_w != w) || (prev_h != 0 && prev_h != h) ) {
		vector<VertexPT> buffer(6);
		buffer[0].set(0, 0, 0, 0, 1);
		buffer[1].set(w, 0, 0, 1, 1);
		buffer[2].set(w, h, 0, 1, 0);
		buffer[3].set(w, h, 0, 1, 0);
		buffer[4].set(0, h, 0, 0, 0);
		buffer[5].set(0, 0, 0, 0, 1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexPT) * buffer.size(), buffer[0].getPtr()); eglGetError();
		prev_w = w;
		prev_h = h;	
	}
	
	if(!vao_bound) {
		glBindVertexArrayAPPLE(vao); eglGetError();
	}	

	shader.enable();
	glEnable(GL_TEXTURE_2D); eglGetError();
	glActiveTexture(GL_TEXTURE0); eglGetError();
	glBindTexture(GL_TEXTURE_2D, tex.getID()); eglGetError();
	shader.uniform1i("texture", 0);
	
	model_matrix.setPosition(x, y,0);
	shader.uniformMat4fv("modelview_matrix", model_matrix.getPtr());
	glDrawArrays(GL_TRIANGLES, 0, 6); eglGetError();
	
	shader.disable();
	glDisable(GL_TEXTURE_2D);
	glBindVertexArrayAPPLE(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}


