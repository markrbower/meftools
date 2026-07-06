// @param filename String: The complete path to a .mef file
// @param password String: The public password for the MEF file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>
#include <cmath>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"
        
using namespace std;

MEF_HEADER_INFO read_mef_header( vector<string> args );
long long** table_of_contents( vector<string> variables );
vector<int> get_discontinuities( vector<string> variables, long long** ToC );

MEFinfo::MEFinfo( string filename, string password ) {
    cout << "Creating MEFinfo." << endl;

    this->filename = filename;
    this->password = password;

    vector<string> args;
    args.push_back( filename );
    args.push_back( password );
    cout << "Trying read_mef_header" << endl;
    header = read_mef_header( args );
    cout << "read_mef_header done" << endl;

    variables.push_back( this->filename );
    variables.push_back( to_string( header.index_data_offset ) );
    variables.push_back( to_string( header.number_of_index_entries ) );

    cout << "Trying table_of_contents" << endl;
    ToC = table_of_contents( variables );
    cout << "table_of_contents done" << endl;

    cout << "Trying get_discontinuities" << endl;
    discontinuities = get_discontinuities( variables, ToC );
    cout << "get_discontinuities done" << endl;

    cout << "MEFinfo created." << endl;
}

vector<int> MEFinfo::getDiscontinuities() { return( discontinuities ); }  
  
MEF_HEADER_INFO MEFinfo::getHeader() {
    return header;
}

long long** MEFinfo::getToC() {
    return ToC;
}

string MEFinfo::getFilename() {
    return filename;
}

string MEFinfo::getPassword() {
    return password;
}

long long MEFinfo::getStepSize() {
    long long stepSize;
    stepSize = 1.0 / header.sampling_frequency;
    return stepSize;
}

