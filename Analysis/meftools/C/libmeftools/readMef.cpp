#include <string>
#include <iostream>

#include "meftools_types.h"

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);

void readMef( char* filename, char* password ) {
	IteratorVector<int> data;

	std::vector<std::string> strings(2);
	strings[0] = filename;
	strings[1] = password;
	MEF_HEADER_INFO mefInfo = read_mef_header( strings );

	MEFcont mefCont <- meftools::MEFcont( filename, 'erlichda', compArgs_caseSpecific$get('bufferSize'), window=timeConstraints, info=compArgs_caseSpecific$get('info') )

	while ( mefCont$hasNext() ) {
	        counterIdx = counterIdx + 1;
        	data = mefCont$nextElem();
	        peaks <- NPI:::computePeaks( peakComputationVariables, data, compArgs_caseSpecific )
	}

}

