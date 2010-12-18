#pragma once

#include <string>
using namespace std;




typedef void (*void_t)();
//libpd_init
//libpd_clear_search_path
//libpd_start_message


typedef int (*int_t)();
//libpd_blocksize();

typedef void (*t__int)(int);
//libpd_set_ticks_per_buffer

typedef void (*t__float)(float);
//libpd_add_float

typedef void (*t_str_float)(const char *, float);
//libpd_float


typedef void (*t_str_str)(const char *, char *);
//libpd_finish_message

typedef void (*t_str)(const char *);
//libpd_bang
//libpd_finish_list
//libpd_add_symbol

typedef void (*t_int_int_int_int)(int, int, int, int);
//libpd_init_audio

typedef void (*t_floatp_floatp)(float *, float *);
//libpd_process_float
//libpd_process_raw










class ofxPd {
public:
	ofxPd();
	void setup(int inputs, int outputs, int samplerate, int bufferSize);
	void getDirAndFile(const char *path, char *outDir, char *outFile);
	int getBlockSize() { return blockSize; } 
	
	
	void setBlockSize(int bufferSize);	
	void load(string patchFile);
	
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
	
	void *pdHandle;
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
	
	
	
	
	
	
	
	
	
	
	
	void_t init;
	void_t clear_search_path;
	void_t start_message;
	
	int_t blocksize;
	t__int set_ticks_per_buffer;
	
	t__float add_float;
	
	
	t_str_float Float;
	t_str_str finish_message;
	
	t_str bang;
	t_str finish_list;
	t_str add_symbol;
	
	t_str add_to_search_path;
	
	t_int_int_int_int init_audio;
	
	
	t_floatp_floatp process_float;
	t_floatp_floatp process_raw;

	
};
