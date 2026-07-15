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
#include "MEFconts.h"
#include "MEFanalysis.h"

#include "CircularBuffer.h"
#include "CircularBufferMEF_allPeaks.h"

#include "FiltFilt.h"

class analysisFindPeaks : public MEFanalysis {
private:
    vector<long long> peakBuffer;
    vector<vector<double>> valuesBuffer;

public:
    CircularBufferMEF_allPeaks circbuf;

    analysisFindPeaks( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFinfo info_, MEFconts conts_, CircularBufferMEF_allPeaks cb_ );

    void compute();
    void performance();
    void store( vector<long long> peakBuffer, vector<vector<int>> valuesBuffer );
    void graph();
    void push_back( long long x );
    bool isPeak();
    vector<double> getBuffer();
    long long time();
};


