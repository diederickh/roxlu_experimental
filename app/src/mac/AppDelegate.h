#import <Cocoa/Cocoa.h>
#import "MacApplication.h"

@class MacOpenGLView;

@interface AppDelegate : NSObject <NSApplicationDelegate> {
	Application* app;
}

@property (copy) NSString *source_dist_value;
@property (copy) NSString *step_size;

@property (assign) IBOutlet MacOpenGLView *mac_opengl_view;
@property (assign) IBOutlet MacApplication *mac_app;
- (IBAction)onCloseDistanceChange:(id)sender;
- (IBAction)onStepDistanceChange:(id)sender;
- (IBAction)onDrawSourceToggle:(id)sender;
- (IBAction)onDrawRootsToggle:(id)sender;
- (IBAction)onDrawSourceGroupsToggle:(id)sender;
- (IBAction)onDrawEdgePoints:(id)sender;
- (IBAction)onDrawEdgesToggle:(id)sender;
- (IBAction)onVeinsColorChange:(id)sender;

@end
