/*
 *  Logger.h
 *  Pandemonium
 *
 *  Created by Marek Bereza on 20/11/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */
#include <fstream>
using namespace std;
#define LOG(TEXT) ofstream out; \
out.open("/Users/marek/Desktop/Pandemonium Log.txt", ios::out | ios::app ); \
out << TEXT << endl; \
out.close();