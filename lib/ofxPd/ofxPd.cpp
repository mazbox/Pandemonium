//
//  ofxPd.cpp
//  audioOutputExample
//
//  Created by Marek Bereza on 19/04/2011.
//

#include "ofxPd.h"
#include "z_libpd.h"
#include <fstream>
#include <vector>
#include <sstream>

int ofxPd::bufferSize = 0;
bool ofxPd::pdInitialized = false;

void pdprint(const char *s) {
	printf("PD: %s", s);
}

ofxPd::ofxPd() {
	opened = false;
	uid = "";
	patchPointer = NULL;
}
ofxPd::~ofxPd() {
	if(opened && pdInitialized) {
		close();
	}
}
void ofxPd::setBufferSize(int bs) {
	if(bufferSize!=bs) {
		bufferSize = (bs/libpd_blocksize()) * libpd_blocksize();
		libpd_set_ticks_per_buffer(bufferSize/libpd_blocksize());
	}

}

void ofxPd::setup(int samplerate) {
    
	// for debugging
	libpd_printhook = (t_libpd_printhook) pdprint;
	
    // work out block size (pd works in 64 sample blocks - 
	// specified by libpd_blocksize())
    int bufferSize = 256;
    bufferSize = (bufferSize/libpd_blocksize()) * libpd_blocksize();
	
	
	
    libpd_init();
    libpd_init_audio(2, 2, samplerate, 1);

    // default value
	setBufferSize(256);
	
    // enable sound - send "pd dsp 1"
	libpd_start_message();
	libpd_add_float(1);
	libpd_finish_message("pd", "dsp");
    pdInitialized = true;
	printf("PD Initialized\n");
}



bool ofxPd::open(string patchFile) {
	close();
    if(!pdInitialized) {
        printf("ofxPd::load() Error: Pd not inited - please call "
               "ofxPd::setup(...) to start pd before loading a patch\n");
        return false;
    }
    this->patchFile = patchFile;
    int lastSlashPos = patchFile.rfind("/");
    if(lastSlashPos==-1) {
        printf("ofxPd::load() Error: absolute url not specified. We need a '/'"
			   " in the path: '%s'\n", patchFile.c_str());
        return false;
    }
	
	folder = patchFile.substr(0, lastSlashPos);
	file = patchFile.substr(lastSlashPos+1);
    

	
	createUniquePatch();

	
	
	
	
	
    // file, folder
    patchPointer = libpd_openfile(file.c_str(), folder.c_str());
	printf("Patch %s loaded successfully\n", file.c_str());
	opened = true;
    return true;
}
void ofxPd::renameReceives(string &contents) {
	
}

void ofxPd::createUniquePatch() {
	
	
	// create a unique string
	char uniqueIdStr[50];
	sprintf(uniqueIdStr, "patch-%x", (int)this);
	uid = uniqueIdStr;
	
	
	// now we need to insert some code into the patch
	
	// load the file in as a string.
	string data = parseFileToString(folder+"/"+file);
	renameReceives(data);
	
	file = uid+".pd";
	
	// then save it to a unique file.
	stringToFile(folder+"/"+file, data);
}

void ofxPd::stringToFile(string filePath, string contents) {
	ofstream myfile (filePath.c_str());
	if (myfile.is_open()) {
		myfile << contents;
		myfile.close();
	} else {
		printf("ofxPd::stringToFile() Error!! could not write temp patch file!\n");
	}
}

#pragma mark string utils
std::vector<std::string> &ofxPdsplit(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> ofxPdsplit(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return ofxPdsplit(s, delim, elems);
}

string ofxPdToString(int i) {
	ostringstream out;
	out << i;
	return out.str();
}

void ofxPd::processForConnects(string &line) {
	// and then add 2 to all the connection object id's
	// - that's #X connect a+2 b c+2 d
	
	if(line.find("#X connect")!=-1) {
		// take the 1st and 3rd number, add one, and get out.
		vector<string> parts = ofxPdsplit(line, ' ');
		parts[2] = ofxPdToString(atoi(parts[2].c_str())+2);
		parts[4] = ofxPdToString(atoi(parts[4].c_str())+2);
		line = "";
		for(int i = 0; i < parts.size(); i++) {
			if(i>0) line += " ";
			line += parts[i];
		}
		
	}
}







string ofxPd::parseFileToString(string filePath) {
	// after #N canvas...
	// we want
	// "#X obj 232 108 r uid;"
	// "#X obj 232 147 switch~;"
	
	// and put this at the end
	// "#X connect 0 0 1 0;"
	string line, buf;
	ifstream InFile( filePath.c_str() );
	while(getline(InFile,line)) {
		if(buf=="") { // this is the first line - we want to append something
					  // after the first line...
			
			buf += line+"\n";
			buf += "#X obj 232 108 r "+uid+";\n#X obj 232 147 switch~;\n";
		} else {
			processForConnects(line);
			buf += line+"\n";
		}

	}
	
	// the connection from the receive to the switch~
	buf += "#X connect 0 0 1 0;\n";
	return buf;

}

void ofxPd::close() {
	if(patchPointer!=NULL) {
		//libpd_clear_search_path();
	
		libpd_closefile(patchPointer);
		
		// this deletes the temp file we made in open.
		string fp = folder + "/" + file;
		remove(fp.c_str());
		
	}
}

void ofxPd::sendFloat(string receiverName, float value) {
	libpd_float(receiverName.c_str(), value);
}

void ofxPd::sendBang(string receiverName) {
	libpd_bang(receiverName.c_str());
}

void ofxPd::sendMidiNote(int channel, int noteNum, int velocity, int blockOffset) {
	libpd_noteon(channel, noteNum, velocity);
}

void ofxPd::sendMidiControlChange(int channel, int ctlNum, int value) {
	libpd_controlchange(channel, ctlNum, value);
}

void ofxPd::sendMidiBend(int channel, int value) {
	libpd_pitchbend(channel, value);
}

void ofxPd::sendMidiAfterTouch(int channel, int value) {
	libpd_aftertouch(channel, value);
}

void ofxPd::sendMidiPolyTouch(int channel, int noteNum, int value) {
	libpd_polyaftertouch(channel, noteNum, value);
}

void ofxPd::sendMidiProgramChange(int channel, int program) {
	libpd_programchange(channel, program);
}






void ofxPd::process(float *inputs, float *outputs, int numFrames) {
	if(bufferSize!=numFrames) setBufferSize(numFrames);
	sendFloat(uid.c_str(), 1);
	libpd_process_float(inputs, outputs);
	sendFloat(uid.c_str(), 0);
}