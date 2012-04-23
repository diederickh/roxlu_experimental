#ifndef ROXLU_MESHH
#define ROXLU_MESHH

#include "OpenGL.h"
#include "VertexTypes.h"
#include "VAO.h"
#include "VBO.h"
#include "Shader.h"
#include "Error.h"
#include <vector>

namespace roxlu {

struct MeshEntry {
	MeshEntry(int start, int count, GLenum mode) 
		:start(start)
		,count(count)
		,mode(mode)
	{
	}
	
	int start;
	int count;
	GLenum mode;
};

class MeshSetup {
public:
	MeshSetup(Shader& shader):shader(shader){}
	~MeshSetup() {}
	virtual void setupShader() = 0;
	virtual void setVertexAttributes() = 0;
	Shader& shader;
};

class MeshSetupPT : public MeshSetup { 
public:	
	MeshSetupPT(Shader& shader):MeshSetup(shader){}
	~MeshSetupPT() {}
	
	void setupShader() {
		shader.addUniform("modelview_matrix").addUniform("projection_matrix").addUniform("texture");
		shader.addAttribute("tex").addAttribute("pos");
	}
	
	void setVertexAttributes() {
		glEnableVertexAttribArray(shader.getAttribute("pos")); eglGetError();
		glEnableVertexAttribArray(shader.getAttribute("tex")); eglGetError();
		glVertexAttribPointer(shader.getAttribute("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*) offsetof(VertexPT, pos)); eglGetError();
		glVertexAttribPointer(shader.getAttribute("tex"), 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*) offsetof(VertexPT, tex)); eglGetError();
	}
};

class MeshSetupP : public MeshSetup {
public:
	MeshSetupP(Shader& shader):MeshSetup(shader){}
	~MeshSetupP(){}
	
	void setupShader() {
		shader.addUniform("modelview_matrix").addUniform("projection_matrix");
		shader.addAttribute("pos");
	}
	
	void setVertexAttributes() {
		glEnableVertexAttribArray(shader.getAttribute("pos")); eglGetError();
		glVertexAttribPointer(shader.getAttribute("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), (GLvoid*)offsetof(VertexP, pos)); eglGetError();
	}
	
};

// T = vertices buffer
// S = mesh setup
template<class T, class S>
class Mesh {
public:
	Mesh(Shader& shader, size_t bufferPageSize = 4096)
		:buffer_size(0)
		,buffer_page_size(bufferPageSize)
		,needs_resize(true)
		,begin_called(false)
		,shader(shader)
		,mesh_setup(shader)
	{
	}
	
	~Mesh() {
	}

	void setup() {
		shader.enable();
		
		mesh_setup.setupShader();
		//setupShader();
		
		shader.enable();		
		glGenVertexArraysAPPLE(1, &vao); eglGetError();
		glBindVertexArrayAPPLE(vao); eglGetError();
		
		glGenBuffers(1, &vbo); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
		
		mesh_setup.setVertexAttributes();
		//setVertexAttributes();
		
		unbind();
	}
	
	void setCamera(Mat4& vm, Mat4& pm) {
		shader.uniformMat4fv("projection_matrix", vm.getPtr());
		shader.uniformMat4fv("modelview_matrix", pm.getPtr());
	}
	
	void bind() {
		shader.enable();
		glBindVertexArrayAPPLE(vao); eglGetError();
	}
	void unbind() {
		shader.disable();
		glBindVertexArrayAPPLE(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	void draw(const Mat4& vm, const Mat4& pm) {
		bind();
		setCamera(vm, pm);
		std::vector<MeshEntry>::iterator it = entries.begin();
		while(it != entries.end()) {
			MeshEntry& me = (*it);
			glDrawArrays(me.mode, me.start, me.count);
			++it;
		}
		unbind();
	}
	
	void drawEntry(const unsigned int dx) {
		MeshEntry& me = entries.at(dx);
		glDrawArrays(me.mode, me.start, me.count);
//		for(int i = me.start; i < (me.start+me.count); ++i) {
//			typename T::element_type& el = buffer[i];
//			el.tex.print();
//		}
//		printf("--\n");
	}
	
	void beginShape(GLenum drawMode) {
		if(begin_called) {
			printf("Already called VertexBuffer<T>::begin!\n");
			exit(0);
		}
		begin_dx = buffer.size();
		begin_called = true;
		begin_mode = drawMode;
	}
	
	void debugDraw(const Mat4& vm, const Mat4& pm) {
		unbind();
		glMatrixMode(GL_PROJECTION); eglGetError();
		glLoadMatrixf(pm.getPtr()); eglGetError();
		
		glMatrixMode(GL_MODELVIEW); eglGetError();
		glLoadMatrixf(vm.getPtr()); eglGetError();

		std::vector<MeshEntry>::iterator it = entries.begin();
		while(it != entries.end()) {
			MeshEntry& me = *it;
			glBegin(me.mode);
			for(int i = me.start; i < (me.start+me.count); ++i) {
				glVertex3fv(buffer[i].pos.getPtr());
			}
			glEnd();
			++it;
		}
		
	}
	
	void debugDrawEntry(const unsigned int dx, const Mat4& vm, const Mat4& pm) {
		debugDrawEntry(entries.at(dx), vm, pm);
	}

	void debugDrawEntry(const MeshEntry& me, const Mat4& vm, const Mat4& pm) {
		unbind();
		
		glMatrixMode(GL_PROJECTION); eglGetError();
		glLoadMatrixf(pm.getPtr()); eglGetError();
		
		glMatrixMode(GL_MODELVIEW); eglGetError();
		glLoadMatrixf(vm.getPtr()); eglGetError();

		glBegin(me.mode);
		for(int i = me.start; i < (me.start+me.count); ++i) {
			glVertex3fv(buffer[i].pos.getPtr());
		}
		glEnd();
	}
	
	
	// returns entries index
	int endShape() {
		begin_called = false;
		MeshEntry entry(begin_dx, (buffer.size() - begin_dx), begin_mode);
		entries.push_back(entry);
		return entries.size() - 1;
	}
	
	/**
	 * Returns:
	 * 0 = we recreated the complete buffer
	 * 1 = we only updated the data
	 */
	int update() {
		if(needsResize()) {
			resize();
			glBindVertexArrayAPPLE(vao); eglGetError();
			glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
			glBufferData(GL_ARRAY_BUFFER, buffer.numBytes(), buffer.getPtr(), GL_DYNAMIC_DRAW); eglGetError();
			
			shader.enable();
				mesh_setup.setVertexAttributes();
				//setVertexAttributes();
			shader.disable();
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArrayAPPLE(0);			
			return 0;
		}
		else {
			glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
			glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.numBytes(), buffer.getPtr());
			unbind();
			return 1;
		}
	}
	
	// returns buffer index
	int add(const typename T::element_type& el) {
		return buffer.add(el);
	}
	
	// returns entries index
	/*
	int addRectangle(const float& x, const float& y, const float& w, const float& h) {
		int start = buffer.size();
		int num = buffer.addRectangle(x,y,w,h);
		MeshEntry entry(start, num, GL_TRIANGLES);
		entries.push_back(entry);
		checkResize();
		return entries.size()-1;
	}
	*/
	
	bool needsResize() {
		return needs_resize;
	}
	
	void resize() {
		buffer_size += buffer_page_size;
		needs_resize = false;
	}
	
	void clear() {
		buffer.clear();
		entries.clear();
	}
	
	size_t numBytes() {	
		return buffer.numBytes();
	}	
	
	float* getPtr() {
		return buffer.getPtr();
	}
	
	size_t getBufferSize() {
		return buffer_size;
	}
	
	const MeshEntry& operator[](const unsigned int dx) {
		return entries.at(dx);
	}
	
	std::vector<MeshEntry>::iterator begin() {
		return entries.begin();
	}
	std::vector<MeshEntry>::iterator end() {
		return entries.end();
	}
//	
//	void setupShader()  {
//		printf("<< setupshader\n");
//	}
//	void setVertexAttributes() {
//		printf("<< setVertexattributes()\n");
//	}
	
	Shader& shader;
	S mesh_setup;
	T buffer;
	GLuint vbo;
	GLuint vao;
	
protected:
	//S mesh_setup;
	
	void checkResize() {
		if(buffer.numBytes() > buffer_size) {
			needs_resize = true;
		}
	}
	
	GLenum begin_mode;
	bool begin_called;
	int begin_dx;
	bool needs_resize;
	size_t buffer_size;
	size_t buffer_page_size;
	
	std::vector<MeshEntry> entries;
};

template<class T, class S>
class MeshPT_Implementation : public Mesh<T, S> {
public:
	MeshPT_Implementation(Shader& shader, size_t bufferPageSize = 4096)
		:Mesh<T, S>(shader, bufferPageSize)
	{
	}
	
	int addRectangle(const float& x, const float& y, const float& w, const float& h, bool flipUV = false) {
		int start = this->buffer.size();
		int num = this->buffer.addRectangle(x,y,w,h);
		MeshEntry entry(start, num, GL_TRIANGLES);
		this->entries.push_back(entry);
		this->checkResize();
		return this->entries.size()-1;
	}
	
//	void setupShader() {
//		printf("SetupShader!!!!\n");
//		this->shader.addUniform("modelview_matrix").addUniform("projection_matrix").addUniform("texture");
//		this->shader.addAttribute("tex").addAttribute("pos");
//	}
//	
//	void setVertexAttributes() {
//		printf("SetVertexAttributes!!!!\n");
//		glEnableVertexAttribArray(this->shader.getAttribute("pos")); eglGetError();
//		glEnableVertexAttribArray(this->shader.getAttribute("tex")); eglGetError();
//		glVertexAttribPointer(this->shader.getAttribute("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*) offsetof(VertexPT, pos)); eglGetError();
//		glVertexAttribPointer(this->shader.getAttribute("tex"), 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*) offsetof(VertexPT, tex)); eglGetError();
//	}

};

template<class T, class S>
class MeshP_Implementation : public Mesh<T, S> {
public:
	MeshP_Implementation(Shader& shader, size_t bufferPageSize = 4096)
		:Mesh<T, S>(shader, bufferPageSize)
	{
	}
	
	int addRectangle(const float& x, const float& y, const float& w, const float& h) {
		int start = this->buffer.size();
		int num = this->buffer.addRectangle(x,y,w,h);
		MeshEntry entry(start, num, GL_TRIANGLES);
		this->entries.push_back(entry);
		this->checkResize();
		return this->entries.size()-1;
	}	
	
//	void setupShader() {
//		this->shader.addUniform("modelview_matrix").addUniform("projection_matrix");
//		this->shader.addAttribute("pos");
//	}
//	
//	void setVertexAttributes() {
//		glEnableVertexAttribArray(this->shader.getAttribute("pos")); eglGetError();
//		glVertexAttribPointer(this->shader.getAttribute("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), (GLvoid*)offsetof(VertexP, pos)); eglGetError();
//	}

};


typedef MeshPT_Implementation<VerticesPT, MeshSetupPT> MeshPT;
typedef MeshP_Implementation<VerticesP, MeshSetupP> MeshP;

} // namespace roxlu

#endif