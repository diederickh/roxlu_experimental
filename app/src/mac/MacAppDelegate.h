#import <Cocoa/Cocoa.h>
#import "MacWindow.h"

@class MacOpenGLView;

@interface MacAppDelegate : NSObject <NSApplicationDelegate> {
	Application* app;
	MacWindow* win;
	MacOpenGLView* gl_view;
};

- (void) initMacWindow:Application:(MacWindow*) macApp withOpenGLView:(MacOpenGLView*)glView andApplication:(Application*) app;

@end
