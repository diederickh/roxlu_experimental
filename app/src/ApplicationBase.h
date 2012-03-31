#ifndef ROXLU_APPLICATION_BASEH
#define ROXLU_APPLICATION_BASEH

class ApplicationBase {
public:
	void setDefaults();
	void setClearColor(float r, float g, float b, float a);
	
	float view_width;
	float view_height;
	float clear_color[4];
};

inline void ApplicationBase::setDefaults() {
	setClearColor(0.1, 0.1, 0.1, 1.0);
}

inline void ApplicationBase::setClearColor(float r, float g, float b, float a) {
	clear_color[0] = r;
	clear_color[1] = g;
	clear_color[2] = b;
	clear_color[3] = a;	
}

#endif