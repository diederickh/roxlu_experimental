
#import "EAGLView.h"

@implementation EAGLView

@synthesize is_animating;


+ (Class) layerClass {
	return [CAEAGLLayer class];
}

- (id) initWithFrame: (CGRect)frame {
	self = [super initWithFrame:frame];
	
	if(self) {
		CAEAGLLayer* layer = (CAEAGLLayer*)self.layer;
		layer.opaque = TRUE;
		layer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], 
										kEAGLDrawablePropertyRetainedBacking, 
										kEAGLColorFormatRGBA8, 
										kEAGLDrawablePropertyColorFormat, 
										nil];
										
		is_animating = FALSE;
		display_link = nil;
		frame_interval = 1;
		
		gl = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		if(!gl || ![EAGLContext setCurrentContext:gl]) {
			return nil;
		}
		[self createFramebuffer];
		simulation.setup();
	}
	
	return self;
}


- (void) drawView:(id) sender {
	if(is_animating) {
		[EAGLContext setCurrentContext:gl];
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, fbo_w, fbo_h);
		
		glClearColor(0.5f, 0.4f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		simulation.update();
		simulation.draw();
		
		glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
	    [gl presentRenderbuffer:GL_RENDERBUFFER];	
	}
}

- (BOOL) resizeFromLayer:(CAEAGLLayer*) glLayer {
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
	[gl renderbufferStorage:GL_RENDERBUFFER fromDrawable:glLayer];
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &fbo_w);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &fbo_h);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		 NSLog(@"ERROR: failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return FALSE;
	}
	return TRUE;
}

- (void) startAnimation {
	if(!is_animating) {
		display_link = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
		[display_link setFrameInterval:frame_interval];
		[display_link addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		is_animating = true;
	}
}

- (void) stopAnimation {
	if(is_animating) {
		[display_link invalidate];
		is_animating = false;
	}
}

- (void) layoutSubviews {
	[self resizeFromLayer: (CAEAGLLayer*) self.layer];
}

- (void) createFramebuffer {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		
		glGenRenderbuffers(1, &color_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, color_rbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_rbo);

}

@end
