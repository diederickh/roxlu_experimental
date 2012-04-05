#import "MacWindow.h"

@implementation MacWindow


// Initialization
- (void) setApplication:(Application*)application {
	app = application;
	[self initializeKeyMap];
}

- (void) initializeKeyMap {
	for(int i=0; i < 512; i++) {
		app->modifier_keys[i] = 0;	
	}
	
	key_map[0x00] = KEY_a;
	key_map[0x01] = KEY_s;
	key_map[0x02] = KEY_d;
	key_map[0x03] = KEY_f;
	key_map[0x04] = KEY_g;
	key_map[0x05] = KEY_h;
	key_map[0x06] = KEY_z;
	key_map[0x07] = KEY_x;
	key_map[0x08] = KEY_c;
	key_map[0x09] = KEY_v;
	key_map[0x0B] = KEY_b;
	key_map[0x0C] = KEY_q;
	key_map[0x0D] = KEY_w;
	key_map[0x0E] = KEY_e;
	key_map[0x0F] = KEY_r;
	key_map[0x10] = KEY_y;
	key_map[0x11] = KEY_t;
	key_map[0x12] = KEY_1;
	key_map[0x13] = KEY_2;
	key_map[0x14] = KEY_3;
	key_map[0x15] = KEY_4;
	key_map[0x16] = KEY_6;
	key_map[0x17] = KEY_5;
	key_map[0x18] = KEY_EQUALS;
	key_map[0x19] = KEY_9;
	key_map[0x1A] = KEY_7;
	key_map[0x1B] = KEY_MINUS;
	key_map[0x1C] = KEY_8;
	key_map[0x1D] = KEY_0;
	key_map[0x1E] = KEY_RIGHTBRACKET;
	key_map[0x1F] = KEY_o;
	key_map[0x20] = KEY_u;
	key_map[0x21] = KEY_LEFTBRACKET;
	key_map[0x22] = KEY_i;
	key_map[0x23] = KEY_p;
	key_map[0x25] = KEY_l;
	key_map[0x26] = KEY_j;
	key_map[0x27] = KEY_QUOTE;
	key_map[0x28] = KEY_k;
	key_map[0x29] = KEY_SEMICOLON;
	key_map[0x2A] = KEY_BACKSLASH;
	key_map[0x2B] = KEY_COMMA;
	key_map[0x2C] = KEY_SLASH;
	key_map[0x2D] = KEY_n;
	key_map[0x2E] = KEY_m;
	key_map[0x2F] = KEY_PERIOD;
	key_map[0x41] = KEY_KP_PERIOD;
	key_map[0x43] = KEY_KP_MULTIPLY;
	key_map[0x45] = KEY_KP_PLUS;
	key_map[0x4B] = KEY_KP_DIVIDE;
	key_map[0x4C] = KEY_KP_ENTER;
	key_map[0x4E] = KEY_KP_MINUS;
	key_map[0x51] = KEY_KP_EQUALS;
	key_map[0x52] = KEY_KP0;
	key_map[0x53] = KEY_KP1;
	key_map[0x54] = KEY_KP2;
	key_map[0x55] = KEY_KP3;
	key_map[0x56] = KEY_KP4;
	key_map[0x57] = KEY_KP5;
	key_map[0x58] = KEY_KP6;
	key_map[0x59] = KEY_KP7;
	key_map[0x5B] = KEY_KP8;
	key_map[0x5C] = KEY_KP9;
	key_map[0x24] = KEY_RETURN;
	key_map[0x30] = KEY_TAB;
	key_map[0x31] = KEY_SPACE;
	key_map[0x33] = KEY_BACKSPACE;
	key_map[0x35] = KEY_ESCAPE;
	key_map[0x37] = KEY_LSUPER;
	key_map[0x36] = KEY_RSUPER;
	key_map[0x38] = KEY_LSHIFT;
	key_map[0x39] = KEY_CAPSLOCK;
	key_map[0x3A] = KEY_LALT;
	key_map[0x3D] = KEY_RALT;	
	key_map[0x3B] = KEY_LCTRL;
	key_map[0x3C] = KEY_RSHIFT;
	key_map[0x3E] = KEY_RCTRL;
	key_map[0x3F] = KEY_COMPOSE;
	key_map[0x60] = KEY_F5;
	key_map[0x61] = KEY_F6;
	key_map[0x62] = KEY_F7;
	key_map[0x63] = KEY_F3;
	key_map[0x64] = KEY_F8;
	key_map[0x65] = KEY_F9;
	key_map[0x67] = KEY_F11;
	key_map[0x69] = KEY_F13;
	key_map[0x6B] = KEY_F14;
	key_map[0x6D] = KEY_F10;
	key_map[0x6F] = KEY_F12;
	key_map[0x71] = KEY_F15;
	key_map[0x72] = KEY_HELP;
	key_map[0x73] = KEY_HOME;
	key_map[0x74] = KEY_PAGEUP;
	key_map[0x75] = KEY_DELETE;
	key_map[0x76] = KEY_F4;
	key_map[0x77] = KEY_END;
	key_map[0x78] = KEY_F2;
	key_map[0x79] = KEY_PAGEDOWN;
	key_map[0x7A] = KEY_F1;
	key_map[0x7B] = KEY_LEFT;
	key_map[0x7C] = KEY_RIGHT;
	key_map[0x7D] = KEY_DOWN;
	key_map[0x7E] = KEY_UP;
}

// Event handling
// --------------
- (void) scrollWheel: (NSEvent*) event {
	float f = [event deltaY];
	app->scrollWheel(f);
}

- (void) mouseDown: (NSEvent*) event {
	app->mouseDown([event locationInWindow].x,[event locationInWindow].y);
}

- (void) mouseUp: (NSEvent*) event {
	app->mouseDown([event locationInWindow].x,[event locationInWindow].y);
}

- (void) mouseDragged: (NSEvent*) event {
	app->mouseDragged(
		 [event locationInWindow].x
		,[event locationInWindow].y
		,[event deltaX]
		,[event deltaY]
	);
}

- (void) rightMouseDown: (NSEvent*) event {
	app->rightMouseDown([event locationInWindow].x,[event locationInWindow].y);
}

- (void) rightMouseUp: (NSEvent*) event {
	app->rightMouseUp([event locationInWindow].x,[event locationInWindow].y);
}

- (void) rightMouseDragged: (NSEvent*) event {
	app->rightMouseDragged(
		 [event locationInWindow].x
		,[event locationInWindow].y
		,[event deltaX]
		,[event deltaY]
	);
}

- (void) keyUp: (NSEvent*) event {
	app->keyUp([[event charactersIgnoringModifiers] characterAtIndex:0]);
}

- (void) keyDown: (NSEvent*) event {
	app->keyDown([[event charactersIgnoringModifiers] characterAtIndex:0]);
}

- (void) flagsChanged: (NSEvent*) event {
	int k = [event keyCode];
	app->modifier_keys[key_map[k]] = (app->modifier_keys[key_map[k]] == 0) ? 1 : 0;
}

- (void) setupApp:(float)width height:(float)height {
	app->setup(width, height);
}

- (void) updateApp {
	app->update();
}


- (Application*) getApp {
	return app;
}

- (void) drawApp {
	app->draw();
}

- (void) resizedApp:(float) width height:(float)height {
	app->resized(width,height);
}

@end
