




void readMef( char* filename, char* password ) {

	MEF_INFO mefInfo = read_mef_header( filename, password );

	iterCont <- meftools::MEFcont( filename, 'erlichda', compArgs_caseSpecific$get('bufferSize'), window=timeConstraints, info=compArgs_caseSpecific$get('info') )

	while ( iterCont$hasNext() ) {
	        counterIdx = counterIdx + 1;
        	data = iterData$nextElem();
	        peaks <- NPI:::computePeaks( peakComputationVariables, data, compArgs_caseSpecific )
	}

}

