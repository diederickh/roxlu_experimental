#import "MacAppDelegate.h"
#import "MacOpenGLView.h"
#include "Application.h"

@implementation MacAppDelegate

- (void)dealloc {
    [super dealloc];
}
	
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

}

- (void) 	initMacWindow:(MacWindow*)macApp 
			withOpenGLView:(MacOpenGLView*)glView 
			andApplication:(Application*)application
{
	win = macApp;
	gl_view = glView;
	

	app = application;
	[win setApplication:app] ;
	[gl_view initMacWindow:win andApplication:app];
}

@end
