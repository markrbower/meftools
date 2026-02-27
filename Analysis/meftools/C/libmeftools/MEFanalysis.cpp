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
#include "MEFcont.h"


MEFanalysis::MEFanalysis( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFcont cont_ ) : cont(cont_) {
}

void MEFanalysis::compute() {};
void MEFanalysis::store() {};
void MEFanalysis::performance() {};
void MEFanalysis::graph() {};



