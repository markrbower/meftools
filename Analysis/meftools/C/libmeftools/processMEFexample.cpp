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
#include "CircularBufferMEF_allPeaks.h"
#include "analysisFindPeaks.h"

MEF_HEADER_INFO read_mef_header(std::vector<std::string> strings);

void processMEFexample( string filename, string password, string subject, string session, int bufferSize, string signalType, int duration ) {
	std::vector<std::string> strings(2);
	strings[0] = filename;
	strings[1] = password;
	MEFinfo info = MEFinfo( filename, password );

// caseSpecificVariables
	CaseSpecificVariables caseSpecVar;
	caseSpecVar.filename = filename;
	caseSpecVar.bufferSize = bufferSize;

// algoComputationVariables
	AlgorithmSpecificVariables algoCompVar;
	algoCompVar.signalType = signalType;
	algoCompVar.duration = duration;

	// Constructor for analysis
	long long stepSize = info.getStepSize();
	CircularBufferMEF_allPeaks circbuf = CircularBufferMEF_allPeaks( 100, 0L, stepSize );
	MEFcont mefCont = MEFcont( info, bufferSize );
	analysisFindPeaks peaks = analysisFindPeaks( caseSpecVar, algoCompVar, info, mefCont, circbuf );
// Threshold
	peaks.compute();
	//peaks.store(); doesn't the analysis handle storage?
	peaks.performance();
	peaks.graph();

	// What do I do with "peaks" for subsequent analysis steps?
//	clusters = analysisNPI( it, caseSpecVar, clusterCompVars, peaks );
//	clusters.localGraph();
//	clusters.globalGraph();
//	clusters.performance();
//	clusters.graph();

// NPI
//        peaks <- NPI:::computePeaks( it, caseSpecVar, peakCompVar )

}

