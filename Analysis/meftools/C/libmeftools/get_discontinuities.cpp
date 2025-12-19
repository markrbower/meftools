#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>

#include "meftools_types.h"
        
using namespace std;

// [[Rcpp::plugins("cpp11")]]

//' @useDynLib meftools
//' @param StringVector strings
//' @param NumericMatrix ToC
//' @export
vector<int> get_discontinuities( vector<string> strings, long long **ToC ) {
  char *filename = (char *)strings[0].c_str();
  int number_of_index_entries = stoi( strings[1] );

  FILE *fp = fopen( filename, "r" );
  unsigned char* buf = (unsigned char *)malloc( 1 * sizeof(unsigned char) );
  // Convert "buf" into a 3-column matrix.
  vector<int> discontinuities(number_of_index_entries);
  for (int col=0; col<number_of_index_entries; col++ ) {
    fseek( fp, ToC[1][col]+30, SEEK_SET );
    fread( buf, sizeof(unsigned char), 1, fp );
    discontinuities.push_back( (int) buf[0] );
  }
  fclose( fp );
  return( discontinuities );
}




