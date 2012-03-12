#import "Osculator.h"

@implementation Osculator

- (void) generateFromWindow:(NSWindow*)inWindow port:(int)port{
	osc_sender = new OSCSender("localhost",port);
	
	window = inWindow;
	views = [NSMutableArray array];
	[self fillSubviews:[window contentView] fill:views];
	
}

- (void) fillSubviews:(NSView*) parent fill:(NSMutableArray*)array {
	for(NSView* view in [parent subviews]) {
		[array addObject:view];	
		
		if([view isKindOfClass:[NSSlider class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[(NSSlider*)view setTarget:self];
			[((NSSlider*)view) setAction:@selector(sliderChanged:)];
		}
		else if([view isKindOfClass:[NSPopUpButton class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[(NSPopUpButton*)view setTarget:self];
			[(NSPopUpButton*)view setAction:@selector(listButtonChanged:)];
		}
		else if([view isKindOfClass:[NSSegmentedControl class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[(NSSegmentedControl*)view setTarget:self];
			[(NSSegmentedControl*)view setAction:@selector(segmentedControlChanged:)];
		}
		else if([view isKindOfClass:[NSButton class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[(NSButton*)view setTarget:self];
			[(NSButton*)view setAction:@selector(buttonChanged:)];
		}
		else if([view isKindOfClass:[NSColorWell class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[(NSColorWell*)view setTarget:self];
			[(NSColorWell*)view setAction:@selector(colorChanged:)];
		}
		else if([view isKindOfClass:[NSMatrix class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[(NSMatrix*)view setTarget:self];
			[(NSMatrix*)view setAction:@selector(matrixChanged:)];
		}
		else if([view isKindOfClass:[NSTabView class]]) {
			NSArray* tabs = [(NSTabView*)view tabViewItems];
			for(NSTabViewItem* tab_view in tabs) {
				[self fillSubviews:[tab_view view] fill:array];
			}
		}
		else {
			[self fillSubviews:view fill:array];
		}
	}
}

- (void) segmentedControlChanged:(NSSegmentedControl*)sender {
	NSInteger dx = [sender selectedSegment];
	const char* varname = [[sender toolTip]UTF8String];

	OSCMessage msg;
	msg.addInt32(OSCU_MATRIX);
	msg.addString(varname);
	msg.addInt32((int32_t)dx);
	osc_sender->sendMessage(msg);
}

- (void) listButtonChanged:(NSPopUpButton*)sender {
	NSInteger dx = [sender indexOfSelectedItem];
	const char* varname = [[sender toolTip]UTF8String];
	
	OSCMessage msg;
	msg.addInt32(OSCU_MATRIX);
	msg.addString(varname);
	msg.addInt32((int32_t)dx);
	osc_sender->sendMessage(msg);
}

- (void) matrixChanged:(NSMatrix*)sender {
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
	const char* varname = [[sender toolTip]UTF8String];
	[self sendSliderChanged:varname sender:sender];	
}

- (void) buttonChanged:(NSButton*)button {
	const char* varname = [[button toolTip]UTF8String];
 	[self sendButtonChanged:varname sender:button];
}


- (void) colorChanged:(NSColorWell*)sender {
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

- (void) sendSliderChanged:(const char*)varname sender:(NSSlider*) sender {
	OSCMessage msg;
	msg.addInt32(OSCU_SLIDER);
	msg.addString(varname);
	msg.addFloat([sender floatValue]);
	osc_sender->sendMessage(msg);
}

- (void) sendTextfieldChanged:(const char*) varname sender:(NSTextField*)sender {
	OSCMessage msg;
	msg.addInt32(OSCU_STRING);
	msg.addString(varname);
	msg.addString([[sender stringValue] UTF8String]);
	osc_sender->sendMessage(msg);
}

- (void) sendButtonChanged:(const char*)varname sender:(NSButton*) button {
	if([[button cell] showsStateBy] != NSNoCellMask) {
		// toggle button
		int on = [button state] == NSOnState;
		OSCMessage msg;
		msg.addInt32(OSCU_BUTTON);
		msg.addString(varname);
		msg.addInt32(on);
		osc_sender->sendMessage(msg);
	}
	else {
		// push button.
		OSCMessage msg;
		msg.addInt32(OSCU_CALLBACK);
		msg.addString(varname);
		osc_sender->sendMessage(msg);
	}

}

@end
