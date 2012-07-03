#import "OsculatoriOS.h"

@implementation OsculatoriOS

- (void) generateFromWindow:(UIWindow*) window {
//	[self fillSubviews:[window contentView] fill:views];
	osc_sender = new OSCSender("localhost",4444);
	
	views = [NSMutableArray array];
	[self fillSubviews:[window subviews] fill:views];
}

- (void) fillSubviews:(NSArray*) subviews fill:(NSMutableArray*) array {
	for(UIView* view in subviews) {
		NSLog(@"view ");
		if([view isKindOfClass:[UISlider class]]) {
			NSLog(@"UI SLIDER");
			UISlider* slider = (UISlider*)view;
			[slider addTarget:self action:@selector(sliderChanged:) forControlEvents:UIControlEventValueChanged];	
			//[(UISlider*)view setTarget:self];
			//[((UISlider*)view) setAction:@selector(sliderChanged:)];
		}
		[self fillSubviews:[view subviews] fill:array];
	}
}

- (void) sliderChanged:(UISlider*) sender {
	NSLog(@"UISLIDER: %f", sender.value);
	//NSLog(@"LABEL %@", [sender label]);
}


@end
