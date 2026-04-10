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
//	MYSQL *mysql;
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[1];
	int value0 = 3;
//	unsigned long length0 = strlen(value0);
	char *value1 = "example";
	unsigned long length1 = strlen(value1);
	long long value2 = 42;
	char *value3 = "example";
	unsigned long length3 = strlen(value3);
	int status;

	cout << "Running" << endl;
//       mysql = mysql_init( DatabaseAccessor::conn );
	stmt = mysql_stmt_init(conn);
	if ( !stmt ) {
		cout << "Could not initialize statement." << endl;
	}
        cout << "Statement initialized" << endl;

	string query = "INSERT INTO peaks (id) VALUES (?)";
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

//	std::auto_ptr< sql::Statement > sqlstmt( session.createStatement() );	

//	mysqlx::PreparedStatement prep = session.sql( "INSERT INTO products (name, price, stock) VALUES (?, ?, ?)").prepare();


	// Start a transaction
	string str = "hi";
	session.startTransaction();
	try {
		for ( auto row: rows ) {
			bind[0].buffer_type = MYSQL_TYPE_LONG;
			bind[0].buffer = (char *)&value0;
			bind[0].length = 0;
			bind[0].is_null = 0;
/*
			bind[1].buffer = (char *)value1;
			bind[1].length = &length1;
			bind[1].is_null = 0;

			bind[2].buffer = (char *)&value2;
			bind[2].is_null = 0;

			bind[3].buffer = (char *)value3;
			bind[3].length = &length3;
			bind[3].is_null = 0;
*/
			status = mysql_stmt_bind_param(stmt, bind);
			if (status) {
			    fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
			    exit(1);
			} else {
			    cout << "Binding looks good." << endl;
			}

			status = mysql_stmt_execute(stmt);
			cout << status << endl;
			if (status) {
			    fprintf(stderr, "Error: %s (errno: %d)\n",
			            mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
			    exit(1);
			}
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
	  fprintf(stderr, " %s\n", mysql_error(conn));
	  exit(0);
	}
}

