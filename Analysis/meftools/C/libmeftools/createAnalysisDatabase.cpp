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
#include "CircularBufferMEF.h"
#include "CircularBufferMEF_allPeaks.h"
#include "analysisFindPeaks.h"
#include "DatabaseAccessor.h"
#include <kfr/base.hpp>
#include <kfr/dft.hpp>
#include <kfr/dsp/iir_design.hpp>
#include <kfr/io/python_plot.hpp>


using namespace std;

void createAnalysisDatabase( const char* name ) {
	char* queryStr;

	DatabaseAccessor dba = DatabaseAccessor( name );

    	sprintf( queryStr, "create database if not exists %s;", name );
    	dba.runSQL( queryStr );

    	sprintf( queryStr, "use %s;", name );
    	dba.runSQL("use NPI;");

    	dba.runSQL("drop table if exists peaks;");

    	dba.runSQL("create table peaks (subject varchar(64), session varchar(64), time bigint, peakValue double, waveform varchar(2048));" );

	cout << "Database table created." << endl;
}

