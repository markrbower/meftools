/* processMEFexample.cpp

A template for using MEF reading functions.

Mark R. Bower
Yale University
*/
#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"
#include "MEFcont.h"

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);

void processMEFexample( string filename, string password, int bufferSize ) {

	std::vector<std::string> strings(2);
	strings[0] = filename;
	strings[1] = password;
	MEFinfo info = MEFinfo( filename, password );

	MEFcont mefCont <- MEFcont( filename, password, info, bufferSize );

	while ( mefCont.hasNext() ) {
        	MEFiter it = mefCont.nextElem();

//	        peaks <- NPI:::computePeaks( peakComputationVariables, data, compArgs_caseSpecific )
	}

}

