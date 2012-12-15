#import "AppViewController.h"

@implementation AppViewController

- (id) initWithFrame:(CGRect) rect {
	self = [super init];
	if(self) {
		gl_view = [[EAGLView alloc] initWithFrame: rect];
		self.view = gl_view;
	}
	return self;
}
- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)viewDidUnload {
    [super viewDidUnload];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated {
	[gl_view startAnimation];	
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated {
	[gl_view stopAnimation];	
	[super viewDidDisappear:animated];
}

@end
