/**
 * Custom NSOpenGLView class which hooks into the display link
 * to get frame-sync'd drawing.
 */

#import <AppKit/AppKit.h>
#include <OpenGL/gl.h>
#import <QuartzCore/QuartzCore.h>
#import "MacApplication.h"
#include "ApplicationBase.h"
	
@interface MacOpenGLView : NSOpenGLView {
	CVDisplayLinkRef display_link;
	double delta_time;
    float view_width;
    float view_height;
	MacApplication* mac_app;
	ApplicationBase* base_app;
}

- (float) getViewWidth;
- (float) getViewHeight;
- (void) initMacApplication:(MacApplication*)macApp application:(ApplicationBase*) baseApp;
- (CVReturn) getFrameForTime:(const CVTimeStamp *)outputTime;
- (void) draw;
- (void) lockOpenGL;
- (void) unlockOpenGL;

@end
