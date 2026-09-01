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
	binding = (MYSQL_BIND*)malloc( insertThis.size() * sizeof(MYSQL_BIND) );

	typeMap = typeMap_;

	generateQuery( tableName, insertThis );
}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, list< map<string,string> > insertThese, map<string,string> typeMap_ ) {
	initialized = 1;
	binding = (MYSQL_BIND*)malloc( insertThese.front().size() * sizeof(MYSQL_BIND) );
        
	typeMap = typeMap_;

	generateQuery( tableName, insertThese.front() );
}

void PreparedStatementBuilder::generateQuery( string tableName, map<string,string> thisMap ) {
        queryPrefix = "INSERT INTO " + tableName + " \(";
        queryPostfix = "\) VALUES \(";

	int firstTimeFlag = 1;
	for ( auto const&[key,value] : thisMap ) {
		if ( firstTimeFlag == 0 ) {
			queryPrefix.append( "," );
			queryPostfix.append( "," );
		}
		firstTimeFlag = 0;
		queryPrefix.append( key );
		queryPostfix.append( "?" );
	}
	query = queryPrefix + queryPostfix + ");";
	cout << query << endl;
}

int PreparedStatementBuilder::getInitialized() {
	return PreparedStatementBuilder::initialized;
}

string PreparedStatementBuilder::getType( string colName ) {
	if( typeMap.find(colName) != typeMap.end() ) { // key found in map
		return typeMap[colName];
	} else {
		cout << "PreparedStatementBuilder::getType :: type not found for " << colName << endl;
		return "";
	}
}

void PreparedStatementBuilder::clear() {
	counter = 0;
}

void PreparedStatementBuilder::addEntry( string key, string value ) {
	unsigned long lenStr;
	char varcharValue[2048];
	unsigned long varcharLength;
	// Find the data type
	string datatype = getType( key );

	// Call the appropriate add function
	if ( datatype == "varchar" ) {
                binding[counter].buffer_type = MYSQL_TYPE_VARCHAR;
                binding[counter].buffer = (char *)value.c_str();
                lenStr = strlen( value.c_str() );
                binding[counter].length = &lenStr;
                binding[counter].is_null = 0;
	} else if ( datatype == "bigint" ) {
                binding[counter].buffer_type = MYSQL_TYPE_LONGLONG;
                binding[counter].buffer = (long long*)&value;
                binding[counter].length = 0;
                binding[counter].is_null = 0;
	} else if ( datatype == "double" ) {
        	binding[counter].buffer_type = MYSQL_TYPE_DOUBLE;
                double dvalue = std::stod( value );
                binding[counter].buffer = (char*)&value;
                binding[counter].length = 0;
                binding[counter].is_null = 0;
	} else if ( datatype == "string" ) {
                binding[counter].buffer_type = MYSQL_TYPE_STRING;
                string value = value;
                strcpy(varcharValue, value.c_str());
                varcharLength = strlen(varcharValue);
                binding[counter].buffer = (char *)varcharValue;
                binding[counter].buffer_length = sizeof(varcharValue);
                binding[counter].length = &varcharLength;
                binding[counter].is_null = 0;
                counter++;
	} else {
		cout << "PreparedStatementBuilder: addEntry: unknown datatype" << endl;
	}
}

void persist() {

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
