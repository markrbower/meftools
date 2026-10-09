#ifndef ALGORITHM_SPECIFIC_FACTORY
#define ALGORITHM_SPECIFIC_FACTORY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

class AlgorithmSpecificFactory {
    private:
	map<string,string> S;
	map<string,double> D;
	map<string,int> I;
	map<string,long long> LL;
    public:
	void set( string, string );
	void set( string, double );
	void set( string, int );
	void set( string, long long );
	MEFinfo getMEFinfo();
        CircularBuffer getCircularBuffer();
        MEFcont getMEFconts();
        MEFinfo getMEFinfo();
        DatabaseAccessor getDatabaseAccessor();

};
#endif

