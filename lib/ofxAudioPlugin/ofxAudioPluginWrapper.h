/*
 *  ofxAudioPluginWrapper.h
 *  Pandemonium
 *
 *  Created by Marek Bereza on 15/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */
#pragma once
class ofxAudioPluginWrapper {
public:
	virtual void setNumParameters(int numParams) = 0;
	virtual void setParameter(int parameterId, float value) = 0;
	virtual float getParameter(int parameterId) = 0;
	virtual void parametersChanged() = 0;
	static ofxAudioPluginWrapper *instance;
	
};