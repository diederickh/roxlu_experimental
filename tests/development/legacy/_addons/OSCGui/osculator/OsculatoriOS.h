#import <Foundation/Foundation.h>
#include "OSC.h"

@interface OsculatoriOS : NSObject {
	NSMutableArray* views;	
	OSCSender* osc_sender;
}

- (void) generateFromWindow:(UIWindow*) window;
- (void) fillSubviews:(NSArray*) views fill:(NSMutableArray*) array;
- (void) sliderChanged:(UISlider*) sender;

@end

/*
#import <Foundation/Foundation.h>
#include "OSC.h"

#define OSCU_SLIDER 	1
#define OSCU_BUTTON		2
#define OSCU_COLOR		3
#define OSCU_MATRIX		4

@interface Osculator : NSObject {
	NSWindow* window;
	NSMutableArray* views;
	OSCSender* osc_sender;
}

- (void) generateFromWindow:(NSWindow*) window;
- (void) fillSubviews:(NSView*) parent fill:(NSMutableArray*)array;

- (void) sliderChanged:(NSSlider*)sender;
- (void) buttonChanged:(NSButton*)button;
- (void) colorChanged:(NSColorWell*)sender;
- (void) matrixChanged:(NSMatrix*)sender;
- (void) listButtonChanged:(NSPopUpButton*)sender;
- (void) segmentedControlChanged:(NSSegmentedControl*)sender;
@end
*/