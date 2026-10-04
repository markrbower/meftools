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

void findEvents( string filename, string password, string subject, string session, int bufferSize, string signalType, int duration ) {
	char queryStr[256];

	DatabaseAccessor dba = DatabaseAccessor( name );

	probably just "processMEFexample(...)";

}

