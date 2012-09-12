** BOX2D Wrapper
*** Box2D info
    - More info at: http://www.box2d.org/manual.html
    - shape: 2D geometrical object (rectangle, circle, polygon)
    - fixture: a fixture binds a shape to a body and add properties like density, friction and restitution
    - density (0-#): used to compute mass: 
    - friction (0-1): how objects slide along each other. 
    - restitution (0-1): how an object bounces


Example
=======
using namespace roxlu::box2d
Simulation2D sim;
Rectangle* r = sim.createRectangle();
