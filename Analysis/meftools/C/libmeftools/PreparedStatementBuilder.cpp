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

	generateQuery( insertThis );
}

PreparedStatementBuilder::PreparedStatementBuilder(string tableName, list< map<string,string> > insertThese, map<string,string> typeMap_ ) {
	initialized = 1;
	bind_ = (MYSQL_BIND*)malloc( insertThese.front().size() * sizeof(MYSQL_BIND) );
        
	typeMap = typeMap_;

	generateQuery( insertThese.front() );
}

void PreparedStatementBuilder::generateQuery( map<string,string> tmp ) {
        queryPrefix = "INSERT INTO " + tableName + " \(";
        queryPostfix = "\) VALUES \(";

	int firstTimeFlag = 1;
	for ( auto const&[key,value] : insertThese.front() ) {
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

void PreparedStatementBuilder::clear() {
	counter = 0;
}

void addEntry( string name, string value ) {
	// Find the data type
	string datatype = typeMap[ name ];

	// Call the appropriate add function
	if ( datatype == "MYSQL_TYPE_VARCHAR" ) {
                bind[4].buffer_type = MYSQL_TYPE_VARCHAR;
                bind[4].buffer = (char *)inner_value.c_str();
                length3 = strlen( inner_value.c_str() );
                bind[4].length = &length3;
                bind[4].is_null = 0;
	} else if ( datatype == "MYSQL_LONGLONG" ) {
                bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
                bind[2].buffer = (long long*)&outer_key;
                bind[2].length = 0;
                bind[2].is_null = 0;
	} else if ( datatype == "MYSQL_TYPE_DOUBLE" ) {
        	bind[3].buffer_type = MYSQL_TYPE_DOUBLE;
                double dvalue = std::stod( inner_value );
                bind[3].buffer = (char*)&dvalue;
                bind[3].length = 0;
                bind[3].is_null = 0;
	} else if ( datatype == "MYSQL_TYPE_STRING" ) {
                cout << count << "\t" << element.first << "\t" << element.second << endl;
                bind[count].buffer_type = MYSQL_TYPE_STRING;
                string value = element.second;
                strcpy(varcharValue, value.c_str());
                varcharLength = strlen(varcharValue);
                bind[count].buffer = (char *)varcharValue;
                bind[count].buffer_length = sizeof(varcharValue);
                bind[count].length = &varcharLength;
                bind[count].is_null = 0;
                count++;
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
