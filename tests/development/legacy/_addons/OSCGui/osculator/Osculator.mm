#import "Osculator.h"

@implementation Osculator

												
- (void) generateFromWindow:(NSWindow*)inWindow senderPort:(int) sp receiverPort:(int) rp { 
	osc_sender = new OSCSender("localhost",sp);
	osc_receiver = new OSCReceiver(rp);
	window = inWindow;
	views = [NSMutableDictionary dictionary];
	
	[self fillSubviews:[window contentView] fill:views];
	[self loadSettings];
	[NSTimer scheduledTimerWithTimeInterval:0.25 target:self selector:@selector(update:) userInfo:views repeats:YES];
}

- (void) loadSettings {
	[self sendCommand:OSCU_COMMAND_LOAD_SETTINGS];
}

- (void) sendCommand:(int)command {
	OSCMessage msg;
	msg.addInt32(OSCU_COMMAND);
	msg.addInt32((int32_t)command);
	osc_sender->sendMessage(msg);
}

- (void) update:(NSTimer*)timer {
	if(osc_receiver->hasMessages()) {
		OSCMessage m;
		while(osc_receiver->getNextMessage(&m)) {
			uint32_t type = m.getInt32(0);
			if(type == OSCU_COMMAND) {
				uint32_t cmd = m.getInt32(1);
				
				// load settings.
				if(cmd == OSCU_COMMAND_LOAD_SETTINGS) {
					size_t num = m.getNumArgs();
					for(int i = 2; i < num; i += 2) {
						string el_name = m.getString(i);
						string el_val = m.getString(i+1);
						NSString* k = [NSString stringWithUTF8String:el_name.c_str()];
						NSString* v = [NSString stringWithUTF8String:el_val.c_str()];
						NSControl* view = [[timer userInfo] objectForKey:k];
						[view setStringValue:v];
					}
				}
			}
		}
	}
	
}

- (void) fillSubviews:(NSView*) parent fill:(NSMutableDictionary*)container {
	for(NSView* view in [parent subviews]) {
	
		if([view isKindOfClass:[NSSlider class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[container setObject:view forKey:[view toolTip]];
			[(NSSlider*)view setTarget:self];
			[((NSSlider*)view) setAction:@selector(sliderChanged:)];
		}
		else if([view isKindOfClass:[NSPopUpButton class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[container setObject:view forKey:[view toolTip]];
			[(NSPopUpButton*)view setTarget:self];
			[(NSPopUpButton*)view setAction:@selector(listButtonChanged:)];
		}
		else if([view isKindOfClass:[NSSegmentedControl class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[container setObject:view forKey:[view toolTip]];
			[(NSSegmentedControl*)view setTarget:self];
			[(NSSegmentedControl*)view setAction:@selector(segmentedControlChanged:)];
		}
		else if([view isKindOfClass:[NSButton class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[container setObject:view forKey:[view toolTip]];
			[(NSButton*)view setTarget:self];
			[(NSButton*)view setAction:@selector(buttonChanged:)];
		}
		else if([view isKindOfClass:[NSColorWell class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[container setObject:view forKey:[view toolTip]];
			[(NSColorWell*)view setTarget:self];
			[(NSColorWell*)view setAction:@selector(colorChanged:)];
		}
		else if([view isKindOfClass:[NSMatrix class]]) {
			if([view toolTip] == NULL) {
				continue;
			}
			[container setObject:view forKey:[view toolTip]];
			[(NSMatrix*)view setTarget:self];
			[(NSMatrix*)view setAction:@selector(matrixChanged:)];
		}
		else if([view isKindOfClass:[NSTabView class]]) {
			NSArray* tabs = [(NSTabView*)view tabViewItems];
			for(NSTabViewItem* tab_view in tabs) {
				[self fillSubviews:[tab_view view] fill:container];
			}
		}
		else {
			[self fillSubviews:view fill:container];
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
