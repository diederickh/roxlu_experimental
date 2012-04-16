#include "Buttons.h"

namespace buttons {

Shader Buttons::gui_shader = Shader();
bool Buttons::shaders_initialized = false;
BitmapFont* Buttons::bmf = NULL;

Buttons::Buttons(const string& title, int w)
	:is_mouse_down(false)
	,first_run(true)
	,x(0)
	,y(0)
	,w(w)
	,win_w(0)
	,win_h(0)
	,header_h(20) // header height (draggable area)
	,border(1)
	,state(BSTATE_NONE)
	,is_changed(false)
	,is_mouse_inside(false)
	,triggered_drag(false)
	,mdx(0)
	,mdy(0)
	,pmx(0)
	,pmy(0)
	,title(title)
	,num_panel_vertices(0) 
{
	if(!shaders_initialized) {
		bmf = new BitmapFont();
		gui_shader.create(BUTTONS_VS, BUTTONS_FS);
		gui_shader.enable();
		gui_shader.addUniform("projection_matrix");
		gui_shader.addAttribute("pos");
		gui_shader.addAttribute("col");
	}
	
	static_text = new Text(*bmf);	
	dynamic_text = new Text(*bmf);
	vao.bind();
	glGenBuffers(1, &vbo); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
	
	gui_shader.enable();	
	glEnableVertexAttribArray(gui_shader.getAttribute("pos")); eglGetError();
	glEnableVertexAttribArray(gui_shader.getAttribute("col")); eglGetError();
	glVertexAttribPointer(gui_shader.getAttribute("pos"), 2, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), offsetof(ButtonVertex,pos));
	glVertexAttribPointer(gui_shader.getAttribute("col"), 4, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), (GLvoid*)offsetof(ButtonVertex,col));
	
	gui_shader.disable();
	vao.unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	createOrtho(ofGetWidth(), ofGetHeight());
	
	// top draggable handle
	BSET_COLOR(header_color_top, 0.07,0.07,0.07,1.0);
	BSET_COLOR(header_color_bottom, 0.1,0.1,0.1,0.8);
	BSET_COLOR(shadow_color, 0.1, 0.1, 0.1, 0.1);
	
	title_dx = static_text->add(x+5, y+2, title);
}

Buttons::~Buttons() {
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		delete *it;
		++it;
	}
}

// http://en.wikipedia.org/wiki/Orthographic_projection_(geometry)
void Buttons::createOrtho(float w, float h) {
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

void Buttons::update() {
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		(*it)->update();
		++it;
	}
	
	// check if we need to update the vertices.
	it = elements.begin();
	bool needs_redraw = false;
	bool needs_text_update = false;
	if(!is_changed) { // is this panel self changed.
		while(it != elements.end()) {
			if((*it)->needs_redraw) {
				needs_redraw = true;
			} 
			if((*it)->needs_text_update) {
				needs_text_update = true;
			}
			++it;
		}
	}
	else {
		// when the gui itself is changed we always update!
		needs_redraw = true;
	}
	
	if(needs_redraw) {
		generateVertices();
	}
	if(needs_text_update) {
		updateDynamicTexts();
	}
	
	if(first_run) {
		first_run = false;
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(ButtonVertex)*vd.size(), vd.getPtr(), GL_DYNAMIC_DRAW); eglGetError();
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ButtonVertex)*vd.size(), (GLvoid*)vd.getPtr()); eglGetError();
	}
}

void Buttons::generateVertices() {
	vd.clear();
	generatePanelVertices();
	generateElementVertices();
}

void Buttons::generatePanelVertices() {
	float border_color[4] = { 1,1,1,0.3};
	num_panel_vertices = buttons::createRect(vd, x+1, y+1, getPanelWidth(), getPanelHeight(), shadow_color, shadow_color);
	num_panel_vertices += buttons::createRect(vd, x-border, y-border, getPanelWidth(), getPanelHeight(), border_color, border_color);
	num_panel_vertices += buttons::createRect(vd, x, y, w, header_h, header_color_top, header_color_bottom);
	is_changed = false;
}

void Buttons::updateDynamicTexts() {
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		(*it)->updateDynamicText(*dynamic_text);
		(*it)->needs_text_update = false;
		++it;
	}
}


void Buttons::generateElementVertices() {
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		(*it)->generateVertices(vd);
		(*it)->needs_redraw = false;
		++it;
	}	
}

void Buttons::draw() {
	// update projection matrix when viewport size changes.
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	if(vp[2] != win_w || vp[3] != win_h) {
		win_w = vp[2];
		win_h = vp[3];
		createOrtho(win_w, win_h);
	}

	// we draw in 2D so no depth test and cull fase.
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	gui_shader.enable();
	vao.bind();
	
	gui_shader.uniformMat4fv("projection_matrix", &ortho[0]);
	
	// draw header.
	int start = 0;
	int end = num_panel_vertices;
	glDrawArrays(GL_TRIANGLES, start, num_panel_vertices); eglGetError();

	start = start + end;
	
	Element* el;
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		el = (*it);
		end = el->num_vertices;
		glDrawArrays(GL_TRIANGLES, start, end);
		start += el->num_vertices;
		++it;
	}
	
	vao.unbind();
	gui_shader.disable();
	static_text->draw();
	dynamic_text->draw();
}

void Buttons::debugDraw() {
	gui_shader.disable();
	vao.unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	int start = 0;
	int end = 0;
	Element* el;
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		el = (*it);
		end = start + el->num_vertices;
	
		glBegin(GL_TRIANGLES);
		for(int i = start; i < end; ++i) {
			ButtonVertex& rv = vd[i];
			glVertex2f(rv.pos[0], rv.pos[1]);
		}
		glEnd();
		
		start += el->num_vertices;
		++it;
	}
}

Slider& Buttons::addFloat(const string& label, float& value) {
	buttons::Slider* el = new Slider(value);
	addElement(el, label);
	return *el;
}

Toggle& Buttons::addBool(const string& label, bool& value) {
	buttons::Toggle* el = new Toggle(value);
	addElement(el, label);
	return *el;
}

void Buttons::addElement(Element* el, const string& label) {
	el->setup();
	el->label = label;
	el->w = w;
	el->needsRedraw();
	
	elements.push_back(el);
	positionElements();
	el->generateStaticText(*static_text);
	el->generateDynamicText(*dynamic_text);
}

void Buttons::onMouseMoved(int x, int y) {
	mdx = x - pmx;
	mdy = y - pmy;
	
	// are we inside our header area? TODO: we cn optimize here by returning early
	is_mouse_inside = BINSIDE_HEADER(this, x, y);
	if(is_mouse_inside && state == BSTATE_NONE) {
 		state = BSTATE_ENTER;
		onMouseEnter(x,y);
	}
	else if(!is_mouse_inside && state == BSTATE_ENTER) {
		state = BSTATE_NONE;
		onMouseLeave(x, y); 
	}
	
	if(is_mouse_down) {
		onMouseDragged(mdx, mdy);
	}
	
	// check if we are inside an element and dragging...
	Element* e = elements[0];
	Element* el;
	vector<Element*>::iterator it;
	if(is_mouse_down) {
		it = elements.begin();
		while(it != elements.end()) {
			(*it)->onMouseDragged(x, y, mdx,mdy);
			++it;
		}
	}

	// call "leave" and "enter"
	it = elements.begin();
	while(it != elements.end()) {
		el = *it;
		el->is_mouse_inside = BINSIDE_ELEMENT(el, x, y);
		if(el->is_mouse_inside && el->state == BSTATE_NONE) {
			el->state = BSTATE_ENTER;
			el->onMouseEnter(x,y);
		}
		else if(!el->is_mouse_inside && el->state == BSTATE_ENTER) {
			el->state = BSTATE_NONE;
			el->onMouseLeave(x,y);
		}
		el->onMouseMoved(x,y);
		++it;
	}
	
	// keep track of current x and y;
	pmx = x;
	pmy = y;
}

void Buttons::onMouseDown(int x, int y) {
	if(BINSIDE_HEADER(this, x, y)) {
		triggered_drag = true; // the drag was triggered by the gui
	}

	is_mouse_down = true;
	Element* el;
	vector<Element*>::iterator it = elements.begin();
	it = elements.begin();
	while(it != elements.end()) {
		el = *it;
		el->onMouseDown(x,y);
		if(is_mouse_down && BINSIDE_ELEMENT(el, x, y)) {
			el->drag_inside = true;
		}
		else {
			el->drag_inside = false;
		}
		++it;
	}
}

void Buttons::onMouseUp(int x, int y) {
	triggered_drag = false;
	is_mouse_down = false;
	vector<Element*>::iterator it = elements.begin();
	it = elements.begin();
	Element* el;
	while(it != elements.end()) {
		el = *it;
		el->onMouseUp(x,y);
		if(el->drag_inside && el->is_mouse_inside) {
			el->onMouseClick(x,y);
		}	
		el->drag_inside = false;
		++it;
	}
}

void Buttons::onMouseEnter(int x, int y) {
	BSET_COLOR(header_color_top, 0.07,0.07,0.07,1.0);
	BSET_COLOR(header_color_bottom, 0.2,0.2,0.2,0.8);
	flagChanged();

}

void Buttons::onMouseLeave(int x, int y) {
	BSET_COLOR(header_color_top, 0.07,0.07,0.07,1.0);
	BSET_COLOR(header_color_bottom, 0.1,0.1,0.1,0.8);
	flagChanged();
}

void Buttons::onMouseDragged(int dx, int dy) {
	if(triggered_drag) {
		setPosition(x+dx, y+dy);
	}
}

void Buttons::onResize(int nw, int nh) {
	createOrtho(nw,nh);
}

void Buttons::positionElements() {
	int xx = x;
	int yy = y+header_h;
	Element* el;
	
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		el = *it;
		el->x = xx;
		el->y = yy;
		yy += el->h;
		++it;
	}
}

int Buttons::getPanelHeight() {
	return header_h + getElementsHeight() + border * 2;
}

int Buttons::getPanelWidth() {
	return w + border*2;
}

int Buttons::getElementsHeight() {
	int h = 0;
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		h += (*it)->h;
		++it;
	}
	return h;
}

void Buttons::setPosition(int xx, int yy) {
	x = xx;
	y = yy;
	positionElements();
	updateStaticTextPositions();

	static_text->setTextPosition(title_dx, x + 5, y + 2);
	flagChanged();
}

void Buttons::updateStaticTextPositions() {
	vector<Element*>::iterator it = elements.begin();
	while(it != elements.end()) {
		(*it)->updateTextPosition(*static_text, *dynamic_text);	
		++it;
	}
}

void Buttons::save() {
	save(title);
}

void Buttons::save(const string& file) {
	buttons::Storage storage;
	storage.save(file, this);
}

void Buttons::load() {
	load(title);
}

void Buttons::load(const string& file) {
	buttons::Storage storage;
	storage.load(file, this);
}

} // namespace buttons
