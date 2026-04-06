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

mysqlx::SqlResult DatabaseAccessor::createTable( char& tableName, char& tableValues ) {

}

mysqlx::RowResult DatabaseAccessor::runQuery( string queryString ) {
	mysqlx::RowResult result = session.sql( queryString ).execute();
	return( result );
}

bool DatabaseAccessor::vectorInsert( vector<string> rows ) {
	// Prepare the statement
	MYSQL *mysql;
	mysql = mysql_init(NULL);
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[4];
	char *value0 = "example";
	unsigned long length0 = strlen(value0);
	char *value1 = "example";
	unsigned long length1 = strlen(value1);
	long value2 = 42;
	char *value3 = "example";
	unsigned long length3 = strlen(value3);

	stmt = mysql_stmt_init(mysql);
	char* query = "INSERT INTO peaks (subject,session,time,waveform) VALUES (?, ?, ?, ?)";
	mysql_stmt_prepare(stmt, query, strlen(query));
	memset(bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[3].buffer_type = MYSQL_TYPE_STRING;

	// Start a transaction
	string str = "hi";
	session.startTransaction();
	try {
		for ( auto row: rows ) {
			bind[0].buffer = (char *)value0;
			bind[0].length = &length0;

			bind[1].buffer = (char *)value1;
			bind[1].length = &length1;

			bind[2].buffer = (char *)&value2;

			bind[3].buffer = (char *)value3;
			bind[3].length = &length3;

			mysql_stmt_bind_param(stmt, bind);
			mysql_stmt_execute(stmt);
		}

		// Commit the transaction if everything went well
		session.commit();
	} catch (const mysqlx::Error &err) {
		// Rollback the transaction in case of an error
		session.rollback();
	} 
	if (mysql_stmt_close(stmt)) {
	  fprintf(stderr, " failed while closing the statement\n");
	  fprintf(stderr, " %s\n", mysql_error(mysql));
	  exit(0);
	}
}

