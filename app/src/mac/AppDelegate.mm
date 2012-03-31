 #import "AppDelegate.h"
#import "MacOpenGLView.h"
#import "MacApplication.h"

@implementation AppDelegate

@synthesize mac_opengl_view;
@synthesize mac_app;
@synthesize source_dist_value;
@synthesize step_size;

- (void)dealloc {
    [super dealloc];
}
	
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	app = new Application();	
	[mac_app setApplication:app] ;
	[mac_opengl_view initMacApplication:mac_app application:app];
}

- (IBAction)onCloseDistanceChange:(id)sender {
	app->v.veins.remove_source_if_closer_dist = [sender floatValue];
}

- (IBAction)onStepDistanceChange:(id)sender {
	app->v.veins.D = [sender floatValue];
}

- (IBAction)onDrawSourceToggle:(id)sender {
	app->v.veins.draw_sources = [sender state];
}

- (IBAction)onDrawRootsToggle:(id)sender {
	app->v.veins.draw_roots = [sender state];
}

- (IBAction)onDrawSourceGroupsToggle:(id)sender {
	app->v.veins.draw_source_groups = [sender state];
}

- (IBAction)onDrawEdgePoints:(id)sender {
	app->v.veins.draw_edge_points = [sender state];
}

- (IBAction)onDrawEdgesToggle:(id)sender {
	app->v.veins.draw_edges = [sender state];
}

- (IBAction)onVeinsColorChange:(id)sender {
	float* c = app->v.veins.veins_color;
	NSColor* col = [sender color];
	c[0] = (float)[col redComponent];
	c[1] = (float)[col greenComponent];
	c[2] = (float)[col blueComponent];
//	[col getRed:c green:c+1 blue:c+2 alpha:&a];
}
@end
