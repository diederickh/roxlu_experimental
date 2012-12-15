#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#include "Simulation.h"

	#define eglCheckFramebufferStatus( )\
			{\
				switch ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) )\
				{\
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"         ); assert( 0 ); break;\
					case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:         printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS"         ); assert( 0 ); break;\
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: printf( "\n%s\n\n", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" ); assert( 0 ); break;\
					case GL_FRAMEBUFFER_UNSUPPORTED:                   printf( "\n%s\n\n", "GL_FRAMEBUFFER_UNSUPPORTED"                   ); assert( 0 ); break;\
					default:                                                                                                                              break;\
				}\
			}

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
//@property (nonatomic, getter=getFrameInterval) NSInteger frame_interval;

- (void) createFramebuffer;
- (void) startAnimation;
- (void) stopAnimation;
- (void) drawView:(id)sender;
//- (void) render;
- (BOOL) resizeFromLayer:(CAEAGLLayer*) layer;
@end
