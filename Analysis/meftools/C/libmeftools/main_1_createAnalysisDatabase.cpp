/* main_1_createDatabase
Mark Bower
Yale University

Compilation:
make main_1

Usage:
main_1_createDatabase

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

	const char* name = argv[1];
	createAnalysisDatabase( name );

}
