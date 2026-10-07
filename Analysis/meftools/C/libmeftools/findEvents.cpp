/* findEvents.cpp

Find time-frequency-bounded events.

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
#include "CircularBufferMEF.h"
#include "CircularBufferMEF_allPeaks.h"
#include "analysisFindPeaks.h"
#include "DatabaseAccessor.h"
#include <kfr/base.hpp>
#include <kfr/dft.hpp>
#include <kfr/dsp/iir_design.hpp>
#include <kfr/io/python_plot.hpp>

using namespace std;

void findEvents( AlgoithmSpecificFactory asf ) {
	char queryStr[256];

	DatabaseAccessor dba = asf.getDatabaseAccessor();
        MEFinfo info = asf.getMEFinfo();
        CircularBufferMEF_allPeaks circbuf = asf.getCircularBuffer( 51, 0L );
        MEFconts mefConts = asf.getMEFconts();

        analysisFindPeaks peaks = analysisFindPeaks( caseSpecVar, algoCompVar, info, mefConts, circbuf );
        cout << "analysisFindPeaks object constructed" << endl;
        peaks.compute();

}

