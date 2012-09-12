#ifndef ROXLU_BOX2D_UTILSH
#define ROXLU_BOX2D_UTILSH

static float b2_pixels_per_meter = 100.0f;
static float b2_meters_per_pixel = 1.0f/b2_pixels_per_meter;

#define		PIXELS_TO_METERS(x)	((x)*(b2_meters_per_pixel))
#define		METERS_TO_PIXELS(x)	((x)*(b2_pixels_per_meter))

#endif