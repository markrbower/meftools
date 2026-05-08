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

using namespace std;

mysqlx::SqlResult DatabaseAccessor::createTable( char* tableName, char* tableValues ) {

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
	MYSQL_BIND bind[4];
	char *value0 = "subject";
	unsigned long length0 = strlen(value0);
	char *value1 = "example";
	unsigned long length1 = strlen(value1);
	long long value2 = 42;
	unsigned long length3;
	int status;

	cout << "Running" << endl;
	stmt = mysql_stmt_init(conn);
	if ( !stmt ) {
		cout << "Could not initialize statement." << endl;
	}
        cout << "Statement initialized" << endl;

	string query = "INSERT INTO peaks (subject,session,time,waveform) VALUES (?,?,?,?)";
	unsigned long stmt_length = query.size();
	cout << stmt_length << endl;
	status = mysql_stmt_prepare(stmt, query.c_str(), stmt_length );
  	if (status) {
	    cout << "Failed on prepare" << endl;
	    fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
	    exit(1);
	} else {
		cout << "Statement looks good." << endl;
	}
	memset(bind, 0, sizeof(bind));

	// Start a transaction
	int fixedLength = fixed_values.size();
	
	int count = 0;
	for ( auto element: fixed_values ) {
		cout << count << "\t" << element.first << endl;
		bind[count].buffer_type = MYSQL_TYPE_VARCHAR;
		string value = element.second;
		bind[count].buffer = (char *)value.c_str();
		unsigned long L = strlen(value.c_str()) + 1;
		bind[count].length = &L;
		bind[count].is_null = 0;
		count++;
	}

	runSQL( "START TRANSACTION;" );
	try {
		for ( auto element: variables ) {
			bind[fixedLength].buffer_type = MYSQL_TYPE_LONG;
			bind[fixedLength].buffer = (char *)&element.first;
			bind[fixedLength].length = 0;
			bind[fixedLength].is_null = 0;

			bind[fixedLength+1].buffer_type = MYSQL_TYPE_VARCHAR;
			bind[fixedLength+1].buffer = (char *)&element.second;
			length3 = strlen( element.second.c_str() );
			bind[fixedLength+1].length = &length3;
			bind[fixedLength+1].is_null = 0;

			status = mysql_stmt_bind_param(stmt, bind);
			if (status) {
			    fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
			    exit(1);
			} else {
			    cout << "Binding looks good." << endl;
			}

			status = mysql_stmt_execute(stmt);
			if (status) {
			    fprintf(stderr, "Error: %s (errno: %d)\n",
			            mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
			    exit(1);
			}
		}
		// Commit the transaction if everything went well
		runSQL( "COMMIT;" );
	} catch (const mysqlx::Error &err) {
		// Rollback the transaction in case of an error
		runSQL( "ROLLBACK;" );
		cout << "rolled back" << endl;
	} 
	if (mysql_stmt_close(stmt)) {
	  fprintf(stderr, " failed while closing the statement\n");
	  fprintf(stderr, " %s\n", mysql_error(conn));
	  exit(0);
	}
}

