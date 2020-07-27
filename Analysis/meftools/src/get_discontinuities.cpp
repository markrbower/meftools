#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "../inst/include/meftools_types.h"
        
// [[Rcpp::plugins("cpp11")]]

#include <RcppCommon.h>
#include <Rcpp.h>

//' @importFrom Rcpp evalCpp
//' @useDynLib meftools
//' @param StringVector strings
//' @param NumericMatrix ToC
//' @export
// [[Rcpp::export]]
Rcpp::NumericVector get_discontinuities( Rcpp::StringVector strings, Rcpp::NumericMatrix ToC ) {
  char *filename = strings(0);
  int number_of_index_entries = atoi( strings(1) );

  FILE *fp = fopen( filename, "r" );
  unsigned char* buf = (unsigned char *)malloc( 1 * sizeof(unsigned char) );
  // Convert "buf" into a 3-column matrix.
  Rcpp::NumericVector discontinuities(number_of_index_entries);
  for (int col=0; col<number_of_index_entries; col++ ) {
    fseek( fp, ToC(1,col)+30, SEEK_SET );
    fread( buf, sizeof(unsigned char), 1, fp );
    discontinuities(col) = (int) buf[0];
  }
  fclose( fp );
  return( discontinuities );
}




