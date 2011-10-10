#ifndef ROXLU_RENDERERH
#define ROXLU_RENDERERH

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
	inline Shader* getShader();
	inline void setScene(Scene* someScene);	
	inline void setCamera(Camera* camera);

private:
	Scene* scene;
	Camera* cam;
	Shader* shader;
};


inline void Renderer::setShader(Shader* someShader) {
	shader = someShader;
}

inline void Renderer::setScene(Scene* someScene) {
	scene = someScene;
}

inline void Renderer::setCamera(Camera* camera) {
	cam = camera;
}

inline Shader* Renderer::getShader() {
	return shader;
}

} // roxlu
#endif