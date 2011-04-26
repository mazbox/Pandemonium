#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "ofxFileUtils.h"

#define COMMENT_START "#X text"


using namespace std;

class ZGParameter {
public:
	ZGParameter() {
		type = "float";
		name = "no name";
		min = 0;
		max = 1;
		units = "";
		value = 0;
	}

	
	// "slider" or "bang"
	string type;
	
	// name (no spaces), displayed in the audio unit
	// and also the receive name in the patch
	string name;
	
	// min and max if it's a slider
	float min;
	float max;
	
	// serves as the default value and then the current value
	float value;
	
	// name of units (TODO)
	string units;
	
	// use the y to sort the parameters
	float y;
};

bool sortZGParameters(ZGParameter *one, ZGParameter *two);


class PdPluginParameters {
public:
	void load(char *path) {
		
		string p = path;
		string filename = ofxFileName(p);
		p += "/" + filename + ".pd";
		string line;
		for(int i = 0; i < parameters.size(); i++) {
			delete parameters[i];
		}
		parameters.clear();
		ifstream infile (p.c_str(), ios_base::in);
		while (getline(infile, line, '\n')) {
			if(line.find(COMMENT_START)==0 && line.find(" param ")!=-1) {
				parseParameter(line);
			}
		}
		
		
		sort(parameters.begin(), parameters.end(), sortZGParameters);
		
	}
	
	
	
	ZGParameter *getParameter(int i) { return parameters[i]; };
	int size() { return parameters.size(); }
	//void clear() { for(int i = 0; i < parameters.size(); i++) delete parameters[i]; parameters.clear(); }
private:
	
	
	void parseParameter(string paramStr) {
		// format is 
		
		// #X text [x] [y] param [name] [type] [min] [max] [default] [units];
		
		// where x and y are numbers, ignored
		// name is the string name of the slider
		// type is either slider, button
		// min, max and units are optional. 
		// they're for setting the minimum, maximum, default and units of the slider
		// (default to range [0,1])
		
		// after, we need to sort on y
		
		vector<string> parts = split(paramStr, ' ');
		if(parts.size()<7) {
			cout << "Wrong format\n" << endl;
			return;
		}
		ZGParameter *param = new ZGParameter();
		param->y = atof(parts[3].c_str());
		param->name = parts[5];
		param->type = parts[6];
		
		
		if(param->type=="slider" || param->type=="int" || param->type=="float") {
			if(parts.size()>=9) {
				param->min = atof(parts[7].c_str());
				param->max = atof(parts[8].c_str());
			}
			
			if(parts.size()>=10) {
				param->value = atof(parts[9].c_str());
			} else {
				param->value = param->min;
			}
			
			if(parts.size()>=11) {
				param->units = parts[10];
			}
		}	
		parameters.push_back(param);
	}

	vector<ZGParameter*> parameters;
	
	
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while(std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
	
	
	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		return split(s, delim, elems);
	}
};