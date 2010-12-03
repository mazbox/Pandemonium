#include <AudioUnit/AudioUnit.r>

#include "ofxAudioPluginConfig.h"



#define RES_ID			1000
#define COMP_TYPE		ofxAudioPlugin_AUMainType
#define COMP_SUBTYPE	ofxAudioPlugin_AUSubType
#define COMP_MANUF		ofxAudioPlugin_AUManufacturerCode	

#define VERSION			ofxAudioPlugin_VersionCode
#define NAME			ofxAudioPlugin_Manufacturer ": " ofxAudioPlugin_Name
#define DESCRIPTION		ofxAudioPlugin_Desc
#define ENTRY_POINT		"ofxAudioUnitEntry"

#include "AUResources.r"