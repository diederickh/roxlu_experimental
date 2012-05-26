#ifndef ROXLU_PANGO_RUFONTH
#define ROXLU_PANGO_RUFONTH

#include <pango/pangocairo.h>
#include <string>
using std::string;

class ruFont {
public:
	ruFont();
	ruFont(const string& desc);
	~ruFont();
	
	bool createFromString(const string& desc);

	void setSize(int size);

	void useOblique();
	void useNormal();
	void useItalic();
	
	void setWeightThin(); // 100
	void setWeightUltraLight(); // 200
	void setWeightLight(); // 300
	void setWeightBook(); // 380
	void setWeightNormal(); // 400
	void setWeightMedium(); // 500
	void setWeightSemiBold(); // 600
	void setWeightBold(); // 700
	void setWeightUltraBold(); // 800
	void setWeightHeavy(); // 900
	void setWeightUltraHeavy(); // 1000
	void setWeight(PangoWeight w);

	PangoFontDescription* getFontDescription();
	
private:
	PangoFontDescription* description;

};

inline PangoFontDescription* ruFont::getFontDescription() {
	return description;
}

inline void ruFont::useOblique() {
	pango_font_description_set_style(description, PANGO_STYLE_OBLIQUE);
}

inline void ruFont::useNormal() {
	pango_font_description_set_style(description, PANGO_STYLE_NORMAL);
}

inline void ruFont::useItalic() {
	pango_font_description_set_style(description, PANGO_STYLE_ITALIC);
}

inline void ruFont::setWeightThin()			{	setWeight(PANGO_WEIGHT_THIN);			}
inline void ruFont::setWeightUltraLight()	{	setWeight(PANGO_WEIGHT_ULTRALIGHT);		}
inline void ruFont::setWeightLight()		{	setWeight(PANGO_WEIGHT_LIGHT);			}
inline void ruFont::setWeightBook()			{	setWeight(PANGO_WEIGHT_BOOK);			}
inline void ruFont::setWeightNormal()		{	setWeight(PANGO_WEIGHT_NORMAL);			}
inline void ruFont::setWeightMedium()		{	setWeight(PANGO_WEIGHT_MEDIUM);			}
inline void ruFont::setWeightSemiBold()		{	setWeight(PANGO_WEIGHT_SEMIBOLD);		}
inline void ruFont::setWeightBold()			{	setWeight(PANGO_WEIGHT_BOLD);			}
inline void ruFont::setWeightUltraBold()	{	setWeight(PANGO_WEIGHT_ULTRABOLD);		}
inline void ruFont::setWeightHeavy()		{	setWeight(PANGO_WEIGHT_HEAVY);			}
inline void ruFont::setWeightUltraHeavy()	{	setWeight(PANGO_WEIGHT_ULTRAHEAVY);		}

inline void ruFont::setWeight(PangoWeight w) {
	pango_font_description_set_weight(description, w);
}

inline void ruFont::setSize(int s) {
	pango_font_description_set_size(description, s * PANGO_SCALE);
}
#endif