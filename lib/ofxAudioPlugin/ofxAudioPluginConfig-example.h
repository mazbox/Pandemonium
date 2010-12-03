/*
 *  ofxAudioPluginConfig.h
 *  Pandemonium
 *
 *  Created by Marek Bereza on 15/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#define ofxAudioPlugin_Name                 "Pandemonium"
#define ofxAudioPlugin_Desc                 "Pandemonium Effect"
#define ofxAudioPlugin_Manufacturer         "MAZBOX.COM"
#define ofxAudioPlugin_ManufacturerCode     'MazB'
#define ofxAudioPlugin_PluginCode           'PanE'
#define ofxAudioPlugin_MaxNumInputChannels  2
#define ofxAudioPlugin_MaxNumOutputChannels 2
#define ofxAudioPlugin_PreferredChannelConfigurations   {1, 1}, {2, 2}
#define ofxAudioPlugin_IsSynth              0
#define ofxAudioPlugin_WantsMidiInput       0
#define ofxAudioPlugin_ProducesMidiOutput   0
#define ofxAudioPlugin_SilenceInProducesSilenceOut  0
#define ofxAudioPlugin_TailLengthSeconds    0
#define ofxAudioPlugin_EditorRequiresKeyboardFocus  1
#define ofxAudioPlugin_VersionCode          0x10000
#define ofxAudioPlugin_VersionString        "1.0.0"
#define ofxAudioPlugin_VSTUniqueID          ofxAudioPlugin_PluginCode
#define ofxAudioPlugin_VSTCategory          kPlugCategEffect
#define ofxAudioPlugin_AUMainType           kAudioUnitType_Effect
#define ofxAudioPlugin_AUSubType            ofxAudioPlugin_PluginCode
#define ofxAudioPlugin_AUExportPrefix       PandemoniumAU
#define ofxAudioPlugin_AUExportPrefixQuoted "PandemoniumAU"
#define ofxAudioPlugin_AUManufacturerCode   ofxAudioPlugin_ManufacturerCode
#define ofxAudioPlugin_CFBundleIdentifier   com.mazbox.pandemonium
