//
//  ParameterTableController.h
//  ZG Importer
//
//  Created by Marek Bereza on 11/07/2010.
//  Copyright 2010 Marek Bereza. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <string>
#include "PdPluginParameters.h"
using namespace std;

@interface ParameterTableController :  NSObject <NSTableViewDataSource> {
	NSTableView *parameterTable;
	PdPluginParameters params;
}

- (id) initWithParameterTable:(NSTableView *)tv;
- (void) setPatch:(string)path;

@property (assign) IBOutlet NSTableView *parameterTable;


@end
