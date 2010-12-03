#ifndef __ofxFileUtils__
#define __ofxFileUtils__
#include <string> 
using namespace std;



/**
 * This gets the read/write directory.
 */
string ofxGetDocumentsDirectory();

/**
 * This gets the read-only directory.
 */
string ofxGetBundlePath();

/**
 * iPhone apps have tmp directories per application. Use this
 * to get the path for this app.
 */
string ofxGetTmpPath();

bool ofxIsDirectory(string filename);
/**
 * Check if a file exists, given its path. Returns 1 if it exists.
 */
bool ofxFileExists(string filename);

/**
 * Create a new directory. Returns 1 if successful.
 */
bool ofxCreateDirectory(string dir);

/**
 * Copies a file from one place to another. Returns 1 if successful.
 */
bool ofxCopyFile(string source, string destination);

/**
 * Copies all files in a directory to another directory. (non-recursive)
 */
bool ofxCopyDirContentsToDir(string source, string destination);

/**
 * Empties a directory given its path. It's recursive and doesn't 
 * delete the directory itself.
 */
void ofxEmptyDir(string path);

/**
 * Same as ofxEmptyDir but also deletes the dir.
 */
void ofxDeleteDir(string path);

void ofxDeleteFile(string path);


/**
 * Returns the name component of a path
 */
string ofxFileName(string path);

#endif // __ofxZip__
