#ifndef __NCS_2_MEF
#define __NCS_2_MEF

//#include "size_types.h"
#ifndef OUTPUT_TO_MEF2
#include "meflib.h"
#else
#include "mef.h"
#endif

// Subroutine declarations
ui8 read_ncs_file(si1 *inFileName, si1 *uid, si1 *session_password, si1 *subject_password, si4 anonymize_flag, ui8 uutc_passed_in);
void uutc_time_from_date(sf8 yr, sf8 mo, sf8 dy, sf8 hr, sf8 mn, sf8 sc, ui8 *uutc_time);
si4 read_nev_file(si1 *inFileName, si1 *mef_path, ui8 time_correction_factor);


#define DISCARD_BITS			4
#define NCS_HEADER_SIZE         16384

#endif
