#include "ruFont.h"

ruFont::ruFont(const string& desc)
{
	createFromString(desc);
}

ruFont::ruFont() 
	:description(NULL)
{
}

ruFont::~ruFont() {
}

/**
 * Load a system font using a css kind of descriptions:
 *
 * Examples:
 * ----------
 * sans bold 12
 * serif,monospace bold italic condensed 16
 * normal 10
 * Arial 16
 *
 */
bool ruFont::createFromString(const string& desc) {
	description = pango_font_description_from_string(desc.c_str());
	if(description == NULL) {
		return false;
	}
	return true;
}


