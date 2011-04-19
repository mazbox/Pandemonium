/*
 *  ZGPlugin.h
 *  ZenGarden
 *
 *  Created by Marek Bereza on 08/07/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#include "ofxDirList.h"

#include "ofxPd.h"
#include "ofxFileUtils.h"
#include <Carbon/Carbon.h>
#include "PdPluginParameters.h"
#include <AudioUnit/AudioUnit.h>
#include "ofxAudioPluginConfig.h"

#pragma once

class ZGPlugin {
public:
	
	ZGPlugin() {
		lastBufferSize = 256;
#ifdef ofxAudioPlugin_IsSynth	
		findPatches("/Library/Audio/Sounds/Pandemonium/Instruments");
#else
		findPatches("/Library/Audio/Sounds/Pandemonium/Effects");
#endif
	

		pd = new ofxPd();
		pd->setup(44100);
		running = true;
		
	}
	
	
	~ZGPlugin() {
		running = false;
		delete pd;
	}
	
	int getAudioUnitUnit(string type, string unit) {
		
		
		if(type=="bang") {
			return kAudioUnitParameterUnit_Boolean;
		}
		

		
		if(unit=="percent") {
			return kAudioUnitParameterUnit_Percent;
		}
		
		if(unit=="indexed") {
			return kAudioUnitParameterUnit_Indexed;
		}
		
		if(unit=="hz") {
			return kAudioUnitParameterUnit_Hertz;
		}
		
		if(type=="int") {
			return kAudioUnitParameterUnit_Indexed;
		} else {
			return kAudioUnitParameterUnit_Generic;
		}
		
	}
	
	
	void getBundlePath(CFStringRef bundleId, char *path) {
		CFBundleRef bundle = CFBundleGetBundleWithIdentifier(bundleId);
		CFURLRef resourcesURL = CFBundleCopyBundleURL(bundle);
		
		CFStringRef pathCF = CFURLGetString(resourcesURL);
		
		char *pathStr = (char*)malloc(CFStringGetLength(pathCF)+1);
		CFStringGetCString (
							pathCF,
							pathStr,
							CFStringGetLength(pathCF)+1,
							kCFStringEncodingASCII
							);
		
		CFRelease(resourcesURL);
		
		if(strstr(pathStr, "file://localhost")==pathStr) {
			strcpy(path, pathStr+16);
		} else {
			strcpy(path, pathStr);
		}
		
		free(pathStr);
	}
	
	
	void loadPatchFromDir(string dir) {
		// just find out what the name of the dir is and then append it to the end with a .pd
		// eg /path/to/file
		// turns into 
		// /path/to/file/file.pd
		
		string filename = ofxFileName(dir);
		
		printf("path: %s\nfilename: %s\n", dir.c_str(), filename.c_str());
		dir = dir + "/" + filename + ".pd";
		printf("File: %s\n", dir.c_str());
		pd->open(dir);
	}
	
	void findPatches(string dir = "") {
		if(dir!="") {
			dirPath = dir;
		}
		patchPaths.clear();
		
		
		printf("Trying to open %s\n", dir.c_str());
		ofxDirList DIR;
		int length = DIR.listDir(dirPath);
		
		for(int i = 0; i < length; i++) {
			if(ofxIsDirectory(DIR.getPath(i))) {
				patchPaths.push_back(DIR.getPath(i));
			}
		}
	}
protected: 
	
	ofxPd *pd;
	string dirPath;
	PdPluginParameters params;
	vector<string> patchPaths;
	float inBuff[22050];
	float outBuff[22050];
	bool running;
	int lastBufferSize;
	int currPatchIndex;
	
};