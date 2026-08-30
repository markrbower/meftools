#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

#include "PreparedStatementBuilder.h"

using namespace std;
using namespace psb;

PreparedStatementBuilder::PreparedStatementBuilder() {
	initialized = 0;
}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, map<string,string> insertThis, map<string,string> typeMap_ ) {
	initialized = 1;
	bind_ = (MYSQL_BIND*)malloc( insertThis.size() * sizeof(MYSQL_BIND) );

	typeMap = typeMap_;

        queryPrefix = "INSERT INTO " + tableName + " \(";
        queryPostfix = "\) VALUES \(";
}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, list< map<string,string> > insertThese, map<string,string> typeMap_ ) {
	initialized = 1;
	bind_ = (MYSQL_BIND*)malloc( insertThese.front().size() * sizeof(MYSQL_BIND) );
        
	typeMap = typeMap_;

        queryPrefix = "INSERT INTO " + tableName + " \(";
        queryPostfix = "\) VALUES \(";
}

int PreparedStatementBuilder::getInitialized() {
	return PreparedStatementBuilder::initialized;
}

void PreparedStatementBuilder::clear() {
}

/*
int PreparedStatementBuilder::() {
}

int PreparedStatementBuilder::() {
}
*/





// TESTING
/*
int main() {
    // 
    PreparedStatementBuilder builder();




}
*/
