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


class analysisFindPeaks: public MEFanalysis {
private:
    MEFiter iter;

public:
    analysisFindPeaks( MEFiter iter_, CaseSpecificVariables csv, AlgorithmSpecificVariables asv ) : iter(iter_) {
        iter = iter_;

    }

    void setIterator( MEFiter iter_ ) {
        iter = iter_;
    }

    void compute() {
        // Iterate through the given section, find peaks, blackout, then store.
        // Add peak times to a vector.

    }

    void store() {
        // Check buffer, then store.

    }

    void performance() {
        // Implementation of MyFunction

    }

    void graph() {
        // Implementation of MyFunction

    }

};


