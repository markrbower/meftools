// DatabaseAccessor.cpp
// Mark R. Bower
// Yale University
/*
  #'  Organizes MySQL database activity. 
  #'    For general use, create a Singleton that allows users to set their defaults.
  #' 
*/
#include <iostream>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"
#include <string>
#include <map>

using namespace std;

mysqlx::SqlResult DatabaseAccessor::createTable( string tableName, string tableValues ) {

}

// runQuery checks for a valid result
MYSQL_RES* DatabaseAccessor::runQuery( string queryString ) {
	if (mysql_query(conn, queryString.c_str())) {
	    fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
	    return NULL;
	}
	result = mysql_store_result(conn);
	if (result == NULL) {
	    fprintf(stderr, "Could not retrieve result set: %s\n", mysql_error(conn));
	    return NULL;
	}
	return( result );
}

// runSQL does not return a result
void DatabaseAccessor::runSQL( string queryString ) {
	if (mysql_query(conn, queryString.c_str())) {
	    fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
	}
}

bool DatabaseAccessor::mapInsert( string tableName, map<string,string> fixed_values, map<long long,string> variables ) {
	// Prepare the statement
	MYSQL_STMT *stmt;

	// Start a transaction
	if (mysql_query(conn, "START TRANSACTION;")) {
        	std::cerr << "START TRANSACTION failed. Error: " << mysql_error(conn) << "\n";
        	mysql_close(conn);
        	return EXIT_FAILURE;
    	}

        // Your SQL operations go here

	// Commit the transaction
	if (mysql_query(conn, "COMMIT;")) {
        	std::cerr << "COMMIT failed. Error: " << mysql_error(conn) << "\n";
	}
	
}

