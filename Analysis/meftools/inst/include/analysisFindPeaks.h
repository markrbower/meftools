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


class analysisFindPeaks: private MEFanalysis {
private:
    MEFcont cont;
    CircularBuffer circbuf;
    vector<long long> peakBuffer;
    vector<vector<int>> valuesBuffer;

public:
    analysisFindPeaks( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFcont cont_, CircularBuffer cb_ );
    void compute();
    void push_back( long long x );
    bool isPeak();
    vector<int> getBuffer();
    long long time();
};


