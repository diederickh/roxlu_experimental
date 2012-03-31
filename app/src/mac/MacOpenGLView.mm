#import "MacOpenGLView.h"
#import "MacApplication.h"

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

- (void) initMacApplication:(MacApplication*)macApp application:(ApplicationBase*) baseApp {
	mac_app = macApp;
	base_app = baseApp;
	[mac_app setupApp:view_width height:view_height];
	base_app->setDefaults();
	
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
	
	CVDisplayLinkCreateWithActiveCGDisplays(&display_link);
	CVDisplayLinkSetOutputCallback(display_link, &MyDisplayLinkCallback, self);
	CGLContextObj cgl_context = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cgl_pixel_format = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(display_link, cgl_context, cgl_pixel_format);
	CVDisplayLinkStart(display_link);	
}


- (void) prepareOpenGL {
	
}



- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime {
	delta_time = 1.0 / (outputTime->rateScalar * (double)outputTime->videoTimeScale / (double)outputTime->videoRefreshPeriod);
	[self draw];
	return kCVReturnSuccess;
}

- (void)dealloc {
    CVDisplayLinkRelease(display_link);
    [super dealloc];
}

- (void) awakeFromNib {
    NSSize  viewBounds = [self bounds].size;
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
    CGLLockContext((CGLContextObj)[current_context CGLContextObj]);

    // let the context know we've changed size
    [[self openGLContext] update];
	[mac_app resizedApp:view_width height:view_height];
    CGLUnlockContext((CGLContextObj)[current_context CGLContextObj]);
}

- (void) lockOpenGL {
    NSOpenGLContext *current_context = [self openGLContext];
    [current_context makeCurrentContext];
    CGLLockContext((CGLContextObj)[current_context CGLContextObj]);
}

- (void) unlockOpenGL {
    NSOpenGLContext *current_context = [self openGLContext];
    [current_context makeCurrentContext];
    CGLUnlockContext((CGLContextObj)[current_context CGLContextObj]);
}

- (void)draw {
    NSOpenGLContext *current_context = [self openGLContext];
    [current_context makeCurrentContext];
    CGLLockContext((CGLContextObj)[current_context CGLContextObj]);

    glViewport(0, 0, view_width, view_height);

    glClearColor(base_app->clear_color[0],base_app->clear_color[1],base_app->clear_color[2],base_app->clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw here
	[mac_app updateApp];
	[mac_app drawApp];

    [current_context flushBuffer];
    CGLUnlockContext((CGLContextObj)[current_context CGLContextObj]);
}

- (float) getViewWidth {
	return view_width;
}

- (float) getViewHeight {
	return view_height;
}

@end
