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
		ofstream file("/Users/marek/Desktop/log.txt", ofstream::app);
		file << (char *)ptr << endl;
		file.close();
		printf("%s\n", (char *) ptr);
	}
};

ofxPd::ofxPd() {
	running = false;
	hasADC = false;
	patchIsOpen = false;
	libpd_init();
	libpd_clear_search_path();
	LOG("ofxPd::ofxPd");
	
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
		//printf("Readjusting block size from %d to %d\n", blockSize, bufferSize);
		LOG("Changing buffer size from "<< blockSize << " to " << bufferSize << endl);
		load(path, numInputChannels, numOutputChannels, sampleRate, bufferSize);

	}
}

void ofxPd::load(string patchFile, int inputs, int outputs, int samplerate, int bufferSize) {
	running = false;
	

	
	
	hasADC = false;
	// ensure that blockSize is a multiple of Pd's blocksize
	if(bufferSize==0) bufferSize = 256;
	blockSize = (bufferSize / libpd_blocksize()) * libpd_blocksize();
	numInputChannels = inputs;
	numOutputChannels = outputs;
	sampleRate = samplerate;
	path = patchFile;
	
	char file[256];
	char dir[512];
	getDirAndFile(patchFile.c_str(), dir, file);
	
	//printf("Starting graph with in: %d out: %d sr: %d bs: %d\n", numInputChannels, numOutputChannels,
	//	   sampleRate, blockSize);
	/*
	 libpd_printhook = (t_libpd_printhook) ofxPd_printhook;
	 libpd_banghook = (t_libpd_banghook) NULL;
	 libpd_floathook = (t_libpd_floathook) NULL;
	 libpd_symbolhook = (t_libpd_symbolhook) NULL;
	 libpd_listhook = (t_libpd_listhook) NULL;
	 libpd_messagehook = (t_libpd_messagehook) NULL;
	 */
	
	stopPd();
	libpd_init_audio(numInputChannels, numOutputChannels, samplerate, blockSize/libpd_blocksize());
	
	
	
	openPatch(file, dir);
	
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
	
	
	if(hasADC) {
		printf("Has input\n");
	} else {
		printf("No input\n");
	}
	
	
	running = true;
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
	libpd_float(receiverName.c_str(), value);
}

void ofxPd::sendBang(string receiverName) {
	if(!running) return;
	libpd_bang(receiverName.c_str());
}

void ofxPd::sendMidiNote(int channel, int noteNum, int velocity, int blockOffset) {
	if(!running) return;
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
	if(!running) return;  
	setBlockSize(frameCount);
	
	// this function uninterleaves and interleaves the input and output buffers, respectively
	libpd_process_float(input, output);
}

void ofxPd::processNonInterleaved(float *input, float *output, int frameCount) {
	if(!running) return;
	setBlockSize(frameCount);
	libpd_process_raw(input, output);
}


