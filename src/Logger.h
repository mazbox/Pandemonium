/*
 *  Logger.h
 *  Pandemonium
 *
 *  Created by Marek Bereza on 20/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */
#include <fstream>
#include <iostream>

using namespace std;

//#define ENABLE_LOGGING

#ifdef ENABLE_LOGGING
#define INIT_LOG(A) initLog(A);
#else
#define INIT_LOG(A) {string cjdkfjs = A;}
#endif

#ifdef ENABLE_LOGGING
#define LOG(TEXT){ ofstream out; \
out.open(getLogFile().c_str(), ios::out | ios::app ); \
out << TEXT << endl; \
out.close();}
#else
#define LOG(TEXT) {std::cout << TEXT << endl; }
#endif

void initLog(string path);

string getLogFile();

