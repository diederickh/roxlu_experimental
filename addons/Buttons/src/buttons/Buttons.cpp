#include <buttons/Buttons.h>

namespace buttons {

  Shader Buttons::gui_shader = Shader();
  bool Buttons::shaders_initialized = false;
  BitmapFont* Buttons::bmf = NULL;
  unsigned int Buttons::num_created = 0;

  Buttons::Buttons(const string& title, int w)
    :is_mouse_down(false)
    ,first_run(true)
    ,x(0)
    ,y(0)
    ,w(w)
    ,h(0)
    ,win_w(0)
    ,win_h(0)
    ,header_h(20) // header height (draggable area)
    ,border(1)
    ,state(BSTATE_NONE)
    ,is_changed(false)
    ,is_mouse_inside_header(false)
    ,is_mouse_inside_panel(false)
    ,triggered_drag(false)
    ,mdx(0)
    ,mdy(0)
    ,pmx(0)
    ,pmy(0)
    ,title(title)
    ,title_dx(0)
    ,num_panel_vertices(0) 
    ,is_locked(false)
    ,static_text(NULL)
    ,dynamic_text(NULL)
    ,allocated_bytes(0)
    ,is_open(true)
    ,is_visible(true)
    ,name(title)
    ,col_hue(0.0f)
    ,col_sat(0.0f)
    ,col_bright(0.0f)
    ,col_alpha(0.0f)
    ,vao(0)
  {

    if(!shaders_initialized) {
      bmf = new BitmapFont();

      gui_shader.create(BUTTONS_VS, BUTTONS_FS);
      glBindAttribLocation(gui_shader.prog_id, 0, "pos");
      glBindAttribLocation(gui_shader.prog_id, 1, "col");
      gui_shader.link();

      gui_shader.enable();
      gui_shader.addUniform("projection_matrix");
      gui_shader.addAttribute("pos");
      gui_shader.addAttribute("col");
      shaders_initialized = true;
    }
	
    static_text = new Text(*bmf);	
    dynamic_text = new Text(*bmf);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo); eglGetError();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
    glEnableVertexAttribArray(0); eglGetError();
    glEnableVertexAttribArray(1); eglGetError();
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), (GLvoid*)0); 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ButtonVertex), (GLvoid*)8);
    glBindVertexArray(0);

    createOrtho(768, 1024);
	
    // top draggable handle
    BSET_COLOR(header_color_top, 0.07,0.07,0.07,1.0);
    BSET_COLOR(header_color_bottom, 0.1,0.1,0.1,0.8);
    BSET_COLOR(shadow_color, 0.1, 0.1, 0.1, 0.1);
	
    title_dx = static_text->add(x+5, y+2, title);
    name = createCleanName(title);

    ++num_created;
    id = num_created;
  }

  Buttons::~Buttons() {
    vector<Element*>::iterator it = elements.begin();
    while(it != elements.end()) {
      Element* el = *it;
      if(el->is_child) {
        ++it;
        continue;
      }
      delete el;
      ++it;
    }
  }

  void Buttons::setup() {
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
    if(!is_visible) {
      return;
    }

    h = 0;
    bool needs_redraw = false;
    bool needs_text_update = false;
	
    if(is_open) {
      vector<Element*>::iterator it = elements.begin();
      while(it != elements.end()) {
        Element& el = *(*it);
        el.update();
			
        if(el.is_child || !el.is_visible) {
          ++it;
          continue;
        }

        h += el.h;
        ++it;
      }
		
      // check if we need to update the vertices or if a value is changed.
      it = elements.begin();
      if(!is_changed) { // is this panel self changed.
        vector<Element*> value_changed_els;
        bool value_changed = false;
        while(it != elements.end()) {
          Element& el = **it;
          if(el.needs_redraw) {
            needs_redraw = true;
          } 
          if(el.needs_text_update) {
            needs_text_update = true;
          }
          if(el.value_changed) {
            if(el.is_child) {
              el.parent->onChildValueChanged();
            }
            else {
              value_changed_els.push_back(&el);
            }
            el.value_changed = false;
          }
          ++it;
        }
        // notify after all "parents" had their change to update (e..g onChildValueChanged above)
        it = value_changed_els.begin();
        while(it != value_changed_els.end()) {
          notifyListeners(BEVENT_VALUE_CHANGED, *it, (*it)->event_data);
          ++it;
        }
      }
      else {
        // when the gui itself is changed we always update!
        needs_redraw = true;
      }
    }

    if(needs_redraw || is_changed) {
      positionElements();
      updateDynamicTexts(); // need to update everything when a element i.e. get bigger
      updateStaticTextPositions();
      generateVertices();
      notifyListeners(BEVENT_BUTTONS_REDRAW, NULL, NULL);
    }
    if(needs_text_update) {
      updateDynamicTexts();
    }

    // do we need to grow our buffer?
    glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError();
    size_t size_needed = vd.size() * sizeof(ButtonVertex);
    if(size_needed > allocated_bytes) {
      while(allocated_bytes < size_needed) {
        allocated_bytes = std::max<size_t>(allocated_bytes * 2, 256);
      }
      first_run = false; // we don't need this one anymore @todo cleanup
      glBufferData(GL_ARRAY_BUFFER, allocated_bytes, NULL, GL_STREAM_DRAW); eglGetError();
    }
	
    // And update the vbo.
    glBindBuffer(GL_ARRAY_BUFFER, vbo); eglGetError(); // roxlu
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ButtonVertex)*vd.size(), (GLvoid*)vd.getPtr()); eglGetError();
  }

  void Buttons::generateVertices() {
    vd.clear();
    generatePanelVertices();
    generateElementVertices();
  }

  void Buttons::generatePanelVertices() {
    coords.close_x = (x+w)-17;
    coords.close_y = y+4;
    coords.close_w = 13;
    coords.close_h = 13;

    float border_color[4] = { 1,1,1,0.3};
    num_panel_vertices = buttons::createRect(vd, x+1, y+1, getPanelWidth(), getPanelHeight(), shadow_color, shadow_color);
    num_panel_vertices += buttons::createRect(vd, x-border, y-border, getPanelWidth(), getPanelHeight(), border_color, border_color);
    num_panel_vertices += buttons::createRect(vd, x, y, w, header_h, header_color_top, header_color_bottom);
	
    buttons::createRect(vd, coords.close_x, coords.close_y, coords.close_w, coords.close_h, border_color, border_color);
    buttons::createRect(vd, coords.close_x+1, coords.close_y+1, coords.close_w-2, coords.close_h-2, header_color_bottom, header_color_top);
    is_changed = false;
  }

  void Buttons::updateDynamicTexts() {
    vector<Element*>::iterator it = elements.begin();
    while(it != elements.end()) {
      Element* el = *it;
      el->updateDynamicText();
      el->needs_text_update = false;
      ++it;
    }
  }


  void Buttons::generateElementVertices() {
    if(!is_open) {
      return;
    }
	
    vector<Element*>::iterator it = elements.begin();
    while(it != elements.end()) {
      Element* el = *it;
      if(el->is_visible) {
        el->generateVertices(vd);
      }
      el->needs_redraw = false;
      ++it;
    }
  }

  void Buttons::draw() {
    if(!is_visible) {
      return;
    }

    // update projection matrix when viewport size changes.
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    if(vp[2] != win_w || vp[3] != win_h) {
      win_w = vp[2];
      win_h = vp[3];
      createOrtho(win_w, win_h);
    }

    // we draw in 2D so no depth test and cull fase.
    bool cull_enabled = false;
    if(glIsEnabled(GL_CULL_FACE)) {
      glDisable(GL_CULL_FACE);
      cull_enabled = true;
    }

    bool depth_enabled = false;
    if(glIsEnabled(GL_DEPTH_TEST)) {
      glDisable(GL_DEPTH_TEST);
      depth_enabled = true;
    }
	
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gui_shader.enable();
    glBindVertexArray(vao);
    //vao.bind();
	
    gui_shader.uniformMat4fv("projection_matrix", &ortho[0]);
	
    for(int i = 0; i < vd.draw_arrays.size(); ++i) {
      ButtonDrawArray& bda = vd.draw_arrays[i];
      glDrawArrays(bda.mode, bda.start, bda.count);
    }
    //    glBindVertexArray(0); // @todo we might remove this

    static_text->draw();
    dynamic_text->draw();
	
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);	
    //    glBindVertexArray(0);
    glUseProgram(0);

    // Allow custom drawing.
    if(cull_enabled) {
      glEnable(GL_CULL_FACE);
    }

    if(depth_enabled) {
      glEnable(GL_DEPTH_TEST);
    }
	
  }

  void Buttons::debugDraw() {
    gui_shader.disable();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  Sliderf& Buttons::addFloat(const string& label, float& value) {
    buttons::Sliderf* el = new Sliderf(value, createCleanName(label), Sliderf::SLIDER_FLOAT);
    addElement(el, label);
    return *el;
  }

  SliderRangef& Buttons::addFloatRange(const string& label, float* value) {
    buttons::SliderRangef* el = new SliderRangef(value, createCleanName(label));
    addElement(el, label);
    return *el;
  }

  SliderRangei& Buttons::addIntRange(const string& label, int* value) {
    buttons::SliderRangei* el = new SliderRangei(value, createCleanName(label));
    addElement(el, label);
    return *el;
  }

  Slideri& Buttons::addInt(const string& label, int& value) {
    buttons::Slideri* el = new Slideri(value, createCleanName(label), Slideri::SLIDER_INT);
    addElement(el, label);
    return *el;
  }

  Pad<int>& Buttons::addInt2(const string& label, int* value) {
    buttons::Pad<int>* el = new Pad<int>(value, label, PAD_INT);
    addElement(el, label);
    return *el;
  }

  Pad<float>& Buttons::addFloat2(const string& label, float* value) {
    buttons::Pad<float>* el = new Pad<float>(value, label, PAD_FLOAT);
    addElement(el, label);
    return *el;
  }

  Vector<float>& Buttons::addVec2f(const string& label, float* value) {
    buttons::Vector<float>* el = new Vector<float>(value, label, VEC_FLOAT);
    addElement(el, label);
    return *el;
  }

  Rectangle<int>& Buttons::addRectanglei(const string& label, int* value) {
    buttons::Rectangle<int>* el = new Rectangle<int>(value, label, RECT_INT);
    addElement(el, label);
    return *el;
  }

  Rectangle<float>& Buttons::addRectanglef(const string& label, float* value) {
    buttons::Rectangle<float>* el = new Rectangle<float>(value, label, RECT_FLOAT);
    addElement(el, label);
    return *el;
  }

  Toggle& Buttons::addBool(const string& label, bool& value) {
    buttons::Toggle* el = new Toggle(value, createCleanName(label));
    addElement(el, label);
    return *el;
  }

  ColorPicker& Buttons::addColor(const string& label, float* value) {
    buttons::ColorPicker* el = new ColorPicker(createCleanName(label), value);
    addElement(el, label);		
    return *el;
  }

  void Buttons::addElement(Element* el, const string& label) {
    el->setup();
    el->label = label;
    el->w = w;
    el->static_text = static_text;
    el->dynamic_text = dynamic_text;
    el->setColor(col_hue, col_sat, col_bright, col_alpha);
    el->needsRedraw();
	
    elements.push_back(el);
    positionElements();

    el->generateStaticText();
    el->generateDynamicText();

    // Add child elements
    vector<Element*> children;
    el->getChildElements(children);
    addChildElements(el, children);

  }

  void Buttons::addChildElements(Element* parent, vector<Element*>& children) {
    for(vector<Element*>::iterator it = children.begin(); it != children.end(); ++it) {
      Element* el = *it;
      el->setup();
      el->needsRedraw();
      el->is_child = true;
      el->static_text = static_text;
      el->dynamic_text = dynamic_text;
      el->parent = parent;
      elements.push_back(el);	

      el->generateStaticText();
      el->generateDynamicText();	
    }
  }

  void Buttons::onMouseMoved(int mx, int my) {
    if(!is_visible) {
      return;
    }

    mdx = mx - pmx;
    mdy = my - pmy;
	
    // are we inside our header area? TODO: we cn optimize here by returning early
    if(!is_locked) {
      is_mouse_inside_header = BINSIDE_HEADER(this, mx, my);
      if(is_mouse_inside_header && state == BSTATE_NONE) {
        state = BSTATE_ENTER;
        onMouseEnter(mx,my);
      }
      else if(!is_mouse_inside_header && state == BSTATE_ENTER) {
        state = BSTATE_NONE;
        onMouseLeave(mx, my); 
      }
    }
	
    if(is_mouse_down) {
      onMouseDragged(mdx, mdy);
    }

    // check elements when we're opened.	
    if(is_open) {
      // check if we are inside an element and dragging...
      Element* e = elements[0];
      Element* el;
      vector<Element*>::iterator it;
      if(is_mouse_down) {
        it = elements.begin();
        while(it != elements.end()) {
          el = *it;
          if(!el->is_visible) {
            ++it;
            continue;
          }
				
          el->onMouseDragged(mx, my, mdx, mdy);
          ++it;
        }
      }

      // call "leave" and "enter"
      it = elements.begin();
      while(it != elements.end()) {
        el = *it;
        if(!el->is_visible) {
          ++it;
          continue;
        }
			
        el->is_mouse_inside = BINSIDE_ELEMENT(el, mx, my);
        if(el->is_mouse_inside && el->state == BSTATE_NONE) {
          el->state = BSTATE_ENTER;
          el->onMouseEnter(x,y);
        }
        else if(!el->is_mouse_inside && el->state == BSTATE_ENTER) {
          el->state = BSTATE_NONE;
          el->onMouseLeave(mx,my);
        }
        el->onMouseMoved(mx,my);
        ++it;
      }
    }
	
    // is mouse inside the gui
    int panel_h = getPanelHeight();
    if(mx > x && mx < (x+w) && my > y && my < (y+panel_h)) {
      is_mouse_inside_panel = true;
    }
    else {
      is_mouse_inside_panel = false;
    }
	
    // keep track of current x and y;
    pmx = mx;
    pmy = my;
  }

  void Buttons::onMouseDown(int mx, int my) {
    if(!is_visible) {
      return;
    }

    if(!is_locked && BINSIDE_HEADER(this, mx, my)) {
      triggered_drag = true; // the drag was triggered by the gui
    }

    if(BMOUSE_INSIDE(mx, my, coords.close_x, coords.close_y, coords.close_w, coords.close_h)) {
      if(!is_open) {
        open();
      }
      else {
        close();
      }
      return;
    }
	
    is_mouse_down = true;
	
	
    if(!is_open) {
      return;
    }

	
    Element* el;
    vector<Element*>::iterator it = elements.begin();
    it = elements.begin();
    bool inside_el = false;
    while(it != elements.end()) {
      el = *it;
		
      if(!el->is_visible) {
        ++it;
        continue;
      }
		
      el->is_mouse_down_inside = BINSIDE_ELEMENT(el, mx, my);
      el->onMouseDown(mx,my);
      if(is_mouse_down && el->is_mouse_down_inside) {
        el->drag_inside = true;
      }
      else {
        el->drag_inside = false;
      }
      ++it;
    }
  }

  void Buttons::onMouseUp(int mx, int my) {
    if(!is_visible) {
      return;
    }

    triggered_drag = false;
    is_mouse_down = false;
	
    if(!is_open) {
      return;
    }
	
    vector<Element*>::iterator it = elements.begin();
    it = elements.begin();
    Element* el;
    while(it != elements.end()) {
      el = *it;
		
      if(!el->is_visible) {
        ++it;
        continue;
      }
		
      el->is_mouse_down_inside = false;
      el->onMouseUp(mx,my);
      if(el->drag_inside && el->is_mouse_inside) {
        el->onMouseClick(mx,my);
      }
			
      el->drag_inside = false;
      ++it;
    }
  }

  void Buttons::onMouseEnter(int mx, int my) {
    BSET_COLOR(header_color_top, 0.07,0.07,0.07,1.0);
    BSET_COLOR(header_color_bottom, 0.2,0.2,0.2,0.8);
    flagChanged();

  }

  void Buttons::onMouseLeave(int mx, int my) {
    BSET_COLOR(header_color_top, 0.07,0.07,0.07,1.0);
    BSET_COLOR(header_color_bottom, 0.1,0.1,0.1,0.8);
    flagChanged();
  }

  void Buttons::onMouseDragged(int dx, int dy) {
    if(!is_visible) {
      return;
    }

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
      if(el->is_child || !el->is_visible) { // child elements are handled by the parent element
        ++it;
        continue;
      }
		
      el->x = xx;
      el->y = yy;
      el->positionChildren();
		
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

  // @todo, we can use the member "h"
  int Buttons::getElementsHeight() {
    return h;
    /*
      int h = 0;
      vector<Element*>::iterator it = elements.begin();
      while(it != elements.end()) {
      h += (*it)->h;
      ++it;
      }
      return h;
    */
  }

  void Buttons::setPosition(int xx, int yy) {
    x = xx;
    y = yy;
    positionElements();
    updateStaticTextPositions();

    static_text->setTextPosition(title_dx, x + 5, y + 2);
    flagChanged();
  }

  void Buttons::getPosition(int& xx, int& yy) {
    xx = x;
    yy = y;
  }

  void Buttons::updateStaticTextPositions() {
    vector<Element*>::iterator it = elements.begin();
    while(it != elements.end()) {
      Element* el = *it;
      el->updateTextPosition();	
      ++it;
    }
  }

  void Buttons::save() {
    save(title);
  }

  void Buttons::save(const string& file) {
    buttons::Storage storage;
    storage.save(file, this);

    // notify elements.
    vector<Element*>::iterator it = elements.begin();
    while(it != elements.end()) {
      (*it)->onSaved();
      ++it;
    }
  }

  void Buttons::load() {
    load(title);
  }

  void Buttons::load(const string& file) {
    buttons::Storage storage;

    storage.load(file, this);

    // notify elements.
    vector<Element*>::iterator it = elements.begin();
    while(it != elements.end()) {
      (*it)->onLoaded();
      ++it;
    }
  }

  Element* Buttons::getElement(const string name) {

    std::string clean_name = createCleanName(name);

    vector<Element*>::iterator it = std::find_if(elements.begin(), elements.end(), ElementByName(clean_name));
    if(it == elements.end()) {
      return NULL;
    }
    return *it;
  }

  void Buttons::setLock(bool yn) {
    is_locked = yn;
  }

  bool Buttons::isOpen() {
    return is_open;
  }


  void Buttons::setColor(const float hue, float sat, float bright, float a) {
    col_hue = hue;
    col_sat = sat;
    col_bright = bright;
    col_alpha = a;

    for(vector<Element*>::iterator it = elements.begin(); it != elements.end(); ++it) {
      Element& el = **it;
      el.setColor(hue, sat, bright, a);
    }
  }

  void Buttons::addListener(ButtonsListener* listener) {
    listeners.push_back(listener);
  }

  void Buttons::notifyListeners(ButtonsEventType aboutWhat, const Element* target, void* targetData) {
    for(vector<ButtonsListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
      (*it)->onEvent(aboutWhat, *this, target, targetData);
    }
	
    /*
      switch(aboutWhat) {
      case BEVENT_BUTTONS_REDRAW: {
      for(vector<ButtonsListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
      (*it)->onRedraw(*this);
      }
      break;
      }
      default:break;
      }
    */
  }




} // namespace buttons
