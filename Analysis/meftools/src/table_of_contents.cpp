#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <RcppCommon.h>
#include <Rcpp.h>

#include "../inst/include/meftools_types.h"

//' @importFrom Rcpp evalCpp
//' @useDynLib meftools
//' @param StringVector strings
//' @export
// [[Rcpp::export]]
Rcpp::NumericMatrix table_of_contents( Rcpp::StringVector strings ) {
    printf( "ToC\n" );
    char *filename = (si1*)strings(0);
    printf( "filename: %s\n", filename );
    printf( "%s\n", (char *)strings(1) );
    long index_data_offset = atol( strings(1) );
    printf( "%ld\n", index_data_offset );
    printf( "%s\n", (char *)strings(2) );
    long number_of_index_entries = atol( strings(2) );
    printf( "%ld\n", number_of_index_entries );
    //	printf( "%d\n", index_data_offset );
    //  printf( "%d\n", number_of_index_entries );
    printf( "out of ToC\n" );
    
    FILE *fp = fopen( filename, "r" );
    fseek( fp, index_data_offset, SEEK_SET );
    Rcpp::NumericVector NumVecBuf( 3 * number_of_index_entries * sizeof(unsigned long long int) );
    unsigned char* buf = (unsigned char*) &(NumVecBuf[0]);
//    unsigned char* buf = (unsigned char *)malloc( 3 * number_of_index_entries * sizeof(unsigned long long int) );
    fread( buf, sizeof("unsigned long long int"), (long)(3 * number_of_index_entries), fp );
    fclose( fp );
    
    // Convert "buf" into a 3-column matrix.
    unsigned long long int l;
    Rcpp::NumericMatrix ToC(3, number_of_index_entries);
    for (int col=0; col<number_of_index_entries; col++ ) {
        l = 0;
        int offset = (col*3 + 0)*sizeof(unsigned long long int);
        for (int i = 0; i < 8; ++i) {
            l = l | ((unsigned long long int)buf[i+offset] << (8 * i));
        }
        ToC(0,col) = l;
        
        l = 0;
        offset = (col*3 + 1)*sizeof(unsigned long long int);
        for (int i = 0; i < 8; ++i) {
            l = l | ((unsigned long long int)buf[i+offset] << (8 * i));
        }
        ToC(1,col) = l;
        
        l = 0;
        offset = (col*3 + 2)*sizeof(unsigned long long int);
        for (int i = 0; i < 8; ++i) {
            l = l | ((unsigned long long int)buf[i+offset] << (8 * i));
        }
        ToC(2,col) = l;
    }
    return( ToC );
}

