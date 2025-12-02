#include <string>
#include <iostream>

#include "meftools_types.h"

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);

void readMef( char* filename, char* password ) {

	std::vector<std::string> strings(2);
	strings[0] = filename;
	strings[1] = password;
	MEF_HEADER_INFO mefInfo = read_mef_header( strings );

	iterCont <- meftools::MEFcont( filename, 'erlichda', compArgs_caseSpecific$get('bufferSize'), window=timeConstraints, info=compArgs_caseSpecific$get('info') )

	while ( iterCont$hasNext() ) {
	        counterIdx = counterIdx + 1;
        	data = iterData$nextElem();
	        peaks <- NPI:::computePeaks( peakComputationVariables, data, compArgs_caseSpecific )
	}

}

