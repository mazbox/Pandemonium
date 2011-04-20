//
//  ofxPd.cpp
//  audioOutputExample
//
//  Created by Marek Bereza on 19/04/2011.
//

#include "ofxPd.h"
#include "z_libpd.h"
#include <fstream>
#include <sstream>

int ofxPd::bufferSize = 0;
bool ofxPd::pdInitialized = false;
Slots ofxPd::midiChannels;
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

string ofxPdToString(int i) {
	ostringstream out;
	out << i;
	return out.str();
}


void ofxPd::setup(int samplerate) {
    
	if(!pdInitialized) {
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
	} else {
		printf("ofxPd: Warning: setup already called\n");
	}
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
    
	
	// get next available midi channel. 
	midiChannel = midiChannels.getSlot();
	
	createUniquePatch();

	
	
	
	
	
    // file, folder
    patchPointer = libpd_openfile(file.c_str(), folder.c_str());
	dollarZero = libpd_getdollarzero(patchPointer);
	dollarZeroString = ofxPdToString(dollarZero)+"-";
	printf("Patch %s loaded successfully\n", file.c_str());
	opened = true;
    return true;
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



void ofxPdReplace(string &str, const string &find_what, const string &replace_with)
{
	string::size_type pos=0;
	while((pos=str.find(find_what, pos))!=string::npos)
	{
		str.erase(pos, find_what.length());
		str.insert(pos, replace_with);
		pos+=replace_with.length();
	}
}



void ofxPdReplaceWithChannel(string &str, const string &find_begin, const string &replace_with) {
	
	string::size_type pos=0;
	
	while((pos=str.find(find_begin, pos))!=string::npos)
	{
		// now here, we need to work out how much of the string to replace 
		// - everything up to the next ';' (including it)
		string::size_type semiPos = str.find(";", pos)+1;
		str.erase(pos, semiPos - pos);
		str.insert(pos, replace_with);
		pos+=replace_with.length();
	}
}
void ofxPdReplaceCtlinWithChannel(string &str, string newMidiChannel) {
	string::size_type pos=0;
	string find_begin = " ctlin";
	while((pos=str.find(find_begin, pos))!=string::npos)
	{
		
		// we have to decide what to do with it.
		// ctlin; - passthrough
		// ctlin nn; - rewrite as ctlin nn midichannel;
		// ctlin nn mm; - rewrite as ctlin nn midichannel;
		string::size_type semiPos = str.find(";", pos);
		string ss = str.substr(pos, semiPos - pos);
		printf("\n\nremoving : %s\n\n", ss.c_str());
		
		string replace_with = " ctlin;";
		
		if(ss==" ctlin") { // pass through
			replace_with = " ctlin";
		} else {
			vector<string> parts = ofxPdsplit(ss, ' ');
			if(parts.size()>=3) { // ctlin nn mm; - rewrite as ctlin nn midichannel;
				replace_with = " ctlin " + parts[2] + " " + newMidiChannel;
			}
			str.erase(pos, semiPos - pos);
			str.insert(pos, replace_with);
		}
		pos+=replace_with.length();
	}
}


void ofxPd::renameMidiChannels(string &data) {
	
	// TODO:
	// - then find/replace
	string midiChannelString = ofxPdToString(midiChannel);
	
	// notein nn; ->  notein x;
	ofxPdReplaceWithChannel(data, " notein ", " notein " + midiChannelString+";");
	// bendin nn; ->  bendin x;
	ofxPdReplaceWithChannel(data, " bendin ", " bendin " + midiChannelString+";");
	// pgmin nn;  ->  pgmin x;
	ofxPdReplaceWithChannel(data, " pgmin ", " pgmin " + midiChannelString+";");
	
	// notein;    ->  notein x;
	ofxPdReplace(data, " notein;", " notein " + midiChannelString+";");
	
	
	
	// bendin;    ->  bendin x;
	ofxPdReplace(data, " bendin;", " bendin " + midiChannelString+";");
	
	// pgmin;     ->  pgmin x;
	ofxPdReplace(data, " pgmin;", " pgmin " + midiChannelString+";");
	
	// ctlin nn; ->  ctlin nn x;
	// and
	// ctlin nn mm; -> ctlin nn x;
	ofxPdReplaceCtlinWithChannel(data, midiChannelString);
}


void ofxPd::createUniquePatch() {
	
	
	// create a unique string
	char uniqueIdStr[50];
	sprintf(uniqueIdStr, "patch-%x", (int)this);
	uid = uniqueIdStr;
	
	
	// now we need to insert some code into the patch
	
	// load the file in as a string.
	string data = parseFileToString(folder+"/"+file);
	renameMidiChannels(data);
	
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
		printf("Closing patch\n");
		//libpd_clear_search_path();
	
		libpd_closefile(patchPointer);
		patchPointer = NULL;
		// this deletes the temp file we made in open.
		string fp = folder + "/" + file;
		if(!remove(fp.c_str())) {
			printf("Failed to delete %s\n", fp.c_str());
		}
		midiChannels.releaseSlot(midiChannel);
		
	}
}

void ofxPd::sendFloat(string receiverName, float value) {
//	libpd_float((receiverName+"_"+uid).c_str(), value);
	string r = dollarZeroString + receiverName;
	//printf("Sending %s = %f\n", r.c_str(), value);
	libpd_float(r.c_str(), value);
}

void ofxPd::sendBang(string receiverName) {
	//libpd_bang((receiverName+"_"+uid).c_str());
	string r = dollarZeroString + receiverName;
	libpd_bang(r.c_str());
}

void ofxPd::sendMidiNote(int noteNum, int velocity, int blockOffset) {
	libpd_noteon(midiChannel, noteNum, velocity);
}

void ofxPd::sendMidiControlChange(int ctlNum, int value) {
	libpd_controlchange(midiChannel, ctlNum, value);
}

void ofxPd::sendMidiBend(int value) {
	libpd_pitchbend(midiChannel, value);
}

void ofxPd::sendMidiAfterTouch(int value) {
	libpd_aftertouch(midiChannel, value);
}

void ofxPd::sendMidiPolyTouch(int noteNum, int value) {
	libpd_polyaftertouch(midiChannel, noteNum, value);
}

void ofxPd::sendMidiProgramChange(int program) {
	libpd_programchange(midiChannel, program);
}

float ofxPdClip(float inp) {
	if(inp>1) inp = 1;
	else if(inp<-1) inp = -1;
	else if(inp!=inp) inp = 0;
	return inp;
}
void ofxPd::process(float *inputs, float *outputs, int numFrames) {
	if(bufferSize!=numFrames) setBufferSize(numFrames);
	libpd_float(uid.c_str(), 1);
	libpd_process_float(inputs, outputs);
	libpd_float(uid.c_str(), 0);
	
	for(int i = 0; i < numFrames; i++) {
		outputs[i*2] = ofxPdClip(outputs[i*2]);
		outputs[i*2+1] = ofxPdClip(outputs[i*2+1]);
	}
}


