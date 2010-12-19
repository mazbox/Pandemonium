/*
 *  ofxPd.cpp
 *  PdPlayer
 *
 *  Created by Marek Bereza on 11/04/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */
#include <fstream>
#include "ofxPd.h"
#include "ofxFileUtils.h"

#include "z_libpd.h"
#include "Logger.h"
#include "Slots.h"


extern "C" {
	void ofxPd_printhook(const char *ptr) {
		LOG(ptr)
	}
};


class PdInstance {
public:
	
	
	PdInstance() {
		libpd_init();
		libpd_clear_search_path();
		int numInputs = 2;
		int numOutputs = 2;
		int samplerate = 44100;
		int blockSize = 256;
		libpd_init_audio(numInputs, numOutputs, samplerate, blockSize/libpd_blocksize());
	}
	
		
	
	
	static PdInstance *getInstance() {
		static PdInstance *pd = NULL;
		if(pd==NULL) {
			printf("Creating pd\n");
			pd = new PdInstance();
			
		}
		return pd;
	}
	Slots audioIns;
	Slots audioOuts;
	Slots midiChannels;
	
	
};



ofxPd::ofxPd() {
	char strin[512];
	sprintf(strin, "%x", (int)this);
	string ide = strin;
	INIT_LOG("/Users/marek/Desktop/Pandemonium "+ide+" Log.txt")
	LOG("ofxPd::ofxPd")
	
	hasADC = false;
	patchIsOpen = false;
	
	pd = PdInstance::getInstance();

	audioIns = new int[2];
	audioOuts = new int[2];
	audioIns[0] = pd->audioIns.getSlot();
	audioIns[1] = pd->audioIns.getSlot();
	
	audioOuts[0] = pd->audioOuts.getSlot();
	audioOuts[1] = pd->audioOuts.getSlot();
	
	midiChannel = pd->midiChannels.getSlot();
	
	pd->audioIns.print();
	 
}

ofxPd::~ofxPd() {
	stopPd();
	pd->audioIns.releaseSlot(audioIns[0]);
	pd->audioIns.releaseSlot(audioIns[1]);
	
	pd->audioOuts.releaseSlot(audioOuts[0]);
	pd->audioOuts.releaseSlot(audioOuts[1]);

	pd->midiChannels.releaseSlot(midiChannel);
	
	LOG("ofxPd::~ofxPd");
	// TODO(mhroth): what message must be sent to pd in order to clear all canvases?
} 

 
bool ofxPd::hasInput() {
	return hasADC;
}

void ofxPd::getDirAndFile(const char *path, char *outDir, char *outFile) { 
	char *lastSlash = strrchr(path, '/'); 
	sprintf(outDir, ""); 
	if(lastSlash==NULL) { 
		sprintf(outFile, "%s", path); 
	} else { 
		strncpy(outDir, path, 1+1+lastSlash-path); 
		outDir[1+lastSlash-path] = '\0'; 
		strcpy(outFile, lastSlash+1); 
	} 
} 

void ofxPd::setBlockSize(int bufferSize) {
	if(bufferSize != blockSize) {
		blockSize = bufferSize;
		libpd_set_ticks_per_buffer(blockSize/libpd_blocksize());
	}
}

void ofxPd::load(string patchFile) {
	path = patchFile;
	
	hasADC = false;
	
	char file[256];
	char dir[512];
	getDirAndFile(patchFile.c_str(), dir, file);
	
	startPd();
	
	
	

	// look through the pd file and see if there's a adc~ in it
	string line;
	ifstream myfile (patchFile.c_str());
	if (myfile.is_open()) {
		while (! myfile.eof() ) {
			getline (myfile,line);
			if(line.find("adc~;")!=-1) {
				hasADC = true;
				break;
			}
		}
		myfile.close();
	}
	
	// at this point, rewrite the file to another file 
	// with a unique id (this objects memory location)
	// and set the channels up.
	
	openPatch(file, dir);
}
void ofxPd::closePatch(string file, string dir) {
	libpd_clear_search_path();

	libpd_start_message();

	string recvr = "pd-"+file;
	//printf("Trying to close patch %s\n", file.c_str());
	libpd_finish_message(recvr.c_str(), "menuclose");

	patchIsOpen = false;
}

void ofxPd::openPatch(string file, string dir) {

	libpd_clear_search_path();
	libpd_add_to_search_path(dir.c_str());

	currDir = dir;
	currFile = file;
	
	patchIsOpen = true;
	// open patch
	libpd_start_message();
	libpd_add_symbol(file.c_str());
	libpd_add_symbol(dir.c_str());

	libpd_finish_message("pd", "open");


}

void ofxPd::stopPd() {
	
	// send "pd dsp 0", ensure that dsp is turned off
	libpd_start_message();
	libpd_add_float(0);
	libpd_finish_message("pd", "dsp");
	
}

void ofxPd::startPd() {
	
	// send "pd dsp 1"
	libpd_start_message();
	libpd_add_float(1);
	libpd_finish_message("pd", "dsp");
}



void ofxPd::sendFloat(string receiverName, float value) {

	libpd_float(receiverName.c_str(), value);

}

void ofxPd::sendBang(string receiverName) {

	// mutex.lock();
	libpd_bang(receiverName.c_str());
	//bang(receiverName.c_str());
	// mutex.unlock();
}

void ofxPd::sendMidiNote(int channel, int noteNum, int velocity, int blockOffset) {

	// TODO(mhroth): implement for libpd
	libpd_start_message();
	libpd_add_float(noteNum);
	libpd_add_float(velocity);
	libpd_add_float(channel);
	libpd_finish_list("#notein");

}

void ofxPd::sendMidiControlChange(int channel, int ctlNum, int value) {

	libpd_start_message();
	libpd_add_float(ctlNum);
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#ctlin");
	
}
void ofxPd::sendMidiBend(int channel, int value) {

	libpd_start_message();
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#bendin");
}


void ofxPd::sendMidiAfterTouch(int channel, int value) {
	libpd_start_message();
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#touchin");
}

void ofxPd::sendMidiPolyTouch(int channel, int noteNum, int value) {
	libpd_start_message();
	libpd_add_float(noteNum);
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#polytouchin");
}

void ofxPd::sendMidiProgramChange(int channel, int program) {
	libpd_start_message();
	libpd_add_float(program);
	libpd_add_float(channel);
	libpd_finish_list("#pgmin");
}


void ofxPd::process(float *input, float *output, int frameCount) {
	setBlockSize(frameCount);
	
	
	// this function uninterleaves and interleaves the input and output buffers, respectively
	libpd_process_float(input, output);
	
	

	


}

void ofxPd::processNonInterleaved(float *input, float *output, int frameCount) {
	setBlockSize(frameCount);
	libpd_process_raw(input, output);
}


