/*
 *  Pandemonium.cpp
 *  Pandemonium
 *
 *  Created by Marek Bereza on 15/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#include "Pandemonium.h"

ofxAudioPlugin *ofxAudioPlugin_getPlugin() {
	return new Pandemonium();
}