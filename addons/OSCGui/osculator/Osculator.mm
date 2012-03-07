#import "Osculator.h"

@implementation Osculator

- (void) generateFromWindow:(NSWindow*)inWindow {
	osc_sender = new OSCSender("localhost",4444);
	
	window = inWindow;
	views = [NSMutableArray array];
	[self fillSubviews:[window contentView] fill:views];
	
}

- (void) fillSubviews:(NSView*) parent fill:(NSMutableArray*)array {
	for(NSView* view in [parent subviews]) {
		[array addObject:view];	
		if([view isKindOfClass:[NSSlider class]]) {
			[(NSSlider*)view setTarget:self];
			[((NSSlider*)view) setAction:@selector(sliderChanged:)];
		}
		else if([view isKindOfClass:[NSPopUpButton class]]) {
			[(NSPopUpButton*)view setTarget:self];
			[(NSPopUpButton*)view setAction:@selector(listButtonChanged:)];
		}
		else if([view isKindOfClass:[NSSegmentedControl class]]) {
			[(NSSegmentedControl*)view setTarget:self];
			[(NSSegmentedControl*)view setAction:@selector(segmentedControlChanged:)];
		}
		else if([view isKindOfClass:[NSButton class]]) {
			[(NSButton*)view setTarget:self];
			[(NSButton*)view setAction:@selector(buttonChanged:)];
		}
		else if([view isKindOfClass:[NSColorWell class]]) {
			[(NSColorWell*)view setTarget:self];
			[(NSColorWell*)view setAction:@selector(colorChanged:)];
		}
		else if([view isKindOfClass:[NSMatrix class]]) {
			[(NSMatrix*)view setTarget:self];
			[(NSMatrix*)view setAction:@selector(matrixChanged:)];
		}
		else {
			[self fillSubviews:view fill:array];
		}
	}
}

- (void) segmentedControlChanged:(NSSegmentedControl*)sender {
	if([sender toolTip] == NULL) {
		NSLog(@"No tooltip set.");
		return;
	}
	NSInteger dx = [sender selectedSegment];
	const char* varname = [[sender toolTip]UTF8String];

	OSCMessage msg;
	msg.addInt32(OSCU_MATRIX);
	msg.addString(varname);
	msg.addInt32((int32_t)dx);
	osc_sender->sendMessage(msg);
}

- (void) listButtonChanged:(NSPopUpButton*)sender {
	if([sender toolTip] == NULL) {
		NSLog(@"No tooltip set.");
		return;
	}
	NSInteger dx = [sender indexOfSelectedItem];
	const char* varname = [[sender toolTip]UTF8String];
	
	OSCMessage msg;
	msg.addInt32(OSCU_MATRIX);
	msg.addString(varname);
	msg.addInt32((int32_t)dx);
	osc_sender->sendMessage(msg);
}

- (void) matrixChanged:(NSMatrix*)sender {
	if([sender toolTip] == NULL) {
		NSLog(@"No tooltip set.");
		return;
	}

	//id selected = [sender selectedCell];
	//NSLog(@" %d", [(NSButtonCell*)selected state]);
	NSInteger sel_row = [sender selectedRow];
	NSInteger sel_col = [sender selectedColumn];
	NSInteger cols = [sender numberOfColumns];
	NSInteger dx = (sel_row * cols) + sel_col;
	const char* varname = [[sender toolTip]UTF8String];
	
	// send which index is selected 
	OSCMessage msg;
	msg.addInt32(OSCU_MATRIX);
	msg.addString(varname);
	msg.addInt32((int32_t)dx);
	osc_sender->sendMessage(msg);
}

- (void) sliderChanged:(NSSlider*)sender {
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
