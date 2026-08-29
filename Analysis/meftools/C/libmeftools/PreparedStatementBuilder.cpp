#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

#include "PreparedStatementBuilder.h"

using namespace std;

PreparedStatementBuilder::PreparedStatementBuilder() {
	initialized = 0;
}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, map<string,string> insertThis ) {
	initialized = 1;
	bind_ = (MYSQL_BIND*)malloc( insertThis.size() * sizeof(MYSQL_BIND) );
        
	}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, list< map<string,string> > insertThese ) {
	initialized = 1;
	bind_ = (MYSQL_BIND*)malloc( insertThese.front().size() * sizeof(MYSQL_BIND) );
        
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
