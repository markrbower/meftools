// DatabaseAccessor.cpp
// Mark R. Bower
// Yale University
/*
  #'  Organizes MySQL database activity. 
  #' 
*/
#include <string>
#include <iostream>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

DatabaseAccessor::DatabaseAccessor(const std::string& host, int port, const std::string& user, const std::string& password, const std::string& databaseName) {
    _session = make_unique<mysqlx::Session>(host, port, user, password, databaseName );
}

DatabaseAccessor::~DatabaseAccessor() {
    if (_session) {
        _session->close();
    }
}

mysqlx::SqlResult DatabaseAccessor::executeQuery(const std::string& query) {
    mysqlx::SqlStatement stmt = _session->sql(query);
    return( stmt.execute() );
}

