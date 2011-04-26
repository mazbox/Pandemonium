#include "ofxAudioPlugin.h"

#include "ofxAudioPluginConfig.h"
#ifdef ofxAudioPlugin_IsSynth

#endif
//#include "Logger.h"

#include "ofxAudioPlugin.h"
#include "ofxAudioPluginWrapper.h"

#include <fstream>
#include "audioeffectx.h"

#ifdef __GNUC__
#define __cdecl
#endif


class ofxVSTPlugin: public AudioEffectX, public ofxAudioPluginWrapper {
	
public:
	ofxVSTPlugin(audioMasterCallback audioMaster): AudioEffectX (audioMaster, 1, 1) {

		plugin = ofxAudioPlugin_getPlugin(this);
		
		
#ifdef ofxAudioPlugin_IsSynth
		setNumInputs(0);
		isSynth();
#else
		setNumInputs (2);		// stereo in
#endif
		setNumOutputs (2);		// stereo out
		setUniqueID (ofxAudioPlugin_PluginCode);	// identify
		canProcessReplacing ();	// supports replacing output

		
		fGain = 1.f;			// default to 0 dB
		vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name
		
		cEffect.numPrograms = 3;
		
		
		cEffect.numParams = plugin->getNumParameters();
		cEffect.numParams = 1;
		
		parametersChanged();
	}
	
	void setNumParameters(int numParams) {
		
	}
	
	void parametersChanged() {
		updateDisplay();
	}
	
	// Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames){
		float* in1  =  inputs[0];
		float* in2  =  inputs[1];
		float* out1 = outputs[0];
		float* out2 = outputs[1];
		
		while (--sampleFrames >= 0)
		{
			(*out1++) = (*in1++) * fGain;
			(*out2++) = (*in2++) * fGain;
		}
	}
	
	
	
	

	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text) {
		if(index==0) {
			vst_strncpy(text, "zero", kVstMaxProgNameLen);
			return true;
		} else if(index==1) {
			vst_strncpy(text, "one", kVstMaxProgNameLen);
			return true;
		} else if(index==2) {
			vst_strncpy(text, "two", kVstMaxProgNameLen);
			return true;
		}
		return false;
	} 
	
	
	// Parameters
	virtual void setParameter (VstInt32 index, float value){
		fGain = value;
	}
	virtual float getParameter (VstInt32 index){
		return fGain;
	}
	
	virtual void getParameterLabel (VstInt32 index, char* label){
		vst_strncpy (label, "", kVstMaxParamStrLen);
	}
	
	virtual void getParameterDisplay (VstInt32 index, char* text){
		dB2string (fGain, text, kVstMaxParamStrLen);
	}
	
	virtual void getParameterName (VstInt32 index, char* label){
		vst_strncpy (label, "Gain", kVstMaxParamStrLen);
	}
	
	virtual bool getEffectName (char* name){
		vst_strncpy (name, ofxAudioPlugin_Name, kVstMaxEffectNameLen);
		return true;
	}
	virtual bool getVendorString (char* text){
		vst_strncpy (text, ofxAudioPlugin_Manufacturer, kVstMaxVendorStrLen);
		return true;
	}
	virtual bool getProductString (char* text){
		vst_strncpy (text, ofxAudioPlugin_Name, kVstMaxProductStrLen);
		return true;
	}
	virtual VstInt32 getVendorVersion (){ 
		return 1000; 
	}
	
	
	
protected:
	float fGain;
	char programName[kVstMaxProgNameLen + 1];
	ofxAudioPlugin *plugin;
};

float ofxAudioPlugin::getParameter(int parameterId) {

}
void ofxAudioPlugin::setNumParameters(int numParameters) {
}

void ofxAudioPlugin::setParameter(int parameterId, float value) {
}

void ofxAudioPlugin::parametersChanged() {

}
//-------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ofxVSTPlugin (audioMaster);
}

