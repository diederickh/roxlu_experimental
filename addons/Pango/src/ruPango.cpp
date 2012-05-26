#include "ruPango.h"

ruPango::ruPango() 
	:surface(NULL)
	,context(NULL)
{
	color[0] = 0.0;
	color[1] = 0.0;
	color[2] = 0.0;
	color[3] = 0.0;
}

ruPango::~ruPango() {
	if(context != NULL) {
		cairo_destroy(context);
		context = NULL;
	}
	if(surface != NULL) {
		cairo_surface_destroy(surface);
		surface = NULL;
	}
	if(layout != NULL) {
		 g_object_unref(layout);
		 layout = NULL;
	}
}
	

bool ruPango::create(int w, int h) {
		
	// create a ARGB surface.
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
	cairo_status_t status = cairo_surface_status(surface);
	if(status != CAIRO_STATUS_SUCCESS) {
		return false;
	}

	// create the cairo context	
	context = cairo_create(surface);
	status = cairo_status(context);
	if(status == CAIRO_STATUS_NO_MEMORY) {
		return false;
	}
	
	// create pango context
	layout = pango_cairo_create_layout(context);
	
	// tell pango the width into which it should render.
	setTextWidth(w);
	return true;
}

void ruPango::setTextWidth(int w) {
	pango_layout_set_width(layout, w * PANGO_SCALE);
}

void ruPango::setFont(ruFont& font) {
	pango_layout_set_font_description(layout, font.getFontDescription());
}

void ruPango::setText(const string& str, int len) {
	pango_layout_set_text(layout, str.c_str(), len);
}

// http://developer.gnome.org/pango/stable/PangoMarkupFormat.html
void ruPango::setMarkup(const string& str, int len) {
	pango_layout_set_markup(layout, str.c_str(), len);
}

void ruPango::update() {
	pango_cairo_show_layout(context, layout);
}

unsigned char* ruPango::getPixels() {	
	cairo_surface_flush(surface);
	return cairo_image_surface_get_data(surface);
}

void ruPango::setColor(const double& r, const double& g, const double& b, double a) {
	cairo_set_source_rgba(context, r,g,b,a);
	color[0] = r;
	color[1] = g; 
	color[2] = b;
	color[3] = a;
}

void ruPango::fill(const double& r, const double& g, const double& b, const double a) {
	cairo_set_source_rgb(context, r,g,b);
	cairo_paint_with_alpha(context, a);
	setColor(color[0], color[1], color[2], color[3]);
}