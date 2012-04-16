#include "Text.h"

namespace roxlu {

bool Text::initialized = false;
Shader Text::shader = Shader("sh_text");


Text::Text(BitmapFont& bmfont)
	:bmfont(bmfont)
	,is_changed(false)
	,buffer_size(4096)
	,x(0)
	,y(0)
{
	if(!initialized) {
		shader.load();
//		shader.create(TEXT_VS, TEXT_FS);
		shader.enable();
		shader.addUniform("projection_matrix");
		shader.addUniform("model_matrix");
		shader.addUniform("font_texture");
		shader.addAttribute("pos");
		shader.addAttribute("tex");
		printf("Initialized the text\n");
	}

	shader.enable();
	projection_matrix.orthoTopLeft(ofGetWidth(),ofGetHeight(), 0.1, 10);
	shader.uniformMat4fv("projection_matrix", projection_matrix.getPtr());
	vao.bind();		
	
	glGenBuffers(1, &vbo); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_DYNAMIC_DRAW); eglGetError();

	glEnableVertexAttribArray(shader.getAttribute("pos")); eglGetError();
	glEnableVertexAttribArray(shader.getAttribute("tex")); eglGetError();
	
	glVertexAttribPointer(shader.getAttribute("pos"), 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), offsetof(TextVertex, pos)); eglGetError();
	glVertexAttribPointer(shader.getAttribute("tex"), 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (GLvoid*)offsetof(TextVertex, uv)); eglGetError();

	shader.uniform1i("font_texture",0);
	initialized = true;
	
	shader.disable();
	vao.unbind(); // when I unbind here, I get a EXC_BAD_ACCESS in glDrawArrays in dra()
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	// create (re-used) model matrix.
	memset(model, 0, sizeof(float) * 16);
	model[0] = 1.0f;
	model[5] = 1.0f;
	model[10] = 1.0f;
	model[15] = 1.0f;
//	for(int i = 0; i < 16; ++i) {
//		printf("%d = %f\n", i, model[i]);
//	}
	
	// create ortho matrix.
	float n = 0.0;
	float f = 1.0;
	float w = ofGetWidth(); // TODO: replace
	float h = ofGetHeight(); // TODO: replace
	float fmn = f - n;
	ortho[1]  = 0.0f;
	ortho[2]  = 0.0f;
	ortho[3]  = 0.0f;
	ortho[4]  = 0.0f;
	ortho[6]  = 0.0f;
	ortho[7]  = 0.0f;
	ortho[8]  = 0.0f;
	ortho[9]  = 0.0f;
	ortho[11] = 0.0f;
	ortho[15] = 1.0f;
	ortho[0]  = 2.0f / w;
	ortho[5]  = 2.0f / -h;
	ortho[10] = -2.0f / fmn;
	ortho[12] = -(w)/w;
	ortho[13] = -(h)/-h;
	ortho[14] = -(f+n)/fmn;
}

Text::~Text() {
}

int Text::add(const float& x, const float& y, const string& str) {
	is_changed = true;
	printf("ADD TEXT: %f, %f (%s)\n", x, y, str.c_str());
	Color4 col(1,1,1,1);
	TextEntry text;
//	text.model_matrix.setPosition(x, y, 0);
	text.pos[0] = x;
	text.pos[1] = y;
	bmfont.buildText(text.vertices, str, &col);
	texts.push_back(text);
	return texts.size()-1;
}

int Text::update(const int& textIndex, const float& x, const float& y, const string& str) {
	// int updateText(int textIndex, const float& x, const float& y, const string& str);
}


void Text::updateBuffer() {
	TextVertices tv;
	vector<TextEntry>::iterator it = texts.begin();
	while(it != texts.end()) {
		TextEntry& t = *it;
		t.start_dx = vertices.size();
		vertices += t.vertices;
		t.end_dx = t.vertices.size();
		++it;			
	}
	
	for(int i = 0; i < texts.size(); ++i) {
		printf("text.start_x = %d, text.end_x = %d, total size:%zu\n", texts[i].start_dx, texts[i].end_dx, vertices.size());
	}
	
	// do we need to resize the vbo
//	vao.bind();
	glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
	if(vertices.numBytes() > buffer_size) {
		buffer_size = vertices.numBytes() * 2;
		printf("How much we need: %zu, how much we do: %zu\n", vertices.numBytes(), buffer_size);
		glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices.getPtr(), GL_DYNAMIC_DRAW); eglGetError();
		printf("Bigger!\n");
	}
	else {
		printf("numBytes:%zu, num vertices: %zu\n", vertices.numBytes(), vertices.size());
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.numBytes(), vertices.getPtr()); eglGetError();
	}


}

void Text::debugDraw() {
	if(is_changed) {
		is_changed = false;
		updateBuffer();
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArrayAPPLE(0);
	glUseProgram(0);
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	
	
	//glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	bmfont.bind();
	ofEnableAlphaBlending();
	int h = bmfont.ch;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor4f(1,1,1,1); // check if this is necessary
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
//	glDepthFunc( GL_LEQUAL);
	//glScalef(3,3,3);
	int prev = 0;
	vector<TextEntry>::iterator it =  texts.begin();
	while(it != texts.end()) {
		TextEntry& text = (*it);
		vector<TextVertex>::iterator tit = text.vertices.begin();
		glTranslatef(10,h,0);
		glBegin(GL_TRIANGLES);
		int start = text.start_dx;
		int end = start + text.end_dx;
		//printf("Start: %d, End:%d\n", start, end);
		for(int i = start; i < end; ++i) {
			TextVertex& tv = vertices.vertices[i];
			glTexCoord2f(tv.uv.x, tv.uv.y);
			glVertex2f(tv.pos.x, tv.pos.y);
		}

		glEnd();
		prev++;
		h += bmfont.ch;
		++it;
	}
	
}

void Text::draw() {
	if(is_changed) {
		is_changed = false;
		updateBuffer();
	}	

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	vao.bind();
	shader.enable();

	glEnable(GL_BLEND);
//

	ofEnableAlphaBlending();
//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	glColor4f(1,1,1,1); // check if this is necessary

	glEnable(GL_TEXTURE_2D); eglGetError();
	glActiveTexture(GL_TEXTURE0); eglGetError();
	bmfont.bind();
	
//	shader.uniformMat4fv("projection_matrix", projection_matrix.getPtr());
	
	/*
		start: 0, end: 18
		start: 18, end: 36
		start: 36, end: 54
		
		Start: 0, End:18
		Start: 18, End:78
		Start: 78, End:96
		
		Start: 0, End:18
		Start: 18, End:78
		Start: 78, End:96
	*/
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//shader.uniformMat4fv("projection_matrix", projection_matrix.getPtr());	
	shader.uniformMat4fv("projection_matrix", ortho);	
	int yy = 0;
	for(int i = 0; i < texts.size(); ++i) {
		TextEntry& te = texts[i];
		int end = texts[i].start_dx + texts[i].end_dx;	
		model[12] = x+texts[i].pos[0] ;
		model[13] = y+texts[i].pos[1] ;
		model[14] = -1;
		//printf("Start: %d, End:%d\n", texts[i].start_dx, end);
		//printf("Drag at: %f, %f\n", model[12], model[13]);
		shader.uniformMat4fv("model_matrix", model);
		int start = te.start_dx;
		printf("Start: %d, end: %d\n", start, end);
//		glDrawArrays(GL_POINTS, start, end); eglGetError();
	//	glDrawArrays(GL_TRIANGLES, texts[i].start_dx, texts[i].end_dx); eglGetError();
//		glDrawArrays(GL_TRIANGLES, texts[i].start_dx, end);
	}
	//printf("---\n");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Text::setPosition(const int& xx, const int& yy) {
	x = xx;
	y = yy;
}

void Text::translate(const int& xx, const int& yy) {
	x += xx;
	y += yy;
//	vector<TextEntry>::iterator it = texts.begin();
//	while(it != texts.end()) {
//		(*it).pos[0] += x;
//		(*it).pos[1] += y;
//		++it;
//	}
}

} // roxlu