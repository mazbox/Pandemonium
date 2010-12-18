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
#include "z_libpd.h"
#include "Logger.h"
extern "C" {
	void ofxPd_printhook(const char *ptr) {
		LOG(ptr)
	}
};

ofxPd::ofxPd() {
	running = false;
	hasADC = false;
	patchIsOpen = false;
	libpd_init();
	libpd_clear_search_path();
	char strin[512];
	sprintf(strin, "%x", (int)this);
	string ide = strin;
	INIT_LOG("/Users/marek/Desktop/Pandemonium "+ide+" Log.txt")
	LOG("ofxPd::ofxPd")
	
}

void ofxPd::setup(int inputs, int outputs, int samplerate, int bufferSize) {
	// mutex.lock();	
	// ensure that blockSize is a multiple of Pd's blocksize
	if(bufferSize==0) bufferSize = 256;
	blockSize = (bufferSize / libpd_blocksize()) * libpd_blocksize();
	numInputChannels = inputs;
	numOutputChannels = outputs;
	sampleRate = samplerate;

	
	libpd_init_audio(numInputChannels, numOutputChannels, sampleRate, blockSize/libpd_blocksize());
	// mutex.unlock();
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
	// mutex.lock();
	if(bufferSize != blockSize) {
		//stopPd();
		//printf("Readjusting block size from %d to %d\n", blockSize, bufferSize);
//		LOG("Changing buffer size from "<< blockSize << " to " << bufferSize << endl);
//		load(path, numInputChannels, numOutputChannels, sampleRate, bufferSize);
		blockSize = bufferSize;
		libpd_set_ticks_per_buffer(blockSize/libpd_blocksize());
		//startPd();
	}
	// mutex.unlock();
}

void ofxPd::load(string patchFile) {
	// mutex.lock();
	running = false;
	

	path = patchFile;
	
	hasADC = false;
	
	
	char file[256];
	char dir[512];
	getDirAndFile(patchFile.c_str(), dir, file);
	
	//printf("Starting graph with in: %d out: %d sr: %d bs: %d\n", numInputChannels, numOutputChannels,
	//	   sampleRate, blockSize);
	
	 libpd_printhook = (t_libpd_printhook) ofxPd_printhook;
	/*
	 libpd_banghook = (t_libpd_banghook) NULL;
	 libpd_floathook = (t_libpd_floathook) NULL;
	 libpd_symbolhook = (t_libpd_symbolhook) NULL;
	 libpd_listhook = (t_libpd_listhook) NULL;
	 libpd_messagehook = (t_libpd_messagehook) NULL;
	 */
	
	//stopPd();
	
	startPd();
	
	openPatch(file, dir);
	
	
	
	
	
	
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
	
	
	if(hasADC) {
		printf("Has input\n");
	} else {
		printf("No input\n");
	}
	
	
	running = true;
	// mutex.unlock();
}
void ofxPd::closePatch(string file, string dir) {
	libpd_clear_search_path();
	libpd_start_message();
	string recvr = "pd-"+file;
	printf("Trying to close patch %s\n", file.c_str());
	libpd_finish_message(recvr.c_str(), "menuclose");
	patchIsOpen = false;
}


void ofxPd::openPatch(string file, string dir) {

	libpd_clear_search_path();
	//if(patchIsOpen) closePatch(currFile, currDir);
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

ofxPd::~ofxPd() {
	stopPd();
	LOG("ofxPd::~ofxPd");
	// TODO(mhroth): what message must be sent to pd in order to clear all canvases?
}

void ofxPd::sendFloat(string receiverName, float value) {
	if(!running) return;
	// mutex.lock();
	libpd_float(receiverName.c_str(), value);
	// mutex.unlock();
}

void ofxPd::sendBang(string receiverName) {
	if(!running) return;
	// mutex.lock();
	libpd_bang(receiverName.c_str());
	// mutex.unlock();
}

void ofxPd::sendMidiNote(int channel, int noteNum, int velocity, int blockOffset) {
	if(!running) return;
	// mutex.lock();
	// TODO(mhroth): implement for libpd
	libpd_start_message();
	libpd_add_float(noteNum);
	libpd_add_float(velocity);
	libpd_add_float(channel);
	libpd_finish_list("#notein");
	// mutex.unlock();
}

void ofxPd::sendMidiControlChange(int channel, int ctlNum, int value) {
	// mutex.lock();
	libpd_start_message();
	libpd_add_float(ctlNum);
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#ctlin");
	// mutex.unlock();
}
void ofxPd::sendMidiBend(int channel, int value) {
	// mutex.lock();
	libpd_start_message();
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#bendin");
	// mutex.unlock();
}


void ofxPd::sendMidiAfterTouch(int channel, int value) {
	// mutex.lock();
	libpd_start_message();
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#touchin");
	// mutex.unlock();
}

void ofxPd::sendMidiPolyTouch(int channel, int noteNum, int value) {
	// mutex.lock();
	libpd_start_message();
	libpd_add_float(noteNum);
	libpd_add_float(value);
	libpd_add_float(channel);
	libpd_finish_list("#polytouchin");
	// mutex.unlock();
}

void ofxPd::sendMidiProgramChange(int channel, int program) {
	// mutex.lock();
	libpd_start_message();
	libpd_add_float(program);
	libpd_add_float(channel);
	libpd_finish_list("#pgmin");
	// mutex.unlock();
}




void ofxPd::process(float *input, float *output, int frameCount) {
	if(!running) return;  
	// mutex.lock();
	setBlockSize(frameCount);
	
	// this function uninterleaves and interleaves the input and output buffers, respectively
	libpd_process_float(input, output);
/*
	for(int i = 0; i < frameCount; i++) {
		output[i*2 + 0] = 0;
		output[i*2 + 1] = 0;
	}
*/	// mutex.unlock();
}

void ofxPd::processNonInterleaved(float *input, float *output, int frameCount) {
	if(!running) return;
	// mutex.lock();
	setBlockSize(frameCount);
	libpd_process_raw(input, output);
	// mutex.unlock();
}


