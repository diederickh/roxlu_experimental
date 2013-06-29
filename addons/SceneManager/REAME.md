# SceneManager

Basic scene manager that can be used  when you application has several different
"scenes". It's not a scene manager as in scene-graph.  This manager will handle 
all scene changing with a handy feature to set a timeout. Once a scene receives 
a timeout a special function is called that you can use to e.g. jump to a screensaver 
modus in your application.

Each Scene class you create implements a couple of member functions as described 
in the `Scene` abstract class. It's important to know that we keep calling the 
`Scene::update()` function, even if the scene is not yet active.  You can use 
this feature to create nice transitions between the current and next/previous 
scene. For example, you can start a nice dissolve when a `STATE_ENTER` state is set.       

Also, make sure to set the state to `STATE_ACTIVE` yourself, see example below. Most 
of the time you want to set the state to `STATE_ACTIVE` directly when you receive a 
`STATE_ENTER` state in your `setState()` function.

_Handling a state change, you must set the state to STATE_ACTIVE yourself!_


````c++
void MyScene::setState(int st) {
  if(state == STATE_ENTER) {
     state = STATE_ACTIVE; 
  }     
}
````