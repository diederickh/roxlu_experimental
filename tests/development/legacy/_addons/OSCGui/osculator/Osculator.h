#import <Foundation/Foundation.h>
#include "OSC.h"

#define OSCU_SLIDER 	1
#define OSCU_BUTTON		2
#define OSCU_COLOR		3
#define OSCU_MATRIX		4
#define OSCU_STRING		5
#define OSCU_CALLBACK	6
#define OSCU_COMMAND	7 	// protocol commands.
#define OSCU_COMMAND_LOAD_SETTINGS	1 // load remote settings.

@interface Osculator : NSObject {
	NSWindow* window;
	NSMutableDictionary* views;
//	NSMutableArray* views;
	OSCSender* osc_sender;
	OSCReceiver* osc_receiver;
}

- (void) update:(NSTimer*)timer;
- (void) loadSettings;
- (void) sendCommand:(int)command;

- (void) generateFromWindow:(NSWindow*)window senderPort:(int)senderPort receiverPort:(int)receiverPort;
- (void) fillSubviews:(NSView*) parent fill:(NSMutableDictionary*)container;

- (void) sliderChanged:(NSSlider*)sender;
- (void) buttonChanged:(NSButton*)button;
- (void) colorChanged:(NSColorWell*)sender;
- (void) matrixChanged:(NSMatrix*)sender;
- (void) listButtonChanged:(NSPopUpButton*)sender;
- (void) segmentedControlChanged:(NSSegmentedControl*)sender;

- (void) sendTextfieldChanged:(const char*) var sender:(NSTextField*)sender;
- (void) sendButtonChanged:(const char*)varname  sender:(NSButton*) button;
- (void) sendSliderChanged:(const char*)varname sender:(NSSlider*) sender;

@end
