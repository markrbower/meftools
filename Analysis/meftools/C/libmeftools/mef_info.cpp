





std::map mef_info( char* filename, char* password ) {
	Rcpp::StringVector strings(2);
// Pack the strings
	strings(0) = filename;
	strings(1) = password;

// Prepare memory space
	MEFTOOLS::MEF_HEADER_INFO mef_info;

// Read the header
	Rcpp::MEF_HEADER_INFO rcpp_mef_header_info = read_mef_header( strings );

// Unpack the structure
	MEF_HEADER mef_header = convertRcppHeader( rcpp_mef_header_info );

// Return
	return( mef_info );
}


