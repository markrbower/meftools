// DatabaseAccessor.cpp
// Mark R. Bower
// Yale University
/*
  #'  Organizes MySQL database activity. 
  #'    For general use, create a Singleton that allows users to set their defaults.
  #' 
*/
#include <string>
#include <iostream>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

DatabaseAccessor::DatabaseAccessor(const std::string& host, int port, const std::string& user, const std::string& password, const std::string& databaseName) {
    _session = make_unique<mysqlx::Session>(host, port, user, password, databaseName );
}

DatabaseAccessor::DatabaseAccessor(const std::string& databaseName) {
    _session = make_unique<mysqlx::Session>("localhost", 33060, "root", "password", databaseName );
}

DatabaseAccessor::~DatabaseAccessor() {
    if (_session) {
        _session->close();
    }
}

mysqlx::SqlResult DatabaseAccessor::createTable( const std::string& tableName, const std::string& tableValues ) {

}

mysqlx::SqlResult DatabaseAccessor::executeQuery(const std::string& query) {
    mysqlx::SqlStatement stmt = _session->sql(query);
    return( stmt.execute() );
}

