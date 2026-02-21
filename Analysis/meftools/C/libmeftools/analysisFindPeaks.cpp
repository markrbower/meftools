/* analysisFindPeaks.cpp

Implements MEFanalysis interface to find peaks in MEF data.

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
#include "analysisFindPeaks.h"

    analysisFindPeaks::analysisFindPeaks( MEFiter iter_, CaseSpecificVariables csv, AlgorithmSpecificVariables asv ) : iter(iter_) {
        iter = iter_;

    }

    void MEFanalysis::setIterator( MEFiter iter_ ) {
        MEFanalysis::iter = iter_;
    }

    void MEFanalysis::compute() {
        // Iterate through the given section, find peaks, blackout, then store.
	while ( iter.hasNext() ) {
		data.push_back( iter.next() );
        	// Is the value 20 spots back a local peak?
		std::vector<int> y(x.end() - n, x.end());

    }

    void MEFanalysis::store() {
        // Check buffer, then store.

    }

    void MEFanalysis::performance() {
        // Implementation of MyFunction

    }

    void MEFanalysis::graph() {
        // Implementation of MyFunction

    }



