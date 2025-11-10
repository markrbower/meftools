
/***********************************************************************************/
/*** This version runs on little endian machines and writes out big endian files ***/
/***********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>

//#include "size_types.h"
#include "Ncs2Mef.h"
#ifndef OUTPUT_TO_MEF2
#include "write_mef_channel.h"
#else
#include "write_mef_channel_mef2.h"
#endif
//#include "MefChannelWriter.h"

#ifndef OUTPUT_TO_MEF2
#include "meflib.h"
#else
#include "mef.h"
#endif

#define DBUG 1
#define SECS_PER_BLOCK 1

si4	ctrl_c_hit;

#ifdef OUTPUT_TO_MEF2
void pack_mef_header(MEF_HEADER_INFO *out_header_struct, sf8 secs_per_block, si1 *subject_password, si1 *session_password, 
                     si1 *uid, si4 anonymize_flag, si4 dst_flag, si4 bit_shift_flag, sf8 sampling_frequency)
{
   	si8 l;
    sf8 voltage_conversion_factor;
    char anonymized_subject_name[ANONYMIZED_SUBJECT_NAME_LENGTH];
    
    // useful constants
    voltage_conversion_factor = 1.0;  
    
    // Set header fields
    strcpy(out_header_struct->institution, "Mayo Clinic, Rochester, MN, USA");
    strcpy(out_header_struct->unencrypted_text_field, "not entered");
    sprintf(out_header_struct->encryption_algorithm,  "AES %d-bit", ENCRYPTION_BLOCK_BITS);
    
    if (*subject_password)
		out_header_struct->subject_encryption_used = 1;
	else
		out_header_struct->subject_encryption_used = 0;
	
	if (*session_password)
		out_header_struct->session_encryption_used = 1;
	else
		out_header_struct->session_encryption_used = 0;
	
	out_header_struct->subject_encryption_used = 0;
	out_header_struct->session_encryption_used = 0;
	out_header_struct->data_encryption_used = 0; //don't use data encryption by default
    out_header_struct->byte_order_code = 1;      // little-endian
    out_header_struct->header_version_major = (ui1)HEADER_MAJOR_VERSION;
    out_header_struct->header_version_minor = (ui1)HEADER_MINOR_VERSION;
    
    if (uid == NULL)
		memset(out_header_struct->session_unique_ID, 0, SESSION_UNIQUE_ID_LENGTH); //set uid to zeros
	else
		memcpy(out_header_struct->session_unique_ID, uid, SESSION_UNIQUE_ID_LENGTH);
    
    out_header_struct->header_length =  (ui2) MEF_HEADER_LENGTH;
	
    if (anonymize_flag) {
		sprintf(out_header_struct->subject_first_name, "Firstname");
		sprintf(out_header_struct->subject_second_name, "Middlename");
		sprintf(out_header_struct->subject_third_name, "Lastname");
		sprintf(out_header_struct->subject_id, "00-000-000");

	}
    else
    {
        fprintf(stderr, "Subject first name: "); 
        fgets(out_header_struct->subject_first_name, SUBJECT_FIRST_NAME_LENGTH, stdin);
        l = strlen(out_header_struct->subject_first_name) - 1; 
        out_header_struct->subject_first_name[l] = 0;
    
        fprintf(stderr, "Subject second name: "); 
        fgets(out_header_struct->subject_second_name, SUBJECT_SECOND_NAME_LENGTH, stdin);
        l = strlen(out_header_struct->subject_second_name) - 1; 
        out_header_struct->subject_second_name[l] = 0;
    
        fprintf(stderr, "Subject third name: "); 
        fgets(out_header_struct->subject_third_name, SUBJECT_THIRD_NAME_LENGTH, stdin);
        l = strlen(out_header_struct->subject_third_name) - 1; 
        out_header_struct->subject_third_name[l] = 0;
    
        fprintf(stderr, "Subject ID: "); 
        fgets(out_header_struct->subject_id, SUBJECT_ID_LENGTH, stdin);
        l = strlen(out_header_struct->subject_id) - 1; 
        out_header_struct->subject_id[l] = 0;
    }
    
    strncpy2(out_header_struct->session_password, session_password, SESSION_PASSWORD_LENGTH);
    // Subject_validation_field      is filled in later by build_mef_header_block().
    // Session_validation_field      is filled in later by build_mef_header_block().
    // Protected_Region              is filled in later after build_mef_header_block().
    // Number_of_samples             is filled in later
    // Channel_name                  is filled in later
    // Recording_start_time          is filled in later
    // Recording_end_time            is filled in later
    out_header_struct->low_frequency_filter_setting  = 0; // TBD: are these filter settings correct?
    out_header_struct->high_frequency_filter_setting = DMA_HIGH_FREQUENCY_FILTER;
    out_header_struct->notch_filter_frequency        = 0;
    
    out_header_struct->sampling_frequency = sampling_frequency;
    out_header_struct->voltage_conversion_factor = voltage_conversion_factor;
    if (bit_shift_flag)
        out_header_struct->voltage_conversion_factor *= 4;
    strcpy(out_header_struct->acquisition_system, "Digital Lynx");
    // Channel_comments field        is filled in later
    
    //fprintf(stderr, "Study comments: "); 
    //fgets(out_header_struct->study_comments, STUDY_COMMENTS_LENGTH, stdin);
	//l = strlen(out_header_struct->study_comments) - 1; 
    //out_header_struct->study_comments[l] = 0;
    //if (out_header_struct->study_comments[0] == 0)
    
        strcpy((char*)&(out_header_struct->study_comments), "not entered");
    
    
    // Physical_channel_number       is filled in later
	out_header_struct->physical_channel_number = 0;
    strcpy(out_header_struct->compression_algorithm, "Range Encoded Differences (RED)");
    // Maximum_compressed_block_size is filled in later
    // Maximum_block_length          is filled in later
    // Block_interval                is filled in later
	out_header_struct->block_interval = (ui8) ((secs_per_block * 1000000.0) + 0.5);
    // Maximum_data_value            is filled in later
    // Minimum_data_value            is filled in later
    // Index_data_offset             is filled in later
    // Number_of_index_entries       is filled in later
    out_header_struct->block_header_length = BLOCK_HEADER_BYTES;
    if (dst_flag)
        out_header_struct->GMT_offset = -5.0;
    else
        out_header_struct->GMT_offset = -6.0;  
    // Discontinuity index           is filled in later
    // Number of discontinuities     is filled in later
    // File Uid                      is filled in later
    memset(anonymized_subject_name, 0, ANONYMIZED_SUBJECT_NAME_LENGTH);  // leave blank for now
    strcpy(out_header_struct->anonymized_subject_name, anonymized_subject_name);
    // Header CRC                    is filled in later
    
    
}
#endif

ui8 read_ncs_file(si1 *inFileName, si1 *uid, si1 *subject_password, si1 *session_password, si4 anonymize_flag, ui8 uutc_passed_in)
{
    ui8 nr, flen, num_bytes_read, timestamp;
    FILE *infile;
    si4 fd;
    struct stat	sb;
    char header_string[1024];
    char *string_ptr1, *string_ptr2;
    char temp_string[128];
    si2 sample_buffer[512];
    //si2 *sample_buffer;
    si2 *sample_buffer_ptr;
    si4 samps[1024];
    ui8 string_len;
    sf8 month, day, year, hour, minute, second;
    ui8 uutc_time, *uutc_time_ptr;
#ifdef OUTPUT_TO_MEF2
    MEF_HEADER_INFO out_header_struct;
#endif
    ui4 record_frequency;
    ui4 num_valid_samples;
    int i;
    ui8 temp_timestamp, last_temp_timestamp;
    ui8 saved_start_time;
#ifdef OUTPUT_TO_MEF2
    PACKET_TIME	packet_times[1024];
#endif
    //ui8 *timestamps;
    ui8 timestamps[1024];
	CHANNEL_STATE *channel_state_struct;
	//SESSION_STATE *session_state_struct;
#ifndef OUTPUT_TO_MEF2
	extern MEF_GLOBALS	*MEF_globals;
#endif
    si1 dir_name[1024];
    si1 chan_name[1024];
    si1 *ext;
        int month_format_type;
    
    num_bytes_read = 0;
    last_temp_timestamp = 0;
    saved_start_time = 0;
    
#ifndef OUTPUT_TO_MEF2
	// set up mef 3 library
	(void) initialize_meflib();
    MEF_globals->recording_time_offset_mode = RTO_IGNORE;
	//MEF_globals->verbose = MEF_TRUE;
	//MEF_globals->number_of_records = 0;
#endif
    

    fprintf(stderr, "inFileName = %s\n", inFileName);
    
    //extract_path_parts(inFileName, dir_name, chan_name, "ncs");
    
    
    strcpy(chan_name, inFileName);
    
    ext = strrchr((si1 *) chan_name, '.');
    if (ext != NULL)
        *ext = 0;
    
#ifndef OUTPUT_TO_MEF2
    sprintf(dir_name, "mef3");
#else
    sprintf(dir_name, "mef2");
    system("mkdir mef2");
#endif

	
    
    fprintf(stderr, "dir_name = %s chan_name = %s\n", dir_name, chan_name);


#ifndef OUTPUT_TO_MEF2
	channel_state_struct = (CHANNEL_STATE*) calloc((size_t) 1, sizeof(CHANNEL_STATE));
    
    initialize_mef_channel_data(channel_state_struct,
                                SECS_PER_BLOCK,           // seconds per block
                                chan_name  , // channel name
                                0,// bit shift flag, set to 1 for neuralynx, to chop off 2 least-significant sample bits
                                0.0,           // low filt freq
                                9000.0,        // high filt freq
                                -1.0,           // notch filt freq
                                60.0,          // AC line freq
                                1,           // units conversion factor
                                "not_entered ",// chan description
                                32000, // starter freq for channel, make it as high or higher than actual freq to allocate buffers
                                SECS_PER_BLOCK * 1000000, // block interval, needs to be correct, this value is used for all channels
                                0,             // chan number
                                dir_name,      // absolute path of session
                                -6.0,                  // GMT offset
                                "not_entered",        // session description
                                "not_entered",                // anonymized subject name
                                "not_entered",         // subject first name
                                "not_entered",                 // subject second name
                                "0-000-000",               // subject ID
                                "Mayo Clinic, Rochester, MN, USA",           // institution
                                NULL,  // for now unencrypted
                                NULL,  // for now unencrypted
                                //globals->anonymize_output ? NULL : "level_1_pass",                  // level 1 password (technical data)
                                //globals->anonymize_output ? NULL : "level_2_pass",               // level 2 password (subject data), must also specify level 1 password if specifying level 2
                                "not_entered",        // study comments
                                "not_entered",         // channel comments
                                0                      // secs per segment, 0 means no limit to segment size
                                );
    
#else
    
    
    
    
    pack_mef_header(&out_header_struct, 1.0, subject_password, session_password, uid, anonymize_flag, 0, 0, 0.0);
    out_header_struct.sampling_frequency = 32556.0;  // give it something big so it allocates enough space
    
    
    channel_state_struct = (CHANNEL_STATE*) calloc((size_t) 1, sizeof(CHANNEL_STATE));
    
    initialize_mef_channel_data( channel_state_struct, &out_header_struct, SECS_PER_BLOCK,
                                chan_name, NULL,
                                0, dir_name, 0);

    
#endif
	
    // open .Ncs file
    infile = fopen(inFileName, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening .Ncs file\n");
        return (1);
    }
	fd = fileno(infile);
	fstat(fd, &sb);
	flen = sb.st_size;     // flen in bytes
    
    // find "time opened" timestamp from header
    nr = fread(header_string, sizeof(ui1), 1024, infile);
    if (infile == NULL) 
    { 
        fprintf(stderr, "Error reading .Ncs header\n"); 
        return (1); 
    }
    
    
    
    
    
    /*
    
    // point to beginning of date string
    string_ptr1 = strstr(header_string, "Time Opened: (m/d/y):");
    if (string_ptr1 == NULL)
    {
        fprintf(stderr, "Error reading open date 1 from .Ncs header\n");  
        return (1);
    
    }

    string_ptr1 += strlen("Time Opened: (m/d/y): ");
    
    // extract month
    string_ptr2 = strchr(string_ptr1, '/');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    month = atof(temp_string);
    
    // extract day
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, '/');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    day = atof(temp_string);
    
    // extract year
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, ' ');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    year = atof(temp_string);
    
    string_ptr1 = strstr(header_string, "At Time: ");
    if (string_ptr1 == NULL)
    {
        fprintf(stderr, "Error reading open date 2 from .Ncs header\n");  
        return (1);
        
    }

    string_ptr1 += strlen("At Time: ");
    
    // extract hour
    string_ptr2 = strchr(string_ptr1, ':');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    hour = atof(temp_string);

    // extract minute
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, ':');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    minute = atof(temp_string);
    
    // extract second
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, ' ');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    second = atof(temp_string);
    
    
    */
    
    
    
    
    
    
    
    
    
    // point to beginning of date string
    month_format_type = 1;
    string_ptr1 = strstr(header_string, "-TimeCreated ");
    
    fprintf(stderr, "length header = %d\n", strlen(header_string));
    
    /*
     if (string_ptr1 == NULL)
     {
     string_ptr1 = strstr(header_string, "Time Opened: (m/d/y):");
     if (string_ptr1 == NULL)
     {
     fprintf(stderr, "Error reading open date 1 from .Ncs header\n");
     return (1);
     }
     
     month_format_type = 2;
     
     }
     
     if (month_format_type == 1)
     string_ptr1 += strlen("Time Opened (m/d/y): ");
     else
     string_ptr1 += strlen("Time Opened: (m/d/y): ");
     
     */
    
    string_ptr1 += strlen ("-TimeCreated ");
    
    // extract year
    string_ptr2 = strchr(string_ptr1, '/');
    //string_ptr1 = string_ptr2 + 1;
    //string_ptr2 = strchr(string_ptr1, ' ');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    year = atof(temp_string);
    
    // extract month
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, '/');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    month = atof(temp_string);
    
    // extract day
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, ' ');
    //string_ptr2 = string_ptr
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    day = atof(temp_string);
    
    
    
    /* string_ptr1 = strstr(header_string, "(h:m:s.ms) ");
     format_type = 1;
     if (string_ptr1 == NULL)
     {
     string_ptr1 = strstr(header_string, "At Time: ");
     if (string_ptr1 == NULL)
     {
     fprintf(stderr, "Error reading open date 2 from .Ncs header\n");
     return (1);
     }
     format_type = 2;
     
     }
     
     if (format_type == 1)
     string_ptr1 += strlen("(h:m:s.ms) ");
     if (format_type == 2)
     string_ptr1 += strlen("At Time: ");
     */
    
    string_ptr1 = strstr(string_ptr2, " ");
    
    // extract hour
    string_ptr2 = strchr(string_ptr1, ':');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    hour = atof(temp_string);
    
    fprintf(stderr, "hour = %f\n", hour);
    
    // extract minute
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = strchr(string_ptr1, ':');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    minute = atof(temp_string);
    
    // extract second
    string_ptr1 = string_ptr2 + 1;
    string_ptr2 = string_ptr1 + 2;
    //string_ptr2 = strchr(string_ptr1, ' ');
    string_len = string_ptr2 - string_ptr1;
    memcpy(temp_string, string_ptr1, string_len);
    temp_string[string_len] = 0;
    second = atof(temp_string);
    
    fprintf(stderr, "second = %f\n", second);

    
  
    
    // create micro UTC time, assume central standard time zone for Mayo
    uutc_time_ptr = &uutc_time;
    uutc_time_from_date(year, month, day, hour, minute, second, uutc_time_ptr);
    
    fprintf(stderr, "UUTC time is %ld  ", uutc_time);
    
    if (uutc_passed_in > 0)
        *uutc_time_ptr = uutc_passed_in;
    
    fprintf(stderr, "Using time %ld ", *uutc_time_ptr);
    
    fseek(infile, NCS_HEADER_SIZE, SEEK_SET);
    num_bytes_read = NCS_HEADER_SIZE;
    
    inFileName[strlen(inFileName)-4] = '\0';
    
    //sample_buffer = (si2*) calloc((size_t) 1024, sizeof(si2));

	
    // we don't yet know the sampling frequency, so we'll give it something that is big,
    // so large buffers will be allocated.  We'll fix this later with setSamplingFrequency().
    //MefChannelWriter channel_writer(inFileName, NULL, 1.0, 32556.0);


    while (num_bytes_read < flen)
    {
        // read event timestamp
        nr = fread(&timestamp, sizeof(ui8), (size_t) 1, infile);
        num_bytes_read += sizeof(ui8);
        if (nr != 1)
        {
            fprintf(stderr, "Error reading timestamp from .Ncs file\n"); 
            exit(1); 
        }   
        
        fseek(infile, 4, SEEK_CUR);
        num_bytes_read += 4;
        
        // read frequency
        nr = fread(&record_frequency, sizeof(ui4), (size_t) 1, infile);
        num_bytes_read += sizeof(ui4);
        if (nr != 1)
        { 
            fprintf(stderr, "Error reading frequency from .Ncs file\n"); 
            exit(1); 
        } 
       // fprintf(stderr, "sampling frequency = %d\n", record_frequency);
        //channel_writer.setSamplingFrequency(record_frequency);
		// TBD replace this
#ifdef OUTPUT_TO_MEF2
		out_header_struct.sampling_frequency = record_frequency;
#endif
        
        // read num_valid_samples
        nr = fread(&num_valid_samples, sizeof(ui4), (size_t) 1, infile);
        num_bytes_read += sizeof(ui4);
        if (nr != 1)
        { 
            fprintf(stderr, "Error reading number of valid samples from .Ncs file\n"); 
            exit(1); 
        } 
//fprintf(stderr, "num_valid_samples = %d\n", num_valid_samples);
 
        nr = fread(sample_buffer, sizeof(si2), (size_t) 512, infile);
        if (nr != 512)
        { 
            fprintf(stderr, "Error sample buffer from .Ncs file\n"); 
            exit(1);
        } 
        num_bytes_read += 1024;
        
        temp_timestamp = timestamp + *uutc_time_ptr;/* - 3600000000;*/ // adjust 1 hour for daylight savings, this is not necessary
        if (temp_timestamp < last_temp_timestamp + 512.0 * ((1.0/record_frequency) * 1000000.0))
        {
           // fprintf(stderr, "messed up timestamps? %ld, %ld, %ld\n",last_temp_timestamp, temp_timestamp, timestamp);
        
        }
        last_temp_timestamp = temp_timestamp;
        
        sample_buffer_ptr = sample_buffer;
        
        if (num_valid_samples > 512)
            fprintf(stderr, "invalid num_valid_samples... \n");
        
        // iterate through the samples we just read, and assign timestamps to each.
        for (i=0;i<num_valid_samples;i++)
        {
            timestamps[i] = temp_timestamp;
#ifdef OUTPUT_TO_MEF2
            packet_times[i].timestamp = temp_timestamp;
#endif
            //samps[i] = (si4) *sample_buffer_ptr;
            samps[i] =  sample_buffer[i];
            //fprintf(stderr, "ample %d: %d\n", i, samps[i]);
            
            //sample_buffer_ptr++;
            
            if (saved_start_time == 0)
                saved_start_time = temp_timestamp;
            
            temp_timestamp += ((1.0 / record_frequency) * 1000000.0);
        }
 
        //channel_writer.writeData(packet_times, samps, num_valid_samples);

		/*write_mef_channel_data(&session_state_struct->channels[0],
							   packet_times, 
							   samps, 
							   num_valid_samples, 
                               1.0,
							   (sf8) record_frequency);   // this sampling frequency value needs to be correct*/
        
#ifndef OUTPUT_TO_MEF2
         write_mef_channel_data(channel_state_struct, timestamps, samps, num_valid_samples, SECS_PER_BLOCK, record_frequency);
#else
         write_mef_channel_data(channel_state_struct, &out_header_struct, packet_times, samps, num_valid_samples, SECS_PER_BLOCK);
#endif

        
    }

    
	// TBD revisit this
    //out_header_struct.recording_start_time = saved_start_time;


    // finish and close .mef file write
    //channel_writer.close(&out_header_struct, subject_password);
	//close_mef_channel_file(&session_state_struct->channels[0], /*&out_header_struct,*/ session_password, subject_password, 1.0);
    
    
#ifndef OUTPUT_TO_MEF2
	close_mef_channel(channel_state_struct);
#else
    bzero(out_header_struct.channel_name, CHANNEL_NAME_LENGTH);
    strcpy(out_header_struct.channel_name, "chan name");
    strcpy((char*)&(out_header_struct.channel_comments), "not entered");
    out_header_struct.recording_start_time = saved_start_time;
    close_mef_channel_file(channel_state_struct, &out_header_struct, NULL, NULL, SECS_PER_BLOCK);
#endif
 
    fclose(infile);
    
    return *uutc_time_ptr;
}

void uutc_time_from_date(sf8 yr, sf8 mo, sf8 dy, sf8 hr, sf8 mn, sf8 sc, ui8 *uutc_time)
{
	struct tm	tm;
	time_t		UTC_secs;
	long		gm_offset;
	char		timestr[30];
	
	tm.tm_sec = (int) sc;
	tm.tm_min = (int) mn;
	tm.tm_hour = (int) hr;
	tm.tm_mday = (int) dy;
	tm.tm_mon = (int) (mo - 1.0);
	tm.tm_year = (int) (yr - 1900.0);
	
    gm_offset = -6;
	tm.tm_gmtoff = gm_offset * 3600;
	
	tzset();
	fflush(stdout);
	//	fprintf(stdout, "tm_hr %d\ttm_gmtoff %d\n", tm.tm_hour, tm.tm_gmtoff);
	UTC_secs = mktime(&tm);
	fprintf(stdout, "time %s", ctime_r(&UTC_secs, timestr));
	
	*uutc_time = (unsigned long long) (UTC_secs - (int) sc) * 1000000;
	*uutc_time += (unsigned long long) ((sc * 1000000.0) + 0.5);
	
	return;
}


#define NEV_HEADER_SIZE         16384


//si4 read_nev_file(si1 *inFileName, ui8 *file_recording_offset, si1 *subject_first_name, si1 *subject_third_name, si1 *subject_id, si1 *mef_path, ui8 time_correction_factor)
si4 read_nev_file(si1 *inFileName, si1 *mef_path, ui8 time_correction_factor)

{
    ui8 nr, num_events, flen, num_bytes_read, timestamp;
    FILE *infile, *ofp;
    si4 fd;
    struct stat	sb;
    char event_string[128];
    char header_string[1024];
    char *string_ptr1, *string_ptr2;
    char temp_string[128];
    char outFileName[512];
    ui8 string_len;
    sf8 month, day, year, hour, minute, second;
    ui8 uutc_time, *uutc_time_ptr, starting_recording_time;
#ifndef OUTPUT_TO_MEF2
    ANNOTATION_STATE *annotation_state_struct;
#endif
    
    num_events = 0;
    num_bytes_read = 0;
    starting_recording_time = 0;
    //*file_recording_offset = 0;
    
    // open .NEV file
    infile = fopen(inFileName, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening .NEV file\n");
        return (1);
    }
    fd = fileno(infile);
    fstat(fd, &sb);
    flen = sb.st_size;     // flen in bytes
    
    fprintf(stderr, "In NEV processing\n");
    
    
    
    /*
    
    
    
    // read through infile until we find the first "Starting Recording", we'll use this in the episode tag
    // skip over .NEV header bytes
    fseek(infile, NEV_HEADER_SIZE, SEEK_SET);
    num_bytes_read = NEV_HEADER_SIZE;
    
    while (num_bytes_read < flen)
    {
        
        // skip over first 3 16-bit ints of event record
        fseek(infile, 6, SEEK_CUR);
        num_bytes_read += 6;
        
        // read event timestamp
        nr = fread(&timestamp, sizeof(ui8), (size_t) 1, infile);
        num_bytes_read += sizeof(ui8);
        if (nr != 1)
        {
            fprintf(stderr, "Error reading timestamp from .NEV file\n");
            exit(1);
        }
        
        // skip over next 6 event record fields
        fseek(infile, 42, SEEK_CUR);
        num_bytes_read += 42;
        
        nr = fread(event_string, sizeof(ui1), (size_t) 128, infile);
        num_bytes_read += 128;
        if (nr != 128)
        {
            fprintf(stderr, "Error reading eventstring from .NEV file\n");
            exit(1);
        }
        
        
        if (!strcmp(event_string, "Starting Recording"))
        {
            // we only care about the first "starting recording"
            if (starting_recording_time == 0)
            {
                starting_recording_time = timestamp;
                break;
            }
        }
    }
    
    if (starting_recording_time != 0)
    {
        *file_recording_offset = uutc_time + starting_recording_time;
        fprintf(stderr, "Found recording start time from .NEV file.\n");
        fflush(stderr);
    }
    else
    {
        // recording time not found from .NEV file,
        // do not process .NEV file further
        return(1);
    }
    
    */
    
    // Read infile again, this time to record all events in .maf file
    // open output file and write initial tags
   /* if (mef_path != NULL)
        sprintf(outFileName, "%s/%s_%s.maf", mef_path, subject_third_name, subject_id);
    else
        sprintf(outFileName, "%s_%s.maf", subject_third_name, subject_id);
    ofp = fopen(outFileName, "w");
    if (ofp == NULL) {
        fprintf(stderr, "%s: Error opening %s for writing. \n", __FUNCTION__, outFileName);
        return(1);
    }*/
    
   // fprintf(ofp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<XREDE>\n");
    //fprintf(ofp, "<Dataset id=\"1\">\n");
    
    //write subject information
    //fprintf(ofp, "<Subject name_first=\"%s\" name_last=\"%s\" Subject_nbr=\"%s\" id=\"1\">\n",
     //       subject_first_name, subject_third_name, subject_id);
   // fprintf(ofp, "\t<Episode recording_start_time=\"%lu\" time_units=\"uUTC\" id=\"1\" SubjectID=\"1\">\n", *file_recording_offset);
    
    // skip over .NEV header bytes
    fseek(infile, NEV_HEADER_SIZE, SEEK_SET);
    num_bytes_read = NEV_HEADER_SIZE;
    
#ifndef OUTPUT_TO_MEF2
    sprintf(outFileName, "%s.mefd/events.txt", mef_path);
#else
    sprintf(outFileName, "mef2/events.txt", mef_path);
#endif
    ofp = fopen(outFileName, "w");
    
    
    /********************************  RECORDS ***************************************/
    
    // The following code is a simple demonstration of writing Note records.
    //
    // After the close_annotation() command, create_or_append_annotations() could be called again
    // and new records could then be appended to the same records files.
    
    
#ifndef OUTPUT_TO_MEF2
    // allocate struct
    annotation_state_struct = (ANNOTATION_STATE*) calloc((size_t) 1, sizeof(ANNOTATION_STATE));
    
    // create records (annotations) files
    create_or_append_annotations(annotation_state_struct, mef_path, -6.0, "not_entered");
    
#endif
    
    // manually write two "Note" type records
    //write_annotation(annotation_state_struct, 946684800000000, "Note", 0, "This is the text of the first note.");
    //write_annotation(annotation_state_struct, 946684801000000, "Note", 0, "This is the text of the second note.");
    

    
    /********************************  END OF RECORDS ********************************/
    
    
    while (num_bytes_read < flen)
    {
        // skip over first 3 16-bit ints of event record
        fseek(infile, 6, SEEK_CUR);
        num_bytes_read += 6;
        
        // read event timestamp
        nr = fread(&timestamp, sizeof(ui8), (size_t) 1, infile);
        num_bytes_read += sizeof(ui8);
        if (nr != 1)
        {
            fprintf(stderr, "Error reading timestamp from .NEV file\n");
            fflush(stderr);
            exit(1);
        }
        //fprintf(stdout,"Timestamp: %ld ", timestamp);
        //fflush(stderr);
        
        // skip over next 6 event record fields
        fseek(infile, 42, SEEK_CUR);
        num_bytes_read += 42;
        
        nr = fread(event_string, sizeof(ui1), (size_t) 128, infile);
        num_bytes_read += 128;
        if (nr != 128)
        {
            fprintf(stderr, "Error reading eventstring from .NEV file\n");
            fflush(stderr);
            exit(1);
        }
        
        //updateMotorHexToDec((char*)event_string);
        
        //fprintf(stdout,"Event String: %s\n", event_string);
        //fflush(stderr);
        fprintf(stderr, "Event: %ld %s\n", timestamp + time_correction_factor, event_string);
        fprintf(ofp, "%ld, %s\n", timestamp + time_correction_factor, event_string);
#ifndef OUTPUT_TO_MEF2
        write_annotation(annotation_state_struct, timestamp + time_correction_factor, "Note", 0, event_string);
#endif
        
        num_events++;
       // fprintf(ofp, "\t\t<Event type=\"%s\" id=\"%ld\" EpisodeID=\"1\">\n", event_string, num_events);
       // fprintf(ofp, "\t\t\t<Timestamp onset=\"%ld\" id=\"%ld\" EventID=\"%ld\"/>\n", time_correction_factor + timestamp, num_events, num_events);
       // fprintf(ofp, "\t\t</Event>\n");
    }
    
   // fprintf(ofp, "\t\t</Episode>\n");
   // fprintf(ofp, "\t</Subject>\n");
   // fprintf(ofp, "\t<Task name=\"acquisition event import\" id=\"1\" />\n");
   // fprintf(ofp, "\t</Dataset>\n</XREDE>\n");
    
    fclose(infile);
    fclose(ofp);
    
#ifndef OUTPUT_TO_MEF2
    // close records files
    close_annotation(annotation_state_struct);

    // free allocated data
    free (annotation_state_struct);
    
#endif
    
    return 0;
}
