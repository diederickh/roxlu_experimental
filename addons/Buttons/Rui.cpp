#include "Rui.h"

Shader Rui::gui_shader = Shader("sh_gui");
bool Rui::shaders_initialized = false;
BitmapFont* Rui::bmf = NULL;

Rui::Rui()
	:is_mouse_down(false)
	,first_run(true)
	,x(0)
	,y(0)
	,w(150)
	,hh(20) // header height (draggable area)
	,state(RSTATE_NONE)
	,is_changed(false)
	,is_mouse_inside(false)
	,triggered_drag(false)
	,mdx(0)
	,mdy(0)
	,pmx(0)
	,pmy(0)
{
	if(!shaders_initialized) {
		bmf = new BitmapFont();
		gui_shader.load();
		gui_shader.enable();
		gui_shader.addUniform("projection_matrix");
		gui_shader.addUniform("model_matrix");
		gui_shader.addAttribute("pos");
		gui_shader.addAttribute("col");
	}
	static_text = new Text(*bmf);	
	vao.bind();
	glGenBuffers(1, &vbo); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
	
	gui_shader.enable();	
	glEnableVertexAttribArray(gui_shader.getAttribute("pos")); eglGetError();
	glEnableVertexAttribArray(gui_shader.getAttribute("col")); eglGetError();
	glVertexAttribPointer(gui_shader.getAttribute("pos"), 2, GL_FLOAT, GL_FALSE, sizeof(RuiVertex), offsetof(RuiVertex,pos));
	glVertexAttribPointer(gui_shader.getAttribute("col"), 4, GL_FLOAT, GL_FALSE, sizeof(RuiVertex), (GLvoid*)offsetof(RuiVertex,col));
	
	gui_shader.disable();
	vao.unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	createOrtho(ofGetWidth(), ofGetHeight());
	
	memset(model, 0, sizeof(float) * 16);
	model[0] = 1.0f;
	model[5] = 1.0f;
	model[10] = 1.0f;
	model[15] = 1.0f;
	
	// top draggable handle
	RSET_COLOR(bg_color, 0.8,0.4,0,1.0);
}

Rui::~Rui() {

}
// http://en.wikipedia.org/wiki/Orthographic_projection_(geometry)
void Rui::createOrtho(float w, float h) {
	float n = 0.0;
	float f = 1.0;

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

	float fmn = f - n;
	ortho[0]  = 2.0f / w;
	ortho[5]  = 2.0f / -h;
	ortho[10] = -2.0f / fmn;
	ortho[12] = -(w)/w;
	ortho[13] = -(h)/-h;
	ortho[14] = -(f+n)/fmn;

}
void Rui::update() {
	vector<RElement*>::iterator it = elements.begin();
	while(it != elements.end()) {
		(*it)->update();
		++it;
	}
	
	// check if we need to update the vertices.
	it = elements.begin();
	bool needs_updating = false;
	if(!is_changed) { // is this panel self changed.
		while(it != elements.end()) {
			if((*it)->is_changed) {
				needs_updating = true;
			} 
			++it;
		}
	}
	else {
		// when the gui itself is changed we always update!
		needs_updating = true;
	}
	
	if(needs_updating) {
		generateVertices();
	}
	
	if(first_run) {
		first_run = false;
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(RuiVertex)*vd.size(), vd.getPtr(), GL_DYNAMIC_DRAW); eglGetError();
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(RuiVertex)*vd.size(), (GLvoid*)vd.getPtr()); eglGetError();

	}
}


void Rui::generateVertices() {
	vd.clear();
	generatePanelVertices();
	generateElementVertices();
}

void Rui::generatePanelVertices() {
	
	vd.add(x,y,bg_color);
	vd.add(x+w,y,bg_color);
	vd.add(x+w,y+hh,bg_color);
	
	vd.add(x+w,y+hh,bg_color);
	vd.add(x,y+hh,bg_color);
	vd.add(x,y,bg_color);
	
	is_changed = false;
	printf("Number in header: %zu\n", vd.size());
}
void Rui::generateElementVertices() {
	

	vector<RElement*>::iterator it = elements.begin();
	while(it != elements.end()) {
		(*it)->generateVertices(vd);
		(*it)->is_changed = false;
		++it;
	}	
}

void Rui::draw() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	gui_shader.enable();
	vao.bind();
	
	
	int start = 0;
	int end = 6;
	
	gui_shader.uniformMat4fv("projection_matrix", &ortho[0]);
	
	RSET_MAT_POS(model, x, y, 0);
	gui_shader.uniformMat4fv("model_matrix", model);
	glDrawArrays(GL_TRIANGLES, start, end);
	start = start + end;
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	RElement* el;
	vector<RElement*>::iterator it = elements.begin();
	while(it != elements.end()) {
		el = (*it);
		end = start + el->num_vertices;

		//RSET_MAT_POS(model, el->x, el->y, -0.5);
		RSET_MAT_POS(model, el->x, el->y, 0);
		gui_shader.uniformMat4fv("model_matrix", model);

		glDrawArrays(GL_TRIANGLES, start, end);

		start += el->num_vertices;
		++it;
		
	}
	
	vao.unbind();
	gui_shader.disable();

	static_text->draw();
}

void Rui::debugDraw() {
	gui_shader.disable();
	vao.unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	int start = 0;
	int end = 0;
	RElement* el;
	vector<RElement*>::iterator it = elements.begin();
	while(it != elements.end()) {
		el = (*it);
		end = start + el->num_vertices;
	
		glBegin(GL_TRIANGLES);
		for(int i = start; i < end; ++i) {
			RuiVertex& rv = vd[i];
			glVertex2f(rv.pos[0], rv.pos[1]);
		}
		glEnd();
		
		start += el->num_vertices;
		++it;
		
	}

}

RSlider& Rui::addFloat(const string& label, float& value) {
	int h = getHeight();
	RSlider* el = new RSlider(value);
	el->setup();
	el->label = label;
	el->w = w;
	el->flagChanged();
	
	elements.push_back(el);
	positionElements();
	el->generateStaticText(*static_text);

	return *el;
}

void Rui::onMouseMoved(int x, int y) {
	mdx = x - pmx;
	mdy = y - pmy;
	// are we inside our header area? TODO: we cn optimize here by returning early
	is_mouse_inside = RINSIDE_HEADER(this, x, y);
	if(is_mouse_inside && state == RSTATE_NONE) {
 		state = RSTATE_ENTER;
		onMouseEnter(x,y);
		triggered_drag = true; // the drag was triggered by the gui
	}
	else if(!is_mouse_inside && state == RSTATE_ENTER) {
		state = RSTATE_NONE;
		onMouseLeave(x, y); 
	}
	
	if(is_mouse_down) {
		onMouseDragged(mdx, mdy);
	}
	
	// check if we are inside an element
	RElement* e = elements[0];
	RElement* el;
	vector<RElement*>::iterator it;
	if(is_mouse_down) {
		it = elements.begin();
		while(it != elements.end()) {
			(*it)->onMouseDragged(mdx,mdy);
			++it;
		}
	}

	it = elements.begin();
	while(it != elements.end()) {
		el = *it;
		el->is_mouse_inside = RINSIDE_ELEMENT(el, x, y);
	//	printf("B: %d\n", el->is_mouse_inside);
		if(el->is_mouse_inside && el->state == RSTATE_NONE) {
			el->state = RSTATE_ENTER;
			el->onMouseEnter(x,y);
		}
		else if(!el->is_mouse_inside && el->state == RSTATE_ENTER) {
			el->state = RSTATE_NONE;
			el->onMouseLeave(x,y);
		}
		el->onMouseMoved(x,y);
		++it;
	}
	
	// keep track of current x and y;
	pmx = x;
	pmy = y;
}

void Rui::onMouseDown(int x, int y) {
	is_mouse_down = true;
}

void Rui::onMouseUp(int x, int y) {
	triggered_drag = false;
	printf(">>>>>>>>>>>>>>>> MOUSE UP <<<<<<<<<<<<<<<<<<<\n");
	is_mouse_down = false;
}

void Rui::onMouseEnter(int x, int y) {
	RSET_COLOR(bg_color, 0.0,1.0,0.0, 1.0);
	flagChanged();

}

void Rui::onMouseLeave(int x, int y) {
	RSET_COLOR(bg_color, 0.8,0.4,0.0, 1.0);
	flagChanged();
}

void Rui::onMouseDragged(int dx, int dy) {
	printf("dx:%d, dy:%y\n", dx, dy);
	if(triggered_drag) {
		setPosition(x+dx, y+dy);
	}
}

void Rui::positionElements() {

	int xx = x;
	int yy = y+hh;
	RElement* el;
	
	vector<RElement*>::iterator it = elements.begin();
	while(it != elements.end()) {
		el = *it;
		el->x = xx;
		el->y = yy;
		yy += el->h;
		++it;
	}
}

int Rui::getHeight() {
	int h = 0;
	vector<RElement*>::iterator it = elements.begin();
	while(it != elements.end()) {
		h += (*it)->h;
		++it;
	}
	return h;
}

void Rui::setPosition(int xx, int yy) {
	x = xx;
	y = yy;
	positionElements();
	static_text->setPosition(x,y);
	flagChanged();
}
