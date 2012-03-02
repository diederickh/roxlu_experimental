#import "Osculator.h"

@implementation Osculator

- (void) generateFromWindow:(NSWindow*)inWindow {
	osc_sender = new OSCSender("localhost",4444);
	
	window = inWindow;
	views = [NSMutableArray array];
	[self fillSubviews:[window contentView] fill:views];
	
	NSLog(@"c: %zu\n", [views count]);
}

- (void) fillSubviews:(NSView*) parent fill:(NSMutableArray*)array {
	for(NSView* view in [parent subviews]) {
		[array addObject:view];	
		if([view isKindOfClass:[NSSlider class]]) {
			[(NSSlider*)view setTarget:self];
			[((NSSlider*)view) setAction:@selector(sliderChanged:)];
			NSLog(@"SLIDER: %@", [view toolTip]);
		}
		else if([view isKindOfClass:[NSButton class]]) {
			[(NSButton*)view setTarget:self];
			[(NSButton*)view setAction:@selector(buttonChanged:)];
			NSLog(@"BUTTON.");
		}
		else if([view isKindOfClass:[NSColorWell class]]) {
			[(NSColorWell*)view setTarget:self];
			[(NSColorWell*)view setAction:@selector(colorChanged:)];
			NSLog(@"COLORWELL");
		}
		else {
			[self fillSubviews:view fill:array];
		}
	}
}


- (void) sliderChanged:(NSSlider*)sender {
	NSLog(@"Slider dragged: %f", [sender floatValue]);
	if ([sender toolTip] == NULL) {
		NSLog(@"No tooltip set.");
		return;
	}
	const char* varname = [[sender toolTip]UTF8String];
		
	OSCMessage msg;
	msg.addInt32(OSCU_SLIDER);
	msg.addString(varname);
	msg.addFloat([sender floatValue]);
	osc_sender->sendMessage(msg);
}

- (void) buttonChanged:(NSButton*)button {
	int on = [button state] == NSOnState;
	if([button toolTip] == NULL) {
		NSLog(@"No tooltip set.");
		return;
	}
	const char* varname = [[button toolTip]UTF8String];
	
	OSCMessage msg;
	msg.addInt32(OSCU_BUTTON);
	msg.addString(varname);
	msg.addInt32(on);
	osc_sender->sendMessage(msg);
}

- (void) colorChanged:(NSColorWell*)sender {
	if ([sender toolTip] == NULL) {
		NSLog(@"No tooltip set.");
		return;
	}
	const char* varname = [[sender toolTip]UTF8String];
	NSColor* color = [sender color];
	CGFloat rgba[4];
	[color getRed:rgba green:rgba+1 blue:rgba+2 alpha:rgba+3];
	
	OSCMessage msg;
	msg.addInt32(OSCU_COLOR);
	msg.addString(varname);
	msg.addFloat(rgba[0]);
	msg.addFloat(rgba[1]);
	msg.addFloat(rgba[2]);
	msg.addFloat(rgba[3]);
	osc_sender->sendMessage(msg);
}

@end
