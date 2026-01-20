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
#include "MEFanalysis.h"

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);

void processMEFexample( string filename, string password, int bufferSize, int threshold, int duration ) {
	MEFanalysis peaks;
	MEFanalysis clusters;

	std::vector<std::string> strings(2);
	strings[0] = filename;
	strings[1] = password;
	MEFinfo info = MEFinfo( filename, password );

// caseSpecificVariables
	caseSpecificVariables caseSpecVar;
	caseSpecVar.filename = filename
	caseSpecVar.bufferSize = bufferSize;

// peakComputationVariables
	peakComputationVariables peakCompVar;
	peakCompVar.threshold = threshold;
	peakCompVar.duration = duration;

	peaks = analysisFindPeaks( caseSpecVar, peakCompVars );
	MEFcont mefCont = MEFcont( filename, password, info, bufferSize );
	while ( mefCont.hasNext() ) {
        	MEFiter it = mefCont.next();


// Threshold
		peaks.setIterator( it );
		peaks.compute();
		peaks.store();
		peaks.performance();
		peaks.graph();

	// What do I do with "peaks" for subsequent analysis steps?
//		clusters = analysisNPI( it, caseSpecVar, clusterCompVars, peaks );
//		clusters.localGraph();
//		clusters.globalGraph();
//		clusters.performance();
//		clusters.graph();

// NPI
//	        peaks <- NPI:::computePeaks( it, caseSpecVar, peakCompVar )




	}

}

