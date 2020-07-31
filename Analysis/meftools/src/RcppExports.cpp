// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "../inst/include/meftools_types.h"
#include <Rcpp.h>

using namespace Rcpp;

// decomp_mef
std::vector<int> decomp_mef(Rcpp::StringVector strings);
RcppExport SEXP _meftools_decomp_mef(SEXP stringsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::StringVector >::type strings(stringsSEXP);
    rcpp_result_gen = Rcpp::wrap(decomp_mef(strings));
    return rcpp_result_gen;
END_RCPP
}
// get_discontinuities
Rcpp::NumericVector get_discontinuities(Rcpp::StringVector strings, Rcpp::NumericMatrix ToC);
RcppExport SEXP _meftools_get_discontinuities(SEXP stringsSEXP, SEXP ToCSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::StringVector >::type strings(stringsSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type ToC(ToCSEXP);
    rcpp_result_gen = Rcpp::wrap(get_discontinuities(strings, ToC));
    return rcpp_result_gen;
END_RCPP
}
// ncs2mef
void ncs2mef(Rcpp::StringVector strings);
RcppExport SEXP _meftools_ncs2mef(SEXP stringsSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::StringVector >::type strings(stringsSEXP);
    ncs2mef(strings);
    return R_NilValue;
END_RCPP
}
// read_mef_header
Rcpp::MEF_HEADER_INFO read_mef_header(Rcpp::StringVector strings);
RcppExport SEXP _meftools_read_mef_header(SEXP stringsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::StringVector >::type strings(stringsSEXP);
    rcpp_result_gen = Rcpp::wrap(read_mef_header(strings));
    return rcpp_result_gen;
END_RCPP
}
// table_of_contents
Rcpp::NumericMatrix table_of_contents(Rcpp::StringVector strings);
RcppExport SEXP _meftools_table_of_contents(SEXP stringsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::StringVector >::type strings(stringsSEXP);
    rcpp_result_gen = Rcpp::wrap(table_of_contents(strings));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_meftools_decomp_mef", (DL_FUNC) &_meftools_decomp_mef, 1},
    {"_meftools_get_discontinuities", (DL_FUNC) &_meftools_get_discontinuities, 2},
    {"_meftools_ncs2mef", (DL_FUNC) &_meftools_ncs2mef, 1},
    {"_meftools_read_mef_header", (DL_FUNC) &_meftools_read_mef_header, 1},
    {"_meftools_table_of_contents", (DL_FUNC) &_meftools_table_of_contents, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_meftools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}