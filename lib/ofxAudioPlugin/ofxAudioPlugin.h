/*
 *  ofxAudioPlugin.h
 *
 *  Created by Marek Bereza on 15/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#pragma once
#include "ofxAudioPluginWrapper.h"

// these are the types of parameter you can have.
#define ofxAudioPlugin_INT 1
#define ofxAudioPlugin_FLOAT 2
#define ofxAudioPlugin_INDEXED 3


#include <string>
using namespace std;

struct ofxAudioPluginParameterInfo {
	float min;
	float max;
	float initial;
	int type;
	string name;
};

class ofxAudioPlugin {
public:
	
	// return the number of parameters you have here
	virtual int getNumParameters() {
		return 0;
	}
	
	// populate the incoming parameter info here about each of your parameters
	virtual void getParameterInfo(int parameterId, ofxAudioPluginParameterInfo *info) {
		
	}
	
	// if using an indexed parameter, set the name here
	virtual string getParameterValueString(int parameterId, int index) {
		return "";
	}
	
	// implement this if you want to respond to midi notes.
	virtual void noteIn(int noteNum, int velocity) {}
	
	// midi control change
	virtual void ctlIn(int ctlNum, int value) {}
	
	//program change
	virtual void pgmIn(int value) {}
	
	// midi pitch bend
	virtual void bendIn(int value) {}
	
	// aftertouch
	virtual void touchIn(int value) {}
	
	virtual void polyTouchIn(int note, int value) {}
	
	
	virtual void process(int numIns, float **ins, int numOuts, float **outs, int numFrames) {
	}
	
	void setNumParameters(int _numParams);
	
	
	void parametersChanged();
	
	void setParameter(int parameterId, float value);
	float getParameter(int parameterId);
};

// you have to implement this and return an instance of your plugin for everything to work.
ofxAudioPlugin *ofxAudioPlugin_getPlugin();
