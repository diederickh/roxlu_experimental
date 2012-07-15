#ifndef ROXLU_BCOLORH
#define ROXLU_BCOLORH

#include <buttons/Types.h>
#include <buttons/Slider.h>
#include <buttons/Element.h>

namespace buttons {

struct ColorPickerVertex {
	ColorPickerVertex():H(0.0f){}
	ButtonVertex bv;
	float H; // the hue
};

struct ColorPickerCoords {
	ColorPickerCoords()
		:circle_cx(0.0f)
		,circle_cy(0.0f)
		,circle_radius(75.0f)
		,preview_x(0)
		,preview_y(0)
		,preview_w(0)
		,preview_h(0)
	{
	}
	int preview_x;
	int preview_y;
	int preview_w;
	int preview_h;
	float circle_cx;
	float circle_cy;
	float circle_radius;
};

// T = color container, we assume RGBA colors, that can be accessed by [0]..[3]
template<class T>
class ColorT : public Element{
public:	
	ColorT(const string& name, T* color);
	~ColorT();
	
	
	void generateStaticText();
	void generateDynamicText();
	void generateVertices(ButtonVertices& shapeVertices);
	void updateTextPosition();
	void updateDynamicText();
	
	void getChildElements(vector<Element*>& elements);
	void positionChildren();

	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);
	void onMouseMoved(int mx, int my); // global move, using is_mouse_inside
	void onMouseDragged(int mx, int my, int dx, int dy);
	
	void save(std::ofstream& ofs);
	void load(std::ifstream& ifs);
	bool canSave();

	int label_dx;	
	T* color;
	
	int H;  // 0 - 360
	int S; // 0 - 100
	int L; // 0 - 100
	int A; // 0 - 100, alpha
	int h_big; 
	int h_small; 
	bool is_big;
	Slideri hue_slider;
	Slideri sat_slider;
	Slideri light_slider;
	Slideri alpha_slider; 
	ColorPickerCoords coords;
	vector<ColorPickerVertex> circle_vertices; 
};

template<class T>
ColorT<T>::ColorT(const string& name, T* color)
	:Element(BTYPE_COLOR, name)
	,color(color)
	,label_dx(-1)
	,H(0)
	,S(1)
	,L(0)
	,A(0)
	,is_big(false)
	,h_big(140)
	,h_small(20)
	,hue_slider(H, name+"_H", Slideri::SLIDER_INT)
	,sat_slider(S, name+"_S", Slideri::SLIDER_INT)
	,light_slider(L, name+"_L", Slideri::SLIDER_INT)
	,alpha_slider(A, name+"_A", Slideri::SLIDER_INT)

{
	this->h = h_small;
	hue_slider.label = "H";
	sat_slider.label = "S";
	light_slider.label = "L";
	alpha_slider.label = "A";
	hue_slider.setMin(0).setMax(360);
	sat_slider.setMin(0).setMax(100);
	light_slider.setMin(0).setMax(100);
	alpha_slider.setMin(0).setMax(100);
}


template<class T>
ColorT<T>::~ColorT(){
}


template<class T>
void ColorT<T>::getChildElements(vector<Element*>& elements) {
	elements.push_back(&hue_slider);
	elements.push_back(&sat_slider);
	elements.push_back(&light_slider);
	elements.push_back(&alpha_slider);
}

template<class T>
void ColorT<T>::positionChildren() {
	float p_circle = 0.7;
	float p_sliders = 0.4;
	float circle_space = (this->w > this->h) ? this->h : this->w;
	
	coords.circle_radius = circle_space * (p_circle * 0.5);
	coords.circle_cx = 10 + this->x + coords.circle_radius;
	coords.circle_cy = this->y + coords.circle_radius + 0.5 * (this->h+20 - (coords.circle_radius*2));

	hue_slider.x = coords.circle_cx + coords.circle_radius + 15;
	hue_slider.y = coords.circle_cy - (2.0 * hue_slider.h);
	hue_slider.w = this->w * p_sliders;
	
	sat_slider.x = hue_slider.x;
	sat_slider.y = hue_slider.y + hue_slider.h;
	sat_slider.w = hue_slider.w;

	light_slider.x = sat_slider.x;
	light_slider.y = sat_slider.y + sat_slider.h;
	light_slider.w = sat_slider.w;
	
	alpha_slider.x = light_slider.x;
	alpha_slider.y = light_slider.y + light_slider.h;
	alpha_slider.w = light_slider.w;

	coords.preview_w = 20;
	coords.preview_h = 15;
	coords.preview_x = (this->x + this->w) - (coords.preview_w + 10);
	coords.preview_y = this->y + 2; 
}


template<class T>
void ColorT<T>::generateStaticText() {
	this->label_dx = static_text->add(this->x+4, this->y+5, this->label,  0.9, 0.9, 0.9, 0.9);
	//hue_slider.generateStaticText(txt);
}

template<class T>
void ColorT<T>::generateDynamicText() {
	//hue_slider.generateDynamicText(txt);
}

template<class T>
void ColorT<T>::updateTextPosition() {
	static_text->setTextPosition(this->label_dx, this->x+4, this->y+5);
	//hue_slider.updateTextPosition(staticText, dynamicText);
}

template<class T>
void ColorT<T>::updateDynamicText() {
}

template<class T>
void ColorT<T>::generateVertices(ButtonVertices& vd) {

	// CREATE SMALL VERSION OF COLOR PICKER
	// ++++++++++++++++++++++++++++++++++++++++
	if(light_slider.is_visible && !is_big) {
		printf("light_slider.is_visible == FALSE and we are SMALL\n");
		light_slider.hide();
		sat_slider.hide();
		hue_slider.hide();
		alpha_slider.hide();
	}
	else if(!light_slider.is_visible && is_big) {
		printf("We are big!\n");
		light_slider.show();
		sat_slider.show();
		hue_slider.show();
		alpha_slider.show();
	}
	
	// create background
	buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color);
	
	// Calculate the alpha background vertices (the transparency grey/white blocks)
	float block_col_white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float block_col_dark[4] = {0.4f, 0.4f, 0.4f, 1.0f};
	float* block_col = block_col_white;
	int num_blocks_i = 5; // must be odd
	int num_blocks_j = 3; // must be odd
	int block_w = float(coords.preview_w) / num_blocks_i;
	int block_h = float(coords.preview_h) / num_blocks_j;
	for(int i = 0; i < num_blocks_i; ++i) {
		for(int j = 0; j < num_blocks_j; ++j) {
			int dx = (j * num_blocks_i) + i;
			if(dx%2 == 0) {
				block_col = block_col_white;
			}
			else {
				block_col = block_col_dark;
			}
			
			int bv_x = coords.preview_x + i * block_w;
			int bv_y = coords.preview_y + j * block_h;
			buttons::createRect(vd, bv_x, bv_y, block_w, block_h, block_col, block_col);
		}
	}
			
	//hue_slider.generateVertices(vd);
	color[3] = float(A)/100.0f;
	HSL_to_RGB(float(hue_slider.value)/360.0f, float(sat_slider.value)/100.0f, float(light_slider.value)/100.0f, &color[0], &color[1], &color[2]);
	
	
	// create color rect
	buttons::createRect(vd, coords.preview_x, coords.preview_y, coords.preview_w, coords.preview_h, color, color);
	
	if(!is_big) {
		return;
	}
	
	// CREATE EXPANDED VERSION OF COLOR PICKER
	// ++++++++++++++++++++++++++++++++++++++++
	
	// Execute once: calculate the vertices
	if(!circle_vertices.size()) {
		int res = 36;			
		circle_vertices.assign(res+1, ColorPickerVertex());
		float ap = 6.28318531 / res;
		for(int i = 0; i <= res; ++i) {
			float a = i * ap;
			circle_vertices[i].bv.setPos(cos(a) * coords.circle_radius,sin(a) * coords.circle_radius);
			circle_vertices[i].H = (float(i)/res);
		}
	}


	// using the current L value we change the colors of the circle
	float outline_col[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	float hsl_col[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	vector<ButtonVertex> ui_circle_fill;
	vector<ButtonVertex> ui_circle_outline;
	ui_circle_fill.push_back(ButtonVertex(coords.circle_cx, coords.circle_cy, hsl_col));
	
	for(std::vector<ColorPickerVertex>::iterator it = circle_vertices.begin(); it != circle_vertices.end(); ++it) {
		ColorPickerVertex& pv = *it;
		HSL_to_RGB(pv.H, float(S)/100.0f, float(L)/100.0f, &hsl_col[0], &hsl_col[1], &hsl_col[2]);
		ButtonVertex bv(coords.circle_cx + pv.bv.pos[0], coords.circle_cy + pv.bv.pos[1], hsl_col);
		ui_circle_fill.push_back(bv);
		bv.setCol(outline_col);
		ui_circle_outline.push_back(bv);
	}
	vd.add(ui_circle_fill, GL_TRIANGLE_FAN);
	vd.add(ui_circle_outline, GL_LINE_STRIP);
}

template<class T>
void ColorT<T>::onMouseDown(int mx, int my) {
	
}

template<class T>
void ColorT<T>::onMouseUp(int mx, int my) {
	
}

template<class T>
void ColorT<T>::onMouseEnter(int mx, int my) {
	
}

template<class T>
void ColorT<T>::onMouseLeave(int mx, int my) {
	
}

template<class T>
void ColorT<T>::onMouseClick(int mx, int my) {
	
	if(is_mouse_inside) {
	//BMOUSE_INSIDE(mx,my,ex,ey,w,h)
		if(BMOUSE_INSIDE(mx, my, coords.preview_x, coords.preview_y, coords.preview_w, coords.preview_h)) {
			is_big = !is_big;
			this->h = (is_big) ? h_big : h_small;
			positionChildren();
			needsRedraw();
		}
	}
		
}

template<class T>
void ColorT<T>::onMouseMoved(int mx, int my) {
	
}

template<class T>
void ColorT<T>::onMouseDragged(int mx, int my, int dx, int dy) {
	
}

template<class T>
bool ColorT<T>::canSave() {
	return false;
}

template<class T>
void ColorT<T>::save(std::ofstream& ofs) {
	
}

template<class T>
void ColorT<T>::load(std::ifstream& ifs) {
	
}


typedef ColorT<float> ColorPicker;

} // buttons


/*
template<class S, class V> 
class SplineEditor : public Element {
public:	
	SplineEditor(const string& name, S& spline);
	~SplineEditor();

	void setup();
	void draw(Shader& shader, const float* pm);
	
	void generateStaticText(Text& txt);
	void updateTextPosition(Text& staticText, Text& dynamicText);
	void generateVertices(ButtonVertices& shapeVertices);
	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);
	void onMouseMoved(int mx, int my); // global move, using is_mouse_inside
	void onMouseDragged(int mx, int my, int dx, int dy);
	
	void save(std::ofstream& ofs);
	void load(std::ifstream& ifs);
	bool canSave();
	
	SplineEditor<S, V>& setColor(const float r, const float g, const float b, const float a = 1.0f);
	
	int selected_handle_dx;
	float selected_handle_col[4];
	float* default_handle_col; // set to tex col
	float editor_bg_bottom_col[4];
	float editor_bg_top_col[4];
	float editor_spline_col[4];
	
	EditorCoords editor_coords;
	int label_dx;
	S& spline;
	vector<V> screen_handles;
};

*/


#endif