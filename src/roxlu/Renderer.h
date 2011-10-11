#ifndef ROXLU_RENDERERH
#define ROXLU_RENDERERH

#include "OpenGL.h"
namespace roxlu {



class Scene;
class Camera;
class Shader;

class Renderer {
public:
	Renderer();
	~Renderer();
	void render();
	
	inline void setShader(Shader* someShader);
	inline void setShader(Shader& someShader);
	inline Shader* getShader();
	inline void setScene(Scene* someScene);	
	inline void setScene(Scene& scene);
	inline void setCamera(Camera* camera);
	inline void setCamera(Camera& camera);
	inline void	fill();
	inline void noFill();

private:
	bool use_fill;
	Scene* scene;
	Camera* cam;
	Shader* shader;
};

inline void Renderer::fill() {
	use_fill = true;
}

inline void Renderer::noFill() {
	use_fill = false;
}

inline void Renderer::setShader(Shader& someShader) {
	setShader(&someShader);
}

inline void Renderer::setShader(Shader* someShader) {
	shader = someShader;
}

inline void Renderer::setScene(Scene& someScene) {
	setScene(&someScene);
}

inline void Renderer::setScene(Scene* someScene) {
	scene = someScene;
}

inline void Renderer::setCamera(Camera& camera) {
	setCamera(&camera);
}

inline void Renderer::setCamera(Camera* camera) {
	cam = camera;
}

inline Shader* Renderer::getShader() {
	return shader;
}

} // roxlu
#endif