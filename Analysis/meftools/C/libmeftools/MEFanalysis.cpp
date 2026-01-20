/* MEFanalysis.cpp

An interface for analyses.

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



class MEFanalysis {
public:
	virtual void setIterator( MEFiter iter ) = 0;
	virtual void compute() = 0;
	virtual void store() = 0;
	virtual void performance() = 0;
	virtual void graph() = 0;
}





