//
//  ZG_ImporterAppDelegate.m
//  ZG Importer
//
//  Created by Marek Bereza on 08/05/2010.
//  Copyright 2010 Marek Bereza. All rights reserved.
//

#import "ZG_ImporterAppDelegate.h"
#include "ZGAUUtils.h"


@implementation ZG_ImporterAppDelegate

@synthesize window, patches, typeSelector, helpWindow;//, parametersWindow;//, parameterTable;

- (id) init {
	if(self = [super init]) {
		p = [[NSMutableArray alloc] initWithCapacity:2];
		initDirs();
		[self reloadPatches:EFFECT];
	}
	return self;
}


-(void) setPatch:(string)path {
	params.load((char*)path.c_str());
	[parameterTable reloadData];

	const char *cStr = patchNameFromPath(path).c_str();
	/*[parametersWindow setTitle: 
	 [NSString 
	  stringWithUTF8String:cStr]
	 ];*/
	
}


- (void) reloadPatches:(const char*) type {
	patchPaths = loadPatches(type);

	if(type==EFFECT) {
		[typeSelector setSelectedSegment:0];
	} else {
		[typeSelector setSelectedSegment:1];
	}
	
	[p removeAllObjects];
	for(int i = 0; i < patchPaths->size(); i++) {
		
		string path = patchPaths->at(i);
		
		string name = patchNameFromPath(path);
		[p addObject:[[NSString alloc] initWithCString:name.c_str() encoding: NSASCIIStringEncoding]];
		
	}
	[patches reloadData];
}
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
	[parameterTable setDataSource:self];
	[parameterTable reloadData];
	[patches registerForDraggedTypes: [NSArray arrayWithObject:NSFilenamesPboardType]];

	
}
- (BOOL)application:(NSApplication *)theApplication openFiles:(NSArray *)filenames {
	vector<string> files;
	for (int i = 0; i < [filenames count]; i++) {
		NSLog(@"%@", [filenames objectAtIndex:i]);
		NSString *path = [[filenames objectAtIndex:i] stringByReplacingPercentEscapesUsingEncoding:NSASCIIStringEncoding];
		const char *filePath = [path cStringUsingEncoding:NSASCIIStringEncoding];
		files.push_back(string(filePath));
	}
	
	if(files.size()>0) {
		const char *type = installFiles(&files);
		if(type!=NULL) {
			[self reloadPatches:type];
		} else {
			printf("invalid files\n");
			[self alertBadDrop ];
		}
	}
	//[NSApp terminate: nil];
	//NSLog(@"%@", filename);
	//const char *filePath = [filename cStringUsingEncoding:NSASCIIStringEncoding];
	//const char *type = installPatch(filePath);
	//[self reloadPatches:type];

	return YES;
}


-(BOOL)tableView:(NSTableView*)aTableView
		acceptDrop:(id<NSDraggingInfo>)info 
row:(int)row dropOperation:(NSTableViewDropOperation)operation {

	NSPasteboard *pb = [info draggingPasteboard];
	NSArray *patchList = [pb pasteboardItems];
	vector<string> files;
	if(patchList==nil) return NO;
	int i;
	for(i = 0; i < [patchList count]; i++) {
		NSLog(@"%d", i);
		NSPasteboardItem *pbi = [patchList objectAtIndex:i];
		NSArray *arr = [pbi types];
		if([arr count]>0) {
			NSString *url = [[pbi stringForType:[arr objectAtIndex:0]] stringByReplacingPercentEscapesUsingEncoding:NSASCIIStringEncoding];
			NSLog(@"%@", url);
			
			const char *filePath = [url cStringUsingEncoding:NSASCIIStringEncoding];
			files.push_back(string(filePath));
		}
	}
	
	if(files.size()>0) {
		const char *type = installFiles(&files);
		if(type!=NULL) {
			[self reloadPatches:type];
		} else {
			printf("invalid files\n");
			[self alertBadDrop];
		}
	}
	return YES;

}
-(void) alertBadDrop {
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	[alert addButtonWithTitle:@"OK"];
	[alert setMessageText:@"The file or folder was invalid"];
	[alert setInformativeText:@"Maybe you dropped a file with no dac~ or a folder that doesn't contain a patch of the same name!"];
	[alert setAlertStyle:NSWarningAlertStyle];
	[alert beginSheetModalForWindow:window modalDelegate:nil didEndSelector:nil contextInfo:nil];
}

-(NSDragOperation)tableView:(NSTableView*)tableView
validateDrop:(id <NSDraggingInfo>)info
proposedRow:(int)row
proposedDropOperation:(NSTableViewDropOperation)operation {

}


- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	if(tableView==parameterTable) {
		return params.size();
	} else {
		return [p count];
	}
}

- (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
			row:(int)row
{
	if(tableView==parameterTable) {
		ZGParameter *param = params.getParameter(row);
		if([[tableColumn identifier] isEqualToString:  @"name" ]) {
			return [NSString stringWithUTF8String:param->name.c_str()];
		} else if([[tableColumn identifier] isEqualToString:  @"default" ]) {
			return [NSString stringWithFormat:@"%3.2f", param->value];
		} else if([[tableColumn identifier] isEqualToString:  @"min" ]) {
			return [NSString stringWithFormat:@"%3.2f", param->min];
		} else if([[tableColumn identifier] isEqualToString:  @"max" ]) {
			return [NSString stringWithFormat:@"%3.2f", param->max];
		} else {
			return @"none";
		}
	} else {
		return [p objectAtIndex:row];
	}
}


- (BOOL)tableView:(NSTableView *)aTable shouldSelectRow:(int)aRow
{
	
	
	[self setPatch:patchPaths->at(aRow)];
	
	return YES;
}



- (IBAction) deletePressed:(id) sender {
	int patchIndexToDelete = [patches selectedRow];
	if(patchIndexToDelete!=-1) {
		uninstallPatch(patchPaths->at(patchIndexToDelete).c_str());
		if([typeSelector selectedSegment]==0) {
			[self reloadPatches:EFFECT];
		} else {
			[self reloadPatches:INSTRUMENT];
		}
	}
	
}

- (IBAction) parametersPressed:(id) sender {
	//[parametersWindow makeKeyAndOrderFront:sender];
}

- (IBAction) helpPressed:(id) sender {
	[helpWindow makeKeyAndOrderFront:sender];

}
- (IBAction) typeChanged:(id) sender {
	if([typeSelector selectedSegment]==0) {
		[self reloadPatches:EFFECT];
	} else {
		[self reloadPatches:INSTRUMENT];
	}
	// update params to reflect new selection
	if(patchPaths->size()>0) {
		int index = [patches selectedRow];
		if(index!=-1) {
			[self setPatch:patchPaths->at(index)];
		}
	}

}

@end
