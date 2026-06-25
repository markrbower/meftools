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

MEFanalysis::MEFanalysis( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFinfo info_, MEFcont cont_, DatabaseAccessor dba_ ) : info(info_), cont(cont_), dba(dba_) {
}

void MEFanalysis::compute() {};
void MEFanalysis::performance() {};
void MEFanalysis::graph() {};



