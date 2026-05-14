#ifndef MEF_ANALYSIS
#define MEF_ANALYSIS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include "MEFcont.h"
#include "DatabaseAccessor.h"

using namespace std;

struct CaseSpecificVariables {
	string filename;
	string password;
	string subject;
	string session;
	int bufferSize;
};

struct AlgorithmSpecificVariables {
	string signalType;
	int threshold;
	int duration;
};

class MEFanalysis {
    private:
    public:
	CaseSpecificVariables csv;
	AlgorithmSpecificVariables asv;
        MEFcont cont;
	DatabaseAccessor dba;
	
	MEFanalysis();
	MEFanalysis( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFcont cont_ );
        virtual void compute();
        virtual void store( vector<long long> p, vector<vector<int>> v);
        virtual void performance();
        virtual void graph();
};


#endif

