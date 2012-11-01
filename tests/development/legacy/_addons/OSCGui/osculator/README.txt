
OSCUCLATOR
==========
- Rename AppDelegate.m to AppDelegate.mm
- Create a Osculator object
- Initialize with window
- Give each gui object a tooltip; the same as in your OF project.

--------------------------------
#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window = _window;

- (void)dealloc
{
    [super dealloc];
}
	
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	
	osculator = [[Osculator alloc]init];
	[osculator generateFromWindow:[self window]];
}

@end
---------------------------

In you OF project create a OSCGui object listening to a port, then add 
the variables with the same name as the tooltips in the Cocoa object.

gui.addBool("draw_circle", draw_circle);
gui.addFloat("particle_velocity", val);
sgui.addColor("color", col);