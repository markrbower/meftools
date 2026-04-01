#ifndef DATABASE_ACCESSOR
#define DATABASE_ACCESSOR
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include <mysqlx/xdevapi.h>

class DatabaseAccessor {
private:
    std::unique_ptr<mysqlx::Session> _session;

public:
    DatabaseAccessor(const std::string& host, int port, const std::string& user, const std::string& password, const std::string& databaseName);

    DatabaseAccessor(const std::string& databaseName);

    ~DatabaseAccessor();

    mysqlx::SqlResult createTable(const std::string& tableName,const std::string& tableValues);

    mysqlx::SqlResult executeQuery(const std::string& query);
};



