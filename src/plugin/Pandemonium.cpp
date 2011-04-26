/*
 *  Pandemonium.cpp
 *  Pandemonium
 *
 *  Created by Marek Bereza on 15/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#include "Pandemonium.h"

Mutex Pandemonium::takeItInTurnsMutex;

ofxAudioPlugin *ofxAudioPlugin_getPlugin(ofxAudioPluginWrapper *wrapper) {
	return new Pandemonium(wrapper);
}