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

mysqlx::SqlResult DatabaseAccessor::runQuery( string queryString ) {
	mysqlx::SqlResult result = session.sql( queryString ).execute();
	return( result );
}

bool DatabaseAccessor::vectorInsert( map<long long,string> rows ) {
	// Prepare the statement
	MYSQL *mysql;
	mysql = mysql_init(NULL);
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[4];
	char *value0 = "example";
	unsigned long length0 = strlen(value0);
	char *value1 = "example";
	unsigned long length1 = strlen(value1);
	long long value2 = 42;
	char *value3 = "example";
	unsigned long length3 = strlen(value3);

	stmt = mysql_stmt_init(mysql);
	char* query = "INSERT INTO peaks (subject,session,time,waveform) VALUES (?, ?, ?, ?)";
	mysql_stmt_prepare(stmt, query, strlen(query));
	memset(bind, 0, sizeof(bind));

//	std::auto_ptr< sql::Statement > sqlstmt( session.createStatement() );	

//	mysqlx::PreparedStatement prep = session.sql( "INSERT INTO products (name, price, stock) VALUES (?, ?, ?)").prepare();

	bind[0].buffer_type = MYSQL_TYPE_VARCHAR;
	bind[1].buffer_type = MYSQL_TYPE_VARCHAR;
	bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[3].buffer_type = MYSQL_TYPE_VARCHAR;

	// Start a transaction
	string str = "hi";
	session.startTransaction();
	try {
		for ( auto row: rows ) {
			bind[0].buffer = (char *)value0;
			bind[0].length = &length0;
			bind[0].is_null = 0;

			bind[1].buffer = (char *)value1;
			bind[1].length = &length1;
			bind[1].is_null = 0;

			bind[2].buffer = (char *)&value2;
			bind[2].is_null = 0;

			bind[3].buffer = (char *)value3;
			bind[3].length = &length3;
			bind[3].is_null = 0;

			mysql_stmt_bind_param(stmt, bind);
			mysql_stmt_execute(stmt);
			cout << "send" << endl;
			mysqlx::SqlResult result = runQuery("SELECT COUNT(*) FROM peaks;");
			mysqlx::Row val;
			while (val = result.fetchOne()) {
			    std::cout << "Column1: " << val[0] << std::endl;
			}
		}

		// Commit the transaction if everything went well
		session.commit();
		cout << "committed" << endl;
		mysqlx::SqlResult result1 = runQuery("SELECT COUNT(*) FROM peaks;");
		mysqlx::Row val;
		while (val = result1.fetchOne()) {
		    std::cout << "Column1: " << val[0] << std::endl;
		}
	} catch (const mysqlx::Error &err) {
		// Rollback the transaction in case of an error
		session.rollback();
		cout << "rolled back" << endl;
	} 
	if (mysql_stmt_close(stmt)) {
	  fprintf(stderr, " failed while closing the statement\n");
	  fprintf(stderr, " %s\n", mysql_error(mysql));
	  exit(0);
	}
}

