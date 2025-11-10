/*
		Ncs2Mef
 
 Program to convert NeuraLynx CSC (Continuously Sampled Record) files to mef files (v.2.1).
 
 Notes:
 -main.c acts as a wrapper for underlying functions.
 
 
 USAGE: Ncs2Mef data_file
 
 copyright 2011 Mayo Foundation 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <CoreFoundation/CFByteOrder.h>

//#include "size_types.h"
#ifndef OUTPUT_TO_MEF2
#include "meflib.h"
#else
#include "mef.h"
#endif
#include "Ncs2Mef.h"


int main (int argc, const char * argv[]) {
	int	update_mef_header(), convert_mvf(), mayo_encode(), convert_mef();
	int dataFailed = 0;
	int numFiles, uid, anon_flag;
	ui1	*uid_array;
	char subject_password[32], session_password[32];
	time_t start, end;
    ui8 uutc_time;
    int i;
    int nev_count, ncs_count;
	
	time(&start);
	
	if (argc < 2) 
	{
		(void) printf("USAGE: %s data_files (.ncs) [event_file (.nev)]\n", argv[0]);
		return(1);
	}
	
	//defaults
	numFiles = argc;
    uid = 1;
	anon_flag = 1;  // anonymize the data

    // check for input file
	if (numFiles==0) {
		fprintf(stderr, "Error: No file specified\n");
		return(1);
	}
	
	time(&start);
	
	// TBD revisit this
	/*if (uid) {
		uid_array = (ui1 *)calloc(sizeof(ui1), SESSION_UNIQUE_ID_LENGTH);
		if (generate_unique_ID(uid_array) == 0)
		{
			fprintf(stderr, "Error generating UID\n");
			return(1);
		}
	}
	else 
		uid_array = NULL;*/
	uid_array = NULL;
	
	
    //*subject_password = 0;
    //*session_password = 0;
strcpy(subject_password, "blah");
strcpy(session_password, "blah");
    
    uutc_time = 0;
    
    nev_count = 0;
    for (i=1;i<numFiles;i++)
    {
    
        if (!strstr(argv[i], ".nev") == NULL)
            nev_count++;
    }
    
    if (nev_count > 1)
    {
        fprintf(stderr, "More than one event (.nev) file is specified, please fix this so that there is only one event file!  Exiting.\n");
        return(1);
    }
    
    ncs_count = 0;
     for (i=1;i < numFiles;i++)
     {

         // main processing, this is where NCS is read and .MEF files are written
         if (!strstr(argv[i], ".ncs") == NULL)
         {
             ncs_count++;
             uutc_time = read_ncs_file((si1*) argv[i], (si1*) uid_array, session_password, subject_password, anon_flag, uutc_time);
         }
         // do nev in separate pass, so make sure they are done last
         //else
         //    read_nev_file((si1*) argv[i], "mef3", uutc_time);
     }
    
    if (ncs_count == 0)
    {
        fprintf(stderr, "There aren't any data (.ncs) files specified, so nothing to do!  Exiting.\n");
        return(1);
    }
    
    for (i=1;i<numFiles;i++)
    {
        if (!strstr(argv[i], ".nev") == NULL)
            read_nev_file((si1*) argv[i], "mef3", uutc_time);
        
    }
    
    if (dataFailed)
        return 1;

	time(&end);
	fprintf(stdout, "\nProcessing completed in %2.0lf seconds.\n", difftime(end, start) );
	
	return 0;
}
