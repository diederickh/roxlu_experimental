#import <Foundation/Foundation.h>
#include "Application.h"

@interface MacApplication : NSWindow<NSWindowDelegate> {
	Application* app;	
}

- (void) setApplication:(Application*)application; // andOpenGLView:(MacOpenGLView*)glView;
- (void) mouseDown: (NSEvent*) event;
- (void) mouseUp: (NSEvent*) event;
- (void) mouseDragged: (NSEvent*) event;
- (void) scrollWheel: (NSEvent*) event;
- (void) rightMouseDown: (NSEvent*) event;
- (void) rightMouseUp: (NSEvent*) event;
- (void) rightMouseDragged: (NSEvent*) event;
- (void) keyUp: (NSEvent*) event;
- (void) keyDown: (NSEvent*) event;

// Called from MacOpenGLView
- (void) setupApp:(float)width height:(float)height;
- (void) updateApp;
- (void) drawApp;
- (void) resizedApp: (float) width height:(float)height;

- (Application*) getApp;


@end
