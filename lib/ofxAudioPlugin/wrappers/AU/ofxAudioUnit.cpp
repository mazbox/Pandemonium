#include "ofxAudioPlugin.h"

#include "AUEffectBase.h"
#include "ofxAudioPluginConfig.h"
#ifdef ofxAudioPlugin_IsSynth
#include "AUInstrumentBase.h"

#endif
#include "Logger.h"
#if AU_DEBUG_DISPATCHER
#include "AUDebugDispatcher.h"
#endif
#include "ofxAudioPlugin.h"
#include "ofxAudioPluginWrapper.h"

#include <fstream>
#include "Logger.h"


class ofxAudioUnit : 

#ifdef ofxAudioPlugin_IsSynth
	public AUInstrumentBase,
#else 
	public AUEffectBase, 
#endif

public ofxAudioPluginWrapper
{
public:
	ofxAudioUnit(AudioUnit component) : 
#ifdef ofxAudioPlugin_IsSynth
	AUInstrumentBase(component, 0, 1)
#else
	AUEffectBase(component)
#endif
	{
	
		LOG("START AUDIO UNIT")
		plugin = ofxAudioPlugin_getPlugin(this);
		CreateElements();
		int numParams = plugin->getNumParameters();

		Globals()->UseIndexedParameters(numParams);
		
		for(int i = 0; i < numParams; i++) {
			ofxAudioPluginParameterInfo paramInfo;
			plugin->getParameterInfo(i, &paramInfo);
			Globals()->SetParameter(i, paramInfo.initial );
		}
        
#if AU_DEBUG_DISPATCHER
		mDebugDispatcher = new AUDebugDispatcher (this);
#endif
		
	}
	
	
#if AU_DEBUG_DISPATCHER
	virtual ~ofxAudioUnit () { delete mDebugDispatcher; }
#endif
	
	void setNumParameters(int numParams) {
		Globals()->UseIndexedParameters(numParams);
	}
	
	static float **outBuff;	
	
	
#ifdef ofxAudioPlugin_IsSynth

	
	OSStatus HandleMidiEvent (UInt8 nStatus,
                              UInt8 inChannel,
                              UInt8 inData1,
                              UInt8 inData2,
#if defined(MAC_OS_X_VERSION_10_5)
                              UInt32 inStartFrame)
#else
	long inStartFrame)
#endif
    {


		if(nStatus==128) { // noteoff
			plugin->noteIn(inData1, 0);
		} else if(nStatus==144) { // it's a note
			plugin->noteIn(inData1, inData2);
		} else if(nStatus==224) { // bend
			int bend = inData2;
			bend *= 128;
			plugin->bendIn(bend);
		} else if(nStatus==176) { // control change
			plugin->ctlIn(inData1, inData2);
		} else if(nStatus==192) {
			plugin->pgmIn(inData1);
		} else if(nStatus==208) {
			plugin->touchIn(inData1);
		} else if(nStatus==160) {
			plugin->polyTouchIn(inData1, inData2);
		}
			
		//LOG("MIDI RECEIVED: " << (int)nStatus << "      Channel: " << (int) inChannel << " DATA 1: " <<(int)inData1<< " DATA 2: " <<(int)inData2);
       

		
        return noErr;
    }
	
	
	
	
	ComponentResult StartNote (MusicDeviceInstrumentID, MusicDeviceGroupID, NoteInstanceID&, UInt32, const MusicDeviceNoteParams&) { return noErr; }
    ComponentResult StartNote (MusicDeviceInstrumentID, MusicDeviceGroupID, NoteInstanceID*, UInt32, const MusicDeviceNoteParams&) { return noErr; }
    ComponentResult StopNote (MusicDeviceGroupID, NoteInstanceID, UInt32)   { return noErr; }
	/*
	OSStatus			StartNote(		MusicDeviceInstrumentID 	inInstrument, 
												MusicDeviceGroupID 			inGroupID, 
												NoteInstanceID *			outNoteInstanceID, 
												UInt32 						inOffsetSampleFrame, 
												const MusicDeviceNoteParams &inParams) {
		
		//currNoteId++;
		// NoteInstanceID noteID = currNoteId;
		// if(outNoteInstanceID!=NULL) {
		// //printf("outNoteInstanceID: %d\n", outNoteInstanceID);
		// noteIds[currNoteId] = inParams.mPitch;
		// *outNoteInstanceID = noteID;
		// }
		 
		//noteEvents.push_back(NoteEvent((int)inParams.mPitch, (int)inParams.mVelocity, inOffsetSampleFrame, currNoteId));
		plugin->noteIn((int)inParams.mPitch, (int)inParams.mVelocity);//, inOffsetSampleFrame);
		return noErr;
	}
	
	
	OSStatus			StopNote(		MusicDeviceGroupID 			inGroupID, 
											   NoteInstanceID 				inNoteInstanceID, 
											   UInt32 						inOffsetSampleFrame) {
		//printf("Noteoff %d\n", inNoteInstanceID);
		//map<int,float>::iterator it = noteIds.find(inNoteInstanceID);
		//if(it!=noteIds.end()) {
		//noteEvents.push_back(NoteEvent((int)it->second, 0, inOffsetSampleFrame, inNoteInstanceID));
		plugin->noteIn(inNoteInstanceID, 0);//, inOffsetSampleFrame);
		
		//} else {
		//	printf("Can't find note to turn off\n");
		//}
		
		return noErr;
	}
	
	*/
	
	
	
	
	

	
	OSStatus			Render(					AudioUnitRenderActionFlags &	ioActionFlags,
											 const AudioTimeStamp &			inTimeStamp,
											 UInt32							inNumberFrames) {

		UInt32 numOutputs = Outputs().GetNumberOfElements();
		if(outBuff==NULL) {
			outBuff = new float*[2];
		}
		
		

		// only do it if we're stereo
		if(numOutputs==1) {

			AudioBufferList& bufferList = GetOutput(0)->GetBufferList();
			if(bufferList.mNumberBuffers==2) {
				//LOG("PROCESSING" );

				int buffLen = bufferList.mBuffers[0].mDataByteSize/sizeof(float);
				outBuff[0] = (float*)bufferList.mBuffers[0].mData;
				outBuff[1] = (float*)bufferList.mBuffers[1].mData;
				plugin->process(0, NULL, 2, (float**)outBuff, buffLen);
								
				
			}
		}	
		 
		return noErr;
		
		
	}
#else
		
	virtual OSStatus			ProcessBufferLists(
												   AudioUnitRenderActionFlags &	ioActionFlags,
												   const AudioBufferList &			inBuffer,
												   AudioBufferList &				outBuffer,
												   UInt32							inFramesToProcess ) 
	{
		
		// this is for v2 audiounits only
		if(inBuffer.mNumberBuffers == 2) { 	 
			float **inputs = new float*[2]; 
			inputs[0] = (float*)inBuffer.mBuffers[0].mData; 
			inputs[1] = (float*)inBuffer.mBuffers[1].mData; 
			
			float **outputs = new float*[2]; 
			outputs[0] = (float*)outBuffer.mBuffers[0].mData; 
			outputs[1] = (float*)outBuffer.mBuffers[1].mData; 
			
			plugin->process(2, inputs, 2, outputs, inFramesToProcess);
		}
		
		
		
		return noErr; 
	}
#endif
	
	virtual	OSStatus			GetParameterValueStrings(AudioUnitScope			inScope,
														 AudioUnitParameterID		inParameterID,
														 CFArrayRef *			outStrings) {
		
		ofxAudioPluginParameterInfo paramInfo;
		paramInfo.name = "";
		paramInfo.min = 0;
		paramInfo.max = 1;
		paramInfo.initial = 0;
		paramInfo.type = ofxAudioPlugin_FLOAT;

		if (outStrings == NULL) return noErr;
		plugin->getParameterInfo(inParameterID, &paramInfo);
		if(paramInfo.name=="" || paramInfo.type!=ofxAudioPlugin_INDEXED) {
			return kAudioUnitErr_InvalidProperty;
		} else {
			
			CFStringRef strings [(int)(1+paramInfo.max-paramInfo.min)];
			int j = 0;
			for(int i = paramInfo.min; i <= paramInfo.max; i++) {
				strings[j++] = CFStringCreateWithCString(kCFAllocatorDefault, plugin->getParameterValueString(inParameterID, i).c_str(), kCFStringEncodingASCII); 
			}
			
			*outStrings = CFArrayCreate (                      // 4
										NULL,
										(const void **) strings,
										j,     // 5
										NULL
										);
			return noErr;
			
			
		}
	}
    
	UInt32 SupportedNumChannels (const AUChannelInfo** outInfo)
    {
#ifdef ofxAudioPlugin_IsSynth
		//static const AUChannelInfo sChannels[2] = {  {0,1},{0,2} };
		static const AUChannelInfo sChannels[1] = {  {0,2} };
#else 
		//static const AUChannelInfo sChannels[2] = {  {1,1},{2,2} };
		static const AUChannelInfo sChannels[1] = { {2,2} };
#endif
		if (outInfo) *outInfo = sChannels;
        return 1;
    }
	
	
	
	virtual	OSStatus			GetParameterInfo(AudioUnitScope			inScope,
												 AudioUnitParameterID	inParameterID,
												 AudioUnitParameterInfo	&outParameterInfo) {

		
		outParameterInfo.flags = 	kAudioUnitParameterFlag_IsWritable
		|		kAudioUnitParameterFlag_IsReadable;
		
		
		if (inScope == kAudioUnitScope_Global) {
			
			
			ofxAudioPluginParameterInfo paramInfo;
			paramInfo.name = "";
			paramInfo.min = 0;
			paramInfo.max = 1;
			paramInfo.initial = 0;
			paramInfo.type = ofxAudioPlugin_FLOAT;
			//memset(&paramInfo,0,sizeof(ofxAudioPluginParameterInfo));
			
			plugin->getParameterInfo(inParameterID, &paramInfo);
			
			if(paramInfo.name=="") return kAudioUnitErr_InvalidParameter;
			
			
			CFStringRef name = CFStringCreateWithCString(kCFAllocatorDefault, paramInfo.name.c_str(), kCFStringEncodingASCII); 
			
			AUBase::FillInParameterName (outParameterInfo, name, false);
			
			if(paramInfo.type==ofxAudioPlugin_INT || paramInfo.type==ofxAudioPlugin_INDEXED) {
			   
			   outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
			} else {
				outParameterInfo.unit = kAudioUnitParameterUnit_Generic;
			}
			outParameterInfo.minValue = paramInfo.min;
			outParameterInfo.maxValue = paramInfo.max;
			outParameterInfo.defaultValue = paramInfo.initial;
			
			return noErr;
		
			
			
		
		
		}
		return kAudioUnitErr_InvalidParameter;
	}
    
	virtual OSStatus			GetPropertyInfo(AudioUnitPropertyID		inID,
												AudioUnitScope			inScope,
												AudioUnitElement		inElement,
												UInt32 &			outDataSize,
												Boolean	&			outWritable ) {

#ifdef ofxAudioPlugin_IsSynth		
		return AUInstrumentBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
#else
		return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
#endif
	}
	
	virtual OSStatus			GetProperty(AudioUnitPropertyID inID,
											AudioUnitScope 		inScope,
											AudioUnitElement 		inElement,
											void *			outData) {
#ifdef ofxAudioPlugin_IsSynth
		return AUInstrumentBase::GetProperty (inID, inScope, inElement, outData);
#else
		return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
#endif
	}
	
 	virtual	bool				SupportsTail () { return false; }
	
	/*! @method Version */
	virtual OSStatus		Version() { return ofxAudioPlugin_VersionCode; }
	
    
	
	void parametersChanged() {
		AudioUnitElement				inElement;
		PropertyChanged(		kAudioUnitProperty_ParameterList,
						kAudioUnitScope_Global, 
						inElement);
		PropertyChanged(		kAudioUnitProperty_ParameterInfo,
						kAudioUnitScope_Global, 
						inElement);
	}
	
	void setParameter(int parameterId, float value) {
		Globals()->SetParameter(parameterId, value);
	}
	
	float getParameter(int parameterId) {
//		if(parameterId>0) printf("this: %x, globals: %x\n", (int)this, (int)Globals());
		return Globals()->GetParameter(parameterId);
	}
protected:
	ofxAudioPlugin *plugin;
};

//~~~~~~~~~~~~
float **ofxAudioUnit::outBuff = NULL;

void ofxAudioPlugin::setNumParameters(int _numParameters) {
	wrapper->setNumParameters(_numParameters);
}

void ofxAudioPlugin::setParameter(int parameterId, float value) {
	wrapper->setParameter(parameterId, value);
}
float ofxAudioPlugin::getParameter(int parameterId) {
	return wrapper->getParameter(parameterId);
}

void ofxAudioPlugin::parametersChanged() {
	wrapper->parametersChanged();
}


COMPONENT_ENTRY(ofxAudioUnit)