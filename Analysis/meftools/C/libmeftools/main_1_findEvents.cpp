/* main_1_findEvents.cpp
Mark Bower
Yale University

Compilation:
make main_1

Usage:
main_0 test

Result:
Creates a MySQL database for EEG data analysis

*/
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "DatabaseAccessor.h"

using namespace std;

void createAnalysisDatabase( const char* name );

int main(int argc, const char * argv[]) {
	char queryStr[128];
	const char* name = argv[1];
	cout << argv[0] << "\t" << argv[1] << endl;

	findEvents( argv[1] );

	// Begin testing.

}
