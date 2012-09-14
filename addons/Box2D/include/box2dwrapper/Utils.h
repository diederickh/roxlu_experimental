#ifndef ROXLU_BOX2D_UTILSH
#define ROXLU_BOX2D_UTILSH

static float b2_pixels_per_meter = 100.0f;
static float b2_meters_per_pixel = 1.0f/b2_pixels_per_meter;

#define PIXELS_TO_METERS(x)	((x)*(b2_meters_per_pixel))
#define METERS_TO_PIXELS(x)	((x)*(b2_pixels_per_meter))

// Helpers, work from top left corner 
#define W_PIXELS_TO_METERS(w) PIXELS_TO_METERS(((w)*0.5))
#define H_PIXELS_TO_METERS(h) PIXELS_TO_METERS(((h)*0.5))
#define X_PIXELS_TO_METERS(x,w) PIXELS_TO_METERS(((w)*0.5)+(x))
#define Y_PIXELS_TO_METERS(y,h) PIXELS_TO_METERS(((h)*0.5)+(y))
#define SET_SHAPE_AS_BOX(shape, w, h) shape.SetAsBox(W_PIXELS_TO_METERS(w), H_PIXELS_TO_METERS(h))
#define SET_BODYDEF_POSITION(body, x, y, w, h) body.position.Set(X_PIXELS_TO_METERS(x, w), Y_PIXELS_TO_METERS(y, h))
#define VEC2_PIXELS_TO_METERS(x, y) b2Vec2(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y))
#define GET_BODY_POSITION_IN_PIXELS(body, result) result = body->GetTransform().p; result.Set(METERS_TO_PIXELS(result.x), METERS_TO_PIXELS(result.y)); 

#endif
