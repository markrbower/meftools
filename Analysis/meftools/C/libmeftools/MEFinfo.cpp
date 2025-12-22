// @param filename String: The complete path to a .mef file
// @param password String: The public password for the MEF file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>

#include "meftools_types.h"
        
using namespace std;

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);
long long** table_of_contents( vector<string> strings );
vector<int> get_discontinuities( vector<string> strings, long long **ToC );

class MEFinfo {
private:
  string filename, password;
  MEF_HEADER_INFO header;
  long long **ToC;
  vector<int> discontinuities;
  vector<string> variables;

public:
  MEFinfo( vector<string> args ) {
    this->filename = args[0];
    this->password = args[1];

    header = read_mef_header( args );

    variables.push_back( this->filename );
    variables.push_back( header.index_data_offset );
    variables.push_back( header.number_of_index_entries );

    ToC = table_of_contents( variables );

    discontinuities = get_discontinuities( variables, ToC );
  }

  MEF_HEADER_INFO getHeader() { return( header ); }

  long long** getToC() { return( ToC ); }

  vector<int> getDiscontinuities() { return( discontinuities ); }  
  
};

