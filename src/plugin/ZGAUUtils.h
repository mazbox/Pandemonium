/*
 *  ZGAUUtils.h
 *  ZG Importer
 *
 *  Created by Marek Bereza on 05/07/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */
#include <Carbon/Carbon.h>
#include <vector>
using namespace std;
#define BASE_DIRECTORY  "/Library/Audio/Sounds/Pandemonium"
#define INSTRUMENT		BASE_DIRECTORY "/Instruments"
#define EFFECT			BASE_DIRECTORY "/Effects"


// creates directories for the instrument and effects
void initDirs();
/**
 * Returns a list of all the patches for either INSTRUMENT or EFFECT
 */
vector<string>* loadPatches(const char *pluginPath);

/**
 * This installs a patch, given its file path and returns its type (it 
 * determines the type of the plugin depending on whether there's a
 * notein object in it or not.
 */

void uninstallPatch(const char *path);

/**
 * Strips the path component of the patch and just leaves the name of the file
 */
string patchNameFromPath(string path);

/**
 * This can take either a list of patches, a folder (with patches inside)
 * or a single patch. 
 */
const char *installFiles(vector<string> *files);