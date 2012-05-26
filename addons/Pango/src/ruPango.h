#ifndef ROXLU_PANGOH
#define ROXLU_PANGOH
/*
Notes:
- recompiled pango, cairo etc.. and replaced the cairo libs + includes of
openframeworks with my compiled files.
*/

#include <string>
#include <vector>
#include <pango/pangocairo.h>
#include "ruFont.h"

using std::string;

class ruPango {
public:
	ruPango();
	~ruPango();
	void setFont(ruFont& font); 
	bool create(int w, int h);
	void update();
	
	int getPixelsWidth();
	int getPixelsHeight();
	unsigned char* getPixels();
	
	void setText(const string& str, int len = -1);
	void setMarkup(const string& str, int len = -1);
	
	int getSurfaceWidth();		
	int getSurfaceHeight();
	int getTextHeight();
	int getTextWidth();
	
	void setTextWidth(int w);
	void setColor(const double& r, const double& g, const double& b, double a = 1.0);
	void fill(const double& r, const double& g, const double& b, const double a = 1.0);
	void alignLeft();
	void alignRight();
	void alignCenter();
	void justify(bool justify = true);

private:
	double color[4];
	
	cairo_t* context;
	cairo_surface_t* surface;
	PangoLayout* layout;
};


inline int ruPango::getSurfaceWidth() {
	return cairo_image_surface_get_width(surface);
}

inline int ruPango::getSurfaceHeight() {
	return cairo_image_surface_get_height(surface);
}

inline int ruPango::getTextHeight() {
	PangoRectangle rect;
	pango_layout_get_pixel_extents(layout,NULL, &rect);
	return rect.height;
}

inline int ruPango::getTextWidth() {
	PangoRectangle rect;
	pango_layout_get_pixel_extents(layout,NULL, &rect);
	return rect.width;
}

inline void ruPango::alignLeft() {
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
}

inline void ruPango::alignRight() {
	pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
}

inline void ruPango::alignCenter() {
	pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
}

inline void ruPango::justify(bool flag) {
	pango_layout_set_justify(layout, flag);
}

inline int ruPango::getPixelsWidth() {
	return getSurfaceWidth();
}

inline int ruPango::getPixelsHeight() {
	return getTextHeight();
}

#endif