/*
 *  ofxAudioPluginConfig.h
 *  Pandemonium
 *
 *  Created by Marek Bereza on 15/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#define ofxAudioPlugin_IsSynth

// this is set in the project settings
#ifdef ofxAudioPlugin_IsSynth

#define ofxAudioPlugin_Name                 "Pandemonium Instrument"
#define ofxAudioPlugin_Desc                 "Pandemonium Instrument"
#define ofxAudioPlugin_Manufacturer         "MAZBOX.COM"
#define ofxAudioPlugin_ManufacturerCode     'MazB'
#define ofxAudioPlugin_PluginCode           'PanI'
#define ofxAudioPlugin_AUMainType           kAudioUnitType_MusicDevice
#else

#define ofxAudioPlugin_Name                 "Pandemonium"
#define ofxAudioPlugin_Desc                 "Pandemonium Effect"
#define ofxAudioPlugin_Manufacturer         "MAZBOX.COM"
#define ofxAudioPlugin_ManufacturerCode     'MazB'
#define ofxAudioPlugin_PluginCode           'PanE'
#define ofxAudioPlugin_AUMainType           kAudioUnitType_Effect

#endif

#define ofxAudioPlugin_VersionCode          0x10000
#define ofxAudioPlugin_VersionString        "1.0.0"
#define ofxAudioPlugin_VSTUniqueID          ofxAudioPlugin_PluginCode
#define ofxAudioPlugin_VSTCategory          kPlugCategEffect
#define ofxAudioPlugin_AUSubType            ofxAudioPlugin_PluginCode

//#define ofxAudioPlugin_AUExportPrefix       Pandemonium
//#define ofxAudioPlugin_AUExportPrefixQuoted "Pandemonium"
#define ofxAudioPlugin_AUManufacturerCode   ofxAudioPlugin_ManufacturerCode
//#define ofxAudioPlugin_CFBundleIdentifier   com.mazbox.pandemonium
