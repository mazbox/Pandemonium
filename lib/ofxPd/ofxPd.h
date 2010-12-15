#pragma once

#include <string>
using namespace std;


class ofxPd {
public:
	ofxPd();
	
	void getDirAndFile(const char *path, char *outDir, char *outFile);
	int getBlockSize() { return blockSize; } 
	
	
	void setBlockSize(int bufferSize);	
	void load(string patchFile, int inputs, int outputs, int samplerate, int bufferSize);
	
	// returns whether this patch has an audio input
	bool hasInput();
	
	~ofxPd();
	void sendFloat(string messageName, float value);
	void sendBang(string messageName);	
	void sendMidiNote(int channel, int noteNum, int velocity, int blockOffset = 0);
	void sendMidiControlChange(int channel, int ctlNum, int value);
	void sendMidiBend(int channel, int value);
	void sendMidiProgramChange(int channel, int program);
	void sendMidiPolyTouch(int channel, int noteNum, int value);
	void sendMidiAfterTouch(int channel, int value);

	void process(float *input, float *output, int frameCount);
	void processNonInterleaved(float *input, float *output, int frameCount);
	
	bool isRunning() { return running; }
	
	void openPatch(string file, string dir);
	void closePatch(string file, string dir);
	
	
private:
	void stopPd();
	void startPd();
	

	int blockSize;
	int numInputChannels;
	int numOutputChannels;
	int sampleRate;
	string path;
	
	string currFile;
	string currDir;
	bool patchIsOpen;
	bool hasADC;
	bool running;
};
