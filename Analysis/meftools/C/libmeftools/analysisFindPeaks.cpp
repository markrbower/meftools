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



class analysisFindPeaks : public MEFanalysis {
public:
    analysisFindPeaks( caseSpecificVariables csv, peakComputationVariables pcv ) {

    }

    void setIterator( MEFiter iter_ ) override {
        iter = iter_;
    }

    void compute() override {
        // Implementation of MyFunction
    }

    void store() override {
        // Implementation of MyFunction
    }

    void performance() override {
        // Implementation of MyFunction
    }

    void graph() override {
        // Implementation of MyFunction
    }

};


