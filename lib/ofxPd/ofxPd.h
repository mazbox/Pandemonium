//
//  ofxPd.h
//  audioOutputExample
//
//  Created by Marek Bereza on 19/04/2011.
//
#pragma once

#include <string>
#include <vector>
using namespace std;
/**
 * TODO:
 * make different patches receive the same message name from different inputs
 */

class ofxPd {
    
public:
    ofxPd();
	~ofxPd();
	
    // this must be called before anything else!
    static void setup(int samplerate);

    // need to give this an absolute path to the file
    bool open(string patchFile);
    
	void close();
	
	// processing is always in stereo for now, numFrames is how
	// many stereo samples we're using.
	void process(float *inputs, float *outputs, int numFrames);
	
	
	void sendFloat(string messageName, float value);
	void sendBang(string messageName);	
	void sendMidiNote(int channel, int noteNum, int velocity, int blockOffset = 0);
	void sendMidiControlChange(int channel, int ctlNum, int value);
	void sendMidiBend(int channel, int value);
	void sendMidiProgramChange(int channel, int program);
	void sendMidiPolyTouch(int channel, int noteNum, int value);
	void sendMidiAfterTouch(int channel, int value);
	
	
	// stuff for pandemonium - so you can rename parameters to have unique names
	void clearParamList();
	void addParam(string param);
private:
    
	bool opened;
	
    // how many 64 sample chunks to process at one time.
    string patchFile;
    string folder;
	string file;
	
	// must be a multiple of 64!
	static void setBufferSize(int bufferSize);
    
    // static variables
    static int bufferSize;
    static bool pdInitialized;
	
	void *patchPointer;
	
	// string reference to the 
	// patch's switch~ receive.
	string uid;
	
	// this creates a new file in the same folder
	// as the original patch, but it adds the 
	// switch~ code and a unique filename.
	void createUniquePatch();
	
	// this renames any receives that are actually parameters
	// in order to keep the patch unique - so if there's multiple
	// instances we can differentiate.
	void renameReceives(string &contents);
	
	string parseFileToString(string filePath);
	void processForConnects(string &line);
	void stringToFile(string filePath, string contents);
	vector<string> params;
};
