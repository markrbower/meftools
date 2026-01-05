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
    variables.push_back( to_string( header.index_data_offset ) );
    variables.push_back( to_string( header.number_of_index_entries ) );

    ToC = table_of_contents( variables );

    discontinuities = get_discontinuities( variables, ToC );
  }

  MEF_HEADER_INFO getHeader() { return( header ); }

  long long** getToC() { return( ToC ); }

  vector<int> getDiscontinuities() { return( discontinuities ); }  
  
  list<vector<int>> findContinuousMefSequences( int time0, int time1 ) {
    vector<long long> result;
    int microsecPerSample = 1E6/header.sampling_frequency;

    int doneFlag = 0;
    list<vector<int>> conts;
    vector<int> contiguousStarts;
    vector<int> contiguousStops;
    vector<int> dsamp;
    int N = discontinuities.size();
    for ( int i=0; i<(N-1); i++ ) { // the last block cannot contain a start
      dsamp.push_back( ToC[3,(i+1)] - ToC[3,i] );
      if ( ToC[1][i] <= time1 ) {
        if ( ToC[1][i] >= time0 ) {
          if ( discontinuities[i] == 1 ) {
            contiguousStarts.push_back( i );
	    contiguousStops.push_back( i-1 );
          } 
        }
      }
      if ( doneFlag == 0 ) {
        contiguousStops.push_back( i-1 );
        doneFlag = 1;
      }
    }
    contiguousStops.erase( contiguousStops.begin() );
    
    N = contiguousStarts.size();
    for ( int i=0; i<N; i++ ) {
      vector<int> tmp;
      tmp.push_back( ToC[1][contiguousStarts[i]] );
      tmp.push_back( ToC[1][contiguousStops[i]] + dsamp[contiguousStops[i]]*microsecPerSample );
      conts.push_back( tmp );
    }

    return( conts );
  }

};

