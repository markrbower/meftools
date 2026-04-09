// DatabaseAccessor.cpp
// Mark R. Bower
// Yale University
/*
  #'  Organizes MySQL database activity. 
  #'    For general use, create a Singleton that allows users to set their defaults.
  #' 
*/
#include <iostream>
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

bool DatabaseAccessor::mapInsert( map<long long, string> keyValues ) {
	return(0);
}

