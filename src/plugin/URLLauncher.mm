//
//  URLLauncher.m
//  ZenGarden
//
//  Created by Marek Bereza on 08/08/2010.
//  Copyright 2010 Marek Bereza. All rights reserved.
//

#import "URLLauncher.h"

#import <Cocoa/Cocoa.h>




@interface URLLauncher : NSObject {
	
}
- launchURL;
- launchURLMainThread;
@end



@implementation URLLauncher


- launchURLMainThread {
	NSLog(@"Launching url main thread ");
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.google.com/"]];
	//URLLauncher *l = nil;
	//[l launchURL];
}


- launchURL {
	NSLog(@"Launching url");
	[self performSelectorOnMainThread:@selector(launchURLMainThread) withObject:nil waitUntilDone:NO];
}
@end


void launchURL() {
	URLLauncher *l = [[[URLLauncher alloc] init] autorelease];
	[l launchURL];
}