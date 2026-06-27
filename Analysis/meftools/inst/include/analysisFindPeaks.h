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

#include "CircularBuffer.h"
#include "CircularBufferMEF_allPeaks.h"

#include "FiltFilt.h"

class analysisFindPeaks: public MEFanalysis {
private:
    vector<long long> peakBuffer;
    vector<vector<double>> valuesBuffer;

public:
    static CircularBufferMEF circbuf;

    analysisFindPeaks( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFinfo info_, MEFcont cont_, CircularBuffer cb_ );
    analysisFindPeaks( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFinfo info_, MEFcont cont_, CircularBufferMEF_allPeaks cb_ );

    virtual void compute();
    void push_back( long long x );
    bool isPeak();
    vector<double> getBuffer();
    long long time();
};


