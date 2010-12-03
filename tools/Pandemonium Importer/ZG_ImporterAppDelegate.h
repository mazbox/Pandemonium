//
//  ZG_ImporterAppDelegate.h
//  ZG Importer
//
//  Created by Marek Bereza on 08/05/2010.
//  Copyright 2010 Marek Bereza. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <vector>
#include <string>
#include "PdPluginParameters.h"

using namespace std;
//#include "ParameterTableController.h"

@interface ZG_ImporterAppDelegate : NSObject <NSApplicationDelegate,NSTableViewDataSource> {
    
	NSWindow *window;
	NSPanel *helpWindow;
//	NSPanel *parametersWindow;
	NSTableView *patches;
	NSTableView *parameterTable;
	NSSegmentedControl *typeSelector;

//	ParameterTableController *parameters;
	
	NSMutableArray *p;
	vector<string> *patchPaths;
	PdPluginParameters params;
	
}
- (void) reloadPatches:(const char*) type;
- (void) setPatch:(string)path;


- (IBAction) deletePressed:(id) sender;
- (IBAction) helpPressed:(id) sender;
- (IBAction) parametersPressed:(id) sender;
- (IBAction) typeChanged:(id) sender;
-(void) alertBadDrop;


-(BOOL)tableView:(NSTableView*)aTableView
	  acceptDrop:(id<NSDraggingInfo>)info 
			 row:(int)row dropOperation:(NSTableViewDropOperation)operation;

-(NSDragOperation)tableView:(NSTableView*)tableView
			   validateDrop:(id <NSDraggingInfo>)info
				proposedRow:(int)row
	  proposedDropOperation:(NSTableViewDropOperation)operation;

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSPanel *helpWindow;
@property (assign) IBOutlet NSPanel *parametersWindow;
@property (assign) IBOutlet NSTableView *patches;
@property (assign) IBOutlet NSTableView *parameterTable;
@property (assign) IBOutlet NSSegmentedControl *typeSelector;

@end
