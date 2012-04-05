#import <Foundation/Foundation.h>
#include "Application.h"
#include "Keyboard.h"

@interface MacWindow : NSWindow<NSWindowDelegate> {
	Application* app;	
	KeyCode key_map[512];
}


// Mac specific code.
- (void) setApplication:(Application*)application;
- (void) initializeKeyMap;


// Event handlers.
- (void) mouseDown: (NSEvent*) event;
- (void) mouseUp: (NSEvent*) event;
- (void) mouseDragged: (NSEvent*) event;
- (void) scrollWheel: (NSEvent*) event;
- (void) rightMouseDown: (NSEvent*) event;
- (void) rightMouseUp: (NSEvent*) event;
- (void) rightMouseDragged: (NSEvent*) event;
- (void) keyUp: (NSEvent*) event;
- (void) keyDown: (NSEvent*) event;
- (void) flagsChanged: (NSEvent*) event;

// Called from MacOpenGLView
- (void) setupApp:(float)width height:(float)height;
- (void) updateApp;
- (void) drawApp;
- (void) resizedApp: (float) width height:(float)height;

- (Application*) getApp;


@end
