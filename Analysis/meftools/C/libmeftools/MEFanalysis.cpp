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
#include "MEFanalysis.h"


MEFanalysis::MEFanalysis( MEFiter iter_, CaseSpecificVariables csv, AlgorithmSpecificVariables asv ) : iter(iter_) {
}


void MEFanalysis::compute() {};
void MEFanalysis::store() {};
void MEFanalysis::performance() {};
void MEFanalysis::graph() {};



