//
//  ParameterTableController.m
//  ZG Importer
//
//  Created by Marek Bereza on 11/07/2010.
//  Copyright 2010 Marek Bereza. All rights reserved.
//

#import "ParameterTableController.h"


@implementation ParameterTableController

@synthesize parameterTable;


- (id) init {
	if(self = [super init]) {

		
	}
	return self;
}






- (int)numberOfRowsInTableView:(NSTableView *)tableView {
    return params.size();
}

- (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
			row:(int)row
{
    return @"Yo";
}



@end
