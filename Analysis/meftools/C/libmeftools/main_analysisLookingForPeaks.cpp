#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "DatabaseAccessor.h"

void processMEFexample( string filename, string password, string subject, string session, int bufferSize, string signalType, int duration );

using namespace std;

int main(int argc, const char * argv[]) {
    string filename = "";
    string password = "erlichda";
    int bufferSize = 1024;
    string subject = argv[1];
    string session = argv[2];
    string signalType = "IIS";
    int threshold = 50;
    int duration = 100;

    processMEFexample( filename, password, subject, session, bufferSize, signalType, duration );

}
