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


MEFanalysis::MEFanalysis( MEFiter iter_, CaseSpecificVariables case, ComputationSpecificVariables comp ) : info(info_) {
}


void MEFanalysis::setIterator( MEFiter iter ) {};
void MEFanalysis::compute() {};
void MEFanalysis::store() {};
void MEFanalysis::performance() {};
void MEFanalysis::graph() {};



