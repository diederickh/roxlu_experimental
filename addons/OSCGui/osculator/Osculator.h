#import <Foundation/Foundation.h>
#include "OSC.h"

#define OSCU_SLIDER 	1
#define OSCU_BUTTON		2
#define OSCU_COLOR		3

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
@end
