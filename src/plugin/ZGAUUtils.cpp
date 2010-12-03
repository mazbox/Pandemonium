/*
 *  ZGAUUtils.cpp
 *  ZG Importer
 *
 *  Created by Marek Bereza on 05/07/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */

#include "ZGAUUtils.h"
#include "ofxDirList.h"
#include "ofxFileUtils.h"

#include <Carbon/Carbon.h>
#include <fstream>
vector<string> patchPaths;

void findPatches(const char *dir);
void initDirs() {
	printf("initDirs()\n");
	if(!ofxIsDirectory(string(EFFECT))) {
		ofxCreateDirectory(string(EFFECT));
	}
	if(!ofxIsDirectory(string(INSTRUMENT))) {
		ofxCreateDirectory(string(INSTRUMENT));
	}
}

vector<string>* loadPatches(const char *pluginPath) {
	//char path[512];


	//getBundlePath(bundleReference, path);
	//CFRelease(bundleReference);
	//strcat(path, "Contents/Resources/");
	string path = pluginPath;
	//path += "/Contents/Resources/patches";
	findPatches(path.c_str());
	
	
	return &patchPaths;
}





/**
 * Given a directory, this fills up patchPaths with the paths to the files in the directory
 * ending in ".pd"
 */
void findPatches(const char *dir) {
	patchPaths.clear();
	ofxDirList DIR;
	int length = DIR.listDir(dir);

	for(int i = 0; i < length; i++) {

		if(ofxIsDirectory(DIR.getPath(i))) {
			patchPaths.push_back(DIR.getPath(i));
		}
	}
}


/**
 * Looks for a notein object in the patch specified by its path, and then
 * returns EFFECT or INSTRUMENT depending on its presence.
 */
const char *getType(string patchFile) {
	string line;
	ifstream myfile (patchFile.c_str());
	if (myfile.is_open()) {
		while (! myfile.eof() ) {
			getline (myfile,line);
			if(line.find("notein")!=-1) {
				myfile.close();
				return INSTRUMENT;
			}
		}
	}
	myfile.close();
	return EFFECT;
}





vector<string> listDir(string path) {
	vector<string> dir;
	ofxDirList DIR;
	int numEntries = DIR.listDir(path);
	for(int i = 0; i < numEntries; i++) {
		dir.push_back(DIR.getPath(i));
	}

	return dir;
}
string getFileName(string fileName) {
	fileName = fileName.substr(fileName.rfind("/")+1);
	return fileName;
}


string getDirName(string dirName) {
	if(dirName.rfind("/")==dirName.size()-1)
		dirName = dirName.substr(0, dirName.rfind("/"));
	return getFileName(dirName);
}



const char *getTypeForDir(string path) {
	const char *type = NULL;
	vector<string> dir = listDir(path);
	string dirName = getDirName(path);
	for(int i = 0; i < dir.size(); i++) {
		if(getFileName(dir[i])==dirName+".pd") {
			printf("Found %s\n", dirName.c_str());

			const char *t = getType(dir[i]);
			printf("It's a %s\n", t);
			if(strcmp(t, INSTRUMENT)==0) {
				type = INSTRUMENT;
			} else {
				type = EFFECT;
			}
		}
	}
	return type;
}

const char *installFiles(vector<string> *files) {
	
	for(int i = 0; i < files->size(); i++) {
		if(files->at(i).find("file://localhost")!=-1) {
			files->at(i) = files->at(i).substr(string("file://localhost").size());
			printf("file----%s\n", files->at(i).c_str());
		}
	}
	

	// - if so, check that there is a patch with the same name as the folder
	// then copy the files over.
	
	// if not, then 
	const char *type = NULL;
	for(int i = 0; i < files->size(); i++) {
		printf("file:: %s\n", files->at(i).c_str());
		if(ofxIsDirectory(files->at(i))) {
			string patchName = patchNameFromPath(files->at(i));
			
			const char *typ = getTypeForDir(files->at(i));
			if(typ!=NULL) {
				printf("Copeee\n");
				// copy the directory
				type = typ;
				string dest = string(type)+"/"+patchName;
				if(ofxFileExists(dest)) {
					ofxDeleteDir(dest);
				}
				string cmd = "cp -R \"" + files->at(i) + "\" \"" + dest + "\"";
				system(cmd.c_str()); 
			}

		} else {
			// install the directory normally.
			printf("It's a file\n");
			
			
			// create a directory for it
			string path = files->at(i);
			string dir = path.substr(0, path.rfind(".pd"));
			dir = dir.substr(dir.rfind("/")+1);
			type = getType(path);
			string destDir = string(type) + "/"+dir+"/";
			ofxCreateDirectory(destDir);
			ofxCopyFile(path, destDir  +patchNameFromPath(path));



			// install patch
		
		}
	}
	return type;
}


string patchNameFromPath(string path) {
	
	int lastSlashIndex = path.rfind("/");

	if(lastSlashIndex==path.size()-1) { // it's a directory
		lastSlashIndex = path.rfind("/", path.size()-2);
	}

	if(lastSlashIndex!=-1) {
		string name = path.substr(lastSlashIndex+1);
		return name;
	}
	return path;
}

void uninstallPatch(const char *path) {
	ofxDeleteDir(path);
}


