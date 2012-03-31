#import "MacOpenGLView.h"
//#import "MacApplication.h"

@implementation MacOpenGLView

static CVReturn MyDisplayLinkCallback(
	 CVDisplayLinkRef displayLink
	,const CVTimeStamp *now
	,const CVTimeStamp *outputTime
	,CVOptionFlags flagsIn
	,CVOptionFlags *flagsOut
	,void *displayLinkContext
)
{
    // go back to Obj-C for easy access to instance variables
    CVReturn result = [(MacOpenGLView *)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) setMacApplication:(MacApplication*) app {
	mac_app = app;
}


// Is called automagically.
- (void) prepareOpenGL {
	// enable vsync
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 

	CVDisplayLinkCreateWithActiveCGDisplays(&display_link);
	CVDisplayLinkSetOutputCallback(display_link, &MyDisplayLinkCallback, self);
	CGLContextObj cgl_context = [[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cgl_pixel_format = [[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(display_link, cgl_context, cgl_pixel_format);
	CVDisplayLinkStart(display_link);
}


- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime {
    // deltaTime is unused in this bare bones demo, but here's how to calculate it using display link info
    delta_time = 1.0 / (outputTime->rateScalar * (double)outputTime->videoTimeScale / (double)outputTime->videoRefreshPeriod);
	//printf("Delta: %f\n", delta_time);
    [self drawFrame];

    return kCVReturnSuccess;
}

- (void)dealloc {
    CVDisplayLinkRelease(display_link);
    [super dealloc];
}

- (void) awakeFromNib {
    NSSize    viewBounds = [self bounds].size;
    view_width = viewBounds.width;
    view_height = viewBounds.height;
}

- (void) reshape {
    NSSize view_bounds = [self bounds].size;
    view_width = view_bounds.width;
    view_height = view_bounds.height;
	
    NSOpenGLContext *current_context = [self openGLContext];
    [current_context makeCurrentContext];

    // remember to lock the context before we touch it since display link is threaded
    CGLLockContext([current_context CGLContextObj]);

    // let the context know we've changed size
    [[self openGLContext] update];
	[mac_app resized];
    CGLUnlockContext([current_context CGLContextObj]);
}

- (void)drawRect:(NSRect)rect {
    [self drawFrame];
}

- (void)drawFrame {
    NSOpenGLContext *current_context = [self openGLContext];
    [current_context makeCurrentContext];

    // must lock GL context because display link is threaded
    CGLLockContext([current_context CGLContextObj]);

    glViewport(0, 0, view_width, view_height);

    glClearColor(0.9f, 0.9f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw here
	[mac_app update];
	[mac_app draw];
	
    [current_context flushBuffer];
    CGLUnlockContext([current_context CGLContextObj]);
}

- (float) getViewWidth {
	return view_width;
}

- (float) getViewHeight {
	return view_height;
}

@end
