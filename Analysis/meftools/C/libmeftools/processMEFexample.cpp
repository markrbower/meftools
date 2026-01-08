/* processMEFexample.cpp

A template for using MEF reading functions.

Mark R. Bower
Yale University
*/
#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFcont.h"

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);

void processMEFexample( char* filename, char* password, int bufferSize, vector<long long> timeConstraints, MEFinfo info ) {
	IteratorVector<int> data;

	std::vector<std::string> strings(2);
	strings[0] = filename;
	strings[1] = password;
	MEFinfo info = MEFinfo( filename, password );

	MEFcont mefCont <- MEFcont( filename, password, bufferSize, timeConstraints, info );

	while ( mefCont$hasNext() ) {
	        counterIdx = counterIdx + 1;
        	data = mefCont$nextElem();
	        peaks <- NPI:::computePeaks( peakComputationVariables, data, compArgs_caseSpecific )
	}

}

