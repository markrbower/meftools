/* main_analysisLookingForPeaks.cpp
Mark Bower
Yale University

Compilation:
make main_alfp

Usage:
main_analysisLookingForPeaks <MEF_filename> subjectIDorName sessionIDorName

Result:
Stores peaks to MySQL database.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "DatabaseAccessor.h"

void processMEFexample( string filename, string password, string subject, string session, int bufferSize, string signalType, int duration );

using namespace std;

int main(int argc, const char * argv[]) {
    string filename = argv[1];
    string password = "blah";
    string subject = argv[2];
    string session = argv[3];
    int bufferSize = 1024;
    string signalType = "IIS";
    int duration = 100;

    processMEFexample( filename, password, subject, session, bufferSize, signalType, duration );

}
