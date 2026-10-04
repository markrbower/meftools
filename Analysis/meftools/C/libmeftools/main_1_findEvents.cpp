/* main_1_findEvents.cpp
Mark Bower
Yale University

Compilation:
make main_1

Usage:
./main_1 /Users/markbower/Library/CloudStorage/Dropbox/Documents/Concepts/2018_07_29_meftools/meftools/Analysis/meftools/tests/Data/CSC1.mef test_subject test_session


Result:
Finds sovereign peaks and persists them.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "DatabaseAccessor.h"

using namespace std;

void createAnalysisDatabase( const char* name );

int main(int argc, const char * argv[]) {
    	string filename = argv[1];
    	string password = "blah";
    	string subject = argv[2];
    	string session = argv[3];
    	int bufferSize = 1024;
    	string signalType = "IIS";
    	int duration = 100;

	findEvents( filename, password, subject, session, bufferSize, signalType, duration );

	// Begin testing
	// Check that events have been persisted in the "events" table




}

