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

bool DatabaseAccessor::runQuery( string tableName, string queryString ) {
	mysqlx::RowResult result = session.sql( queryString ).execute();
}

