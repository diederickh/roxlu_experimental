#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#include "Simulation.h"

@interface EAGLView : UIView {
	BOOL is_animating;
	id display_link;
	NSInteger frame_interval;
	Simulation simulation;	
	
	EAGLContext* gl;
	GLuint fbo;
	GLint fbo_w;
	GLint fbo_h;
	GLuint color_rbo;
	GLuint depth_rbo;
	
}

@property (readonly, nonatomic, getter=isAnimating) BOOL is_animating;

- (void) createFramebuffer;
- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;
- (BOOL) resizeFromLayer:(CAEAGLLayer*) layer;

@end
