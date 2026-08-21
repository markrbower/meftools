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
	char queryStr[128];
	const char* name = argv[1];
	createAnalysisDatabase( name );

	// Test writing
	DatabaseAccessor dba = DatabaseAccessor( name ); // Generic db that should exist.
    	sprintf( queryStr, "insert into subjects values (UUID_TO_BIN(UUID()),\'testSubject\',\'testSpecies\');" );
	if ( !dba.runSQL( queryStr ) ) {
		cout << "Failure on \'Test writing\'." << endl;
		return 0;
	}

	// Test reading
	sprintf( queryStr, "select name from subjects;" );
	MYSQL_RES* result = dba.runQuery( queryStr );
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result)) != NULL) {
            for (int i = 0; i < mysql_num_fields(result); i++) {
       	        std::cout << row[i] << " ";
            }
            std::cout << std::endl;
	}
        mysql_free_result(result);
}
