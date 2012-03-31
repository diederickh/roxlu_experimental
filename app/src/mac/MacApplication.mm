#import "MacApplication.h"

@implementation MacApplication

- (void) setApplication:(Application*)application {
	app = application;
}

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
