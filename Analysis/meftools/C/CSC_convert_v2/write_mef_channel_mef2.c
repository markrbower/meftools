/**********************************************************************************************************************
 
 This library contains functions to convert data samples to MEF version 2.1.  
 initialize_mef_channel_data() should be called first for each channel, which initializes the data in the channel
 structure.  Then write_mef_channel_data() is called with the actual sample data to be written to the mef.  Finally,
 close_mef_channel_file() will close out the channel mef file, and free allocated memory.
 
 To compile for a 64-bit intel system, linking with the following files is necessary:  
 mef_lib.c endian_functions.c RED_encode.c AES_encryption.c crc_32.c
 
 This software is made freely available under the GNU public license: http://www.gnu.org/licenses/gpl-3.0.txt
 
 Thanks to all who acknowledge the Mayo Systems Electrophysiology Laboratory, Rochester, MN
 in academic publications of their work facilitated by this software.
 
 Written by Dan Crepeau 9/2011
 
 Minor changes added 2/2012 to allow temporary index files to be written.
 *************************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>

//#include "recordmef.h"
#include "mef.h"
#include "write_mef_channel_mef2.h"




extern ui1 local_mef_path[1024];

//extern fftw_plan	global_pf32k;
//extern fftw_plan	global_pf5k;
//extern fftw_plan	global_pf1k;

extern ui1 line_noise_threshold;

extern si4 quality_test_num;

extern si1		**chan_map;

//ui1	line_noise_score(CHANNEL_STATE *channel_state, si4 *in_samps, si8 n_samps, sf8 samp_freq, sf8 line_freq, si4 normal_block_size);




// this creates a single static data block for use by all channels
// this assumes all channels will need the same size data block
ui1* GetDataBlockBuffer(ui8 block_len)
{
    static ui1 *buff = 0;
    
    if (buff == 0) 
        buff = (ui1 *) malloc(block_len * 8);

    return buff;
}

si4 add_block_index_to_channel_list(CHANNEL_STATE *channel_state, ui8 block_hdr_time, ui8 outfile_data_offset, ui8 num_elements_processed)
{
    BLOCK_INDEX_ELEMENT *head, *current;
    ui8 block_sample_index;
    FILE *ofp_mtf;
    ui1 *temp_mtf_index;
    si4 return_value;
    
    // bring in data from channel_state struct
    head               = channel_state->block_index_head;
    current            = channel_state->block_index_current;
    block_sample_index = channel_state->block_sample_index;
    ofp_mtf            = channel_state->out_file_mtf;
    temp_mtf_index     = channel_state->temp_mtf_index;
    
    return_value = 0;
    
    if (head == NULL)
    {
        // empty list, create first element
        head = (BLOCK_INDEX_ELEMENT*) calloc((size_t) 1, sizeof(BLOCK_INDEX_ELEMENT));
        current = head;
    }
    else
    {
        // list is not empty, so add a new element
        current->next = (BLOCK_INDEX_ELEMENT*) calloc((size_t) 1, sizeof(BLOCK_INDEX_ELEMENT));
        current = current->next;
    }
    
    if (current == NULL)
    {
        fprintf(stderr, "Insufficient memory to allocate additional block index storage\n"); 
        //exit(1);
        return (1);
    }
    
    current->block_hdr_time        = block_hdr_time;
    current->outfile_data_offset   = outfile_data_offset;
    current->num_entries_processed = block_sample_index;
    
    // increase block_sample_index so the next block contains the correct index value
    block_sample_index += num_elements_processed;
    
    // write index entry to output mtf file
    memcpy(temp_mtf_index,    &(current->block_hdr_time),        sizeof(ui8));
    memcpy(temp_mtf_index+8,  &(current->outfile_data_offset),   sizeof(ui8));
    memcpy(temp_mtf_index+16, &(current->num_entries_processed), sizeof(ui8));

        if (fwrite(temp_mtf_index, sizeof(ui8), (size_t)3, ofp_mtf) != 3)
        {
            //send_email();
            return_value = 1;
        }
    
#ifdef _LOCAL_COPY
    if (recording_to_local)
    {
        if (fwrite(temp_mtf_index, sizeof(ui8), (size_t)3, channel_state->local_out_file_mtf) != 3)
        {
            //send_email();
            return_value = 1;
        }
    }
#endif
    
    // save state of channel for next time
    channel_state->block_index_head    = head;
    channel_state->block_index_current = current;
    channel_state->block_sample_index  = block_sample_index;
    
    return (return_value);
}

si4 add_discontinuity_index_to_channel_list(CHANNEL_STATE *channel_state, ui8 block_index)
{
    DISCONTINUITY_INDEX_ELEMENT *head, *current;
    
    // bring in data from channel_state struct
    head    = channel_state->discontinuity_index_head;
    current = channel_state->discontinuity_index_current;

    
    if (head == NULL)
    {
        // empty list, create first element
        head = (DISCONTINUITY_INDEX_ELEMENT*) calloc((size_t) 1, sizeof(DISCONTINUITY_INDEX_ELEMENT));
        current = head;
    }
    else
    {
        // list is not empty, so add a new element
        current->next = (DISCONTINUITY_INDEX_ELEMENT*) calloc((size_t) 1, sizeof(DISCONTINUITY_INDEX_ELEMENT));
        current = current->next;
    }
    
    if (current == NULL)
    {
        fprintf(stderr, "Insufficient memory to allocate additional discontinuity index storage\n"); 
        //exit(1);
        return (1);
    }
    
    current->block_index      = block_index;
    
    // save state of channel for next time
    channel_state->discontinuity_index_head    = head;
    channel_state->discontinuity_index_current = current;
    return (0);   
}

si4 initialize_mef_channel_data ( CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, sf8 secs_per_block, si1 *chan_map_name, 
                                 si1* subject_password, si4 bit_shift_flag, si1 *path, si4 chan_num)
{
    ui8 nr;
    si1 temp_str[1024];
    extern int errno;
    ui1 *out_header;
    
    channel_state->chan_num = chan_num;
    // add 10% to buffer size to account for possible sample frequency drift
    channel_state->raw_data_ptr_start = (si4 *) calloc((size_t) (secs_per_block * header_ptr->sampling_frequency * 1.10), sizeof(si4));
    if (channel_state->raw_data_ptr_start == NULL)
    {
        fprintf(stderr, "Insufficient memory to allocate temporary channel buffer\n"); 
        exit(1);
    }
    channel_state->raw_data_ptr_current        = channel_state->raw_data_ptr_start;
    channel_state->block_hdr_time              = 0;
    channel_state->block_boundary              = 0;
    channel_state->last_chan_timestamp         = 0;
    channel_state->first_chan_timestamp        = 0;
    channel_state->max_block_size              = 0;
    channel_state->max_block_len               = 0;
    channel_state->max_data_value_file         = INT_MIN;
    channel_state->min_data_value_file         = INT_MAX;
    channel_state->outfile_data_offset         = MEF_HEADER_LENGTH;
    channel_state->number_of_index_entries     = 0;
    channel_state->number_of_discontinuity_entries = 0;
    channel_state->block_sample_index          = 0;
    channel_state->number_of_samples           = 0;
    channel_state->discontinuity_flag          = 1;  // first block is by definition discontinuous
    channel_state->bit_shift_flag              = bit_shift_flag;
    channel_state->block_index_head            = NULL;
    channel_state->block_index_current         = NULL;
    channel_state->discontinuity_index_head    = NULL;
    channel_state->discontinuity_index_current = NULL;
    
    // Open channel output file, and write header to it
    if (path != NULL)
        sprintf(temp_str, "%s/%s.mef", path, chan_map_name);
    else
        sprintf(temp_str, "%s.mef", chan_map_name); 
    channel_state->out_file = fopen(temp_str, "r");
    if (channel_state->out_file != NULL)
    {
        fclose(channel_state->out_file);
        fprintf(stderr, "The file \"%s\" already exists.  Exiting.\n", temp_str); 
        exit(1);
    }
    channel_state->out_file = fopen(temp_str, "w+");
    if (channel_state->out_file == NULL) 
    { 
        fprintf(stderr, "Error creating channel-data output file \"%s\" %s\n", temp_str, strerror(errno)); 
        exit(1); 
    }

    
#ifdef _LOCAL_COPY
    // Open local output file, and write header to it
        sprintf(temp_str, "%s/%s.mef", local_mef_path, chan_map_name);
    channel_state->local_out_file = fopen(temp_str, "r");
    if (channel_state->local_out_file != NULL)
    {
        fclose(channel_state->local_out_file);
        fprintf(stderr, "The file \"%s\" already exists.  Exiting.\n", temp_str);
        exit(1);
    }
    channel_state->local_out_file = fopen(temp_str, "w+");
    if (channel_state->local_out_file == NULL)
    {
        fprintf(stderr, "Error creating channel-data output file \"%s\" %s\n", temp_str, strerror(errno));
        exit(1);
    }
#endif
    
    // Open channel mtf (timing block index) output file, and write header to it
    if (path != NULL)
        sprintf(channel_state->temp_file_name, "%s/_%s.tmp", path, chan_map_name);
    else
        sprintf(channel_state->temp_file_name, "_%s.tmp", chan_map_name); 
    channel_state->out_file_mtf = fopen(channel_state->temp_file_name, "r");
    if (channel_state->out_file_mtf != NULL)
    {
        fclose(channel_state->out_file_mtf);
        fprintf(stderr, "The file \"%s\" already exists.  Exiting.\n", channel_state->temp_file_name); 
        exit(1);
    }
    channel_state->out_file_mtf = fopen(channel_state->temp_file_name, "w+");
    if (channel_state->out_file_mtf == NULL) 
    { 
        fprintf(stderr, "Error creating temp output file \"%s\" %s\n", channel_state->temp_file_name, strerror(errno)); 
        exit(1); 
    }
    
#ifdef _LOCAL_COPY
    // Open channel mtf (timing block index) output file, and write header to it
    sprintf(channel_state->local_temp_file_name, "%s/_%s.tmp", local_mef_path, chan_map_name);
    channel_state->local_out_file_mtf = fopen(channel_state->local_temp_file_name, "r");
    if (channel_state->local_out_file_mtf != NULL)
    {
        fclose(channel_state->local_out_file_mtf);
        fprintf(stderr, "The file \"%s\" already exists.  Exiting.\n", channel_state->local_temp_file_name);
        exit(1);
    }
    channel_state->local_out_file_mtf = fopen(channel_state->local_temp_file_name, "w+");
    if (channel_state->local_out_file_mtf == NULL)
    {
        fprintf(stderr, "Error creating temp output file \"%s\" %s\n", channel_state->local_temp_file_name, strerror(errno));
        exit(1);
    }
#endif
    
    out_header = calloc((size_t) MEF_HEADER_LENGTH, sizeof(ui1));  // calloc rather than declare to ensure memory boundry alignment
    build_mef_header_block(out_header, header_ptr, subject_password);
    

    nr = fwrite(out_header, sizeof(ui1), (size_t) MEF_HEADER_LENGTH, channel_state->out_file);
    if (nr != MEF_HEADER_LENGTH)
    {
        fprintf(stderr, "Error writing file\n");
        exit(1);
    }
    
#ifdef _LOCAL_COPY
    nr = fwrite(out_header, sizeof(ui1), (size_t) MEF_HEADER_LENGTH, channel_state->local_out_file);
    if (nr != MEF_HEADER_LENGTH)
    {
        fprintf(stderr, "Error writing file\n");
        exit(1);
    }
#endif
    
    free(out_header);
    
    // make these part of the channel state to keep everything thread-safe
    channel_state->out_data = (ui1 *) malloc(32000 * 8);  // This assumes 1 second blocks, sampled at 32000 Hz
    channel_state->temp_mtf_index = (ui1*) malloc(sizeof(ui8) * 3);
    
    // handle line noise scoring
    //channel_state->calculate_line_noise_score = calculate_line_noise_score;
    channel_state->normal_block_size = secs_per_block * header_ptr->sampling_frequency;
    
    return(0);
}

si4 process_filled_block( CHANNEL_STATE *channel_state, si4* raw_data_ptr_start, ui4 num_entries, 
                         ui8 block_len, si4 discontinuity_flag, ui8 block_hdr_time, sf8 sampling_frequency)
{
    ui1 *out_data;
    ui8 max_block_size, max_block_len;
    si4 max_data_value_file, min_data_value_file, *ddp, bit_shift_flag;
    ui8 i, outfile_data_offset, RED_block_size, number_of_index_entries, number_of_discontinuity_entries, number_of_samples;
    RED_BLOCK_HDR_INFO block_hdr;
    ui1 data_key[240];
    FILE *ofp;
    si4 return_value;
    ui1 noise_score;
    char cmd[200];
    
    // bring in data from channel_state struct
    max_block_size                  = channel_state->max_block_size;
    max_block_len                   = channel_state->max_block_len;
    max_data_value_file             = channel_state->max_data_value_file;
    min_data_value_file             = channel_state->min_data_value_file;
    outfile_data_offset             = channel_state->outfile_data_offset;
    number_of_index_entries         = channel_state->number_of_index_entries;
    number_of_discontinuity_entries = channel_state->number_of_discontinuity_entries;
    number_of_samples               = channel_state->number_of_samples;
    ofp                             = channel_state->out_file;
    bit_shift_flag                  = channel_state->bit_shift_flag;
    
    return_value = 0;
    
    // do nothing if there is nothing to be done
    if (num_entries == 0)
        return (0);
    
    memset(data_key, 0, 240);  // for now, assume no data encryption
    
    // use a static out_data buffer, this buffer is shared across channels, so this will only
    // work if the block_len of all channels is the same.  If different channels need different
    // block_lens (ie, different sampling rates), then this block buffer will need to be part
    // of the channel struct.
    //out_data = GetDataBlockBuffer(block_len);
    
    // previous method (GetDataBlockBuffer) is not thread-safe, so use this method.
    // TBD clean this up, so it will works with any block_len, rather than hard-coding
    // 1 second blocks.
    out_data = channel_state->out_data;
    
    
    if (bit_shift_flag) 
    {
        //shift 2 bits to 18 bit resolution
        ddp = raw_data_ptr_start;
        for(i = num_entries; i--;) 
        {
            if (*ddp >= 0) 
                *ddp++ = (si4) (((sf8) *ddp / (sf8) 4.0) + 0.5);
            else
                *ddp++ = (si4) (((sf8) *ddp / (sf8) 4.0) - 0.5);
        }
    }
		//if (num_entries == channel_state->normal_block_size)
	//{
    	//noise_score = line_noise_score(channel_state, raw_data_ptr_start, num_entries, sampling_frequency, 60.0, channel_state->normal_block_size);
    	//if (noise_score < line_noise_threshold)
    	//{
        //	send_quality_email(noise_score, channel_state->chan_num);
		//}
	//}
    
    // RED compress data block
    RED_block_size = RED_compress_block(raw_data_ptr_start, out_data, num_entries, 
                                        block_hdr_time, (ui1)discontinuity_flag, (si1*)data_key, MEF_FALSE, &block_hdr);
    // write block to output file
    //pthread_mutex_lock(&protect_fwrite);

        if (fwrite(out_data, sizeof(si1), RED_block_size, ofp) != RED_block_size)
        {
            //send_email();
            return_value = 1;
            
#ifdef _LOCAL_COPY
            recording_to_remote--;
            if (recording_to_remote == 0)
            {
                sprintf(cmd, "date | mail -s stopping_recording_to_SAN_local_recording_only 5072610337@txt.att.net crepeau.daniel@mayo.edu Kremen.Vaclav@mayo.edu");
                system(cmd);
            }
#endif
        }
    
    //pthread_mutex_unlock(&protect_fwrite);
    
#ifdef _LOCAL_COPY
    if (recording_to_local)
    {
        if (fwrite(out_data, sizeof(si1), RED_block_size, channel_state->local_out_file) != RED_block_size)
        {
            //send_email();
            return_value = 1;
            recording_to_local = 0;
        }
    }
#endif
    
    // write block index info to linked list
    if (add_block_index_to_channel_list(channel_state, block_hdr_time, outfile_data_offset, num_entries) != 0)
        return_value = 1;
    
    // save extra info for .mef header
    if (RED_block_size > max_block_size) max_block_size = RED_block_size;
    if (num_entries > max_block_len) max_block_len = num_entries;
    if (block_hdr.max_value > max_data_value_file) max_data_value_file = block_hdr.max_value;
    if (block_hdr.min_value < min_data_value_file) min_data_value_file = block_hdr.min_value;
    
    // update mef header fields relating to block index
    outfile_data_offset += RED_block_size;
    number_of_index_entries++;
    
    number_of_samples += num_entries;
    
    // update discontinuity index and count
    if (discontinuity_flag)
    {
        add_discontinuity_index_to_channel_list(channel_state, number_of_index_entries);
        number_of_discontinuity_entries++;
    }
    
    // save state of channel for next time
    channel_state->max_block_size                  = max_block_size;
    channel_state->max_block_len                   = max_block_len;
    channel_state->max_data_value_file             = max_data_value_file;
    channel_state->min_data_value_file             = min_data_value_file;
    channel_state->outfile_data_offset             = outfile_data_offset;
    channel_state->number_of_index_entries         = number_of_index_entries;
    channel_state->number_of_discontinuity_entries = number_of_discontinuity_entries;
    channel_state->number_of_samples               = number_of_samples;
    
    return(return_value);
}

si4 write_mef_channel_data( CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, PACKET_TIME *packet_times, si4 *samps, ui8 n_packets_to_process, sf8 secs_per_block)
{
    si4 *raw_data_ptr_start, *raw_data_ptr_current;
    ui8 block_len, block_hdr_time, block_boundary;
    ui8 last_chan_timestamp;
    si4 discontinuity_flag;
    si8 j;
    si4 return_value;
    
    // set local constants
    block_len = (ui8) ceil(secs_per_block * header_ptr->sampling_frequency); //user-defined block size (s), convert to # of samples
    
    // bring in data from channel_state struct
    raw_data_ptr_start   = channel_state->raw_data_ptr_start;
    raw_data_ptr_current = channel_state->raw_data_ptr_current;
    block_hdr_time       = channel_state->block_hdr_time;
    block_boundary       = channel_state->block_boundary;
    last_chan_timestamp  = channel_state->last_chan_timestamp;
    discontinuity_flag   = channel_state->discontinuity_flag;
    
    return_value = 0;

    for (j = 0; j < n_packets_to_process; ++j)
    {
        // set timestamp for the first block processed
        if (block_hdr_time == 0)
        {
            // block_hdr_time is the actual time put into the block header (timestamp of the first 
            // block sample), while block_boundary is used only for calculation of which samples go 
            // into which blocks.  block_boundary is never written to the mef file.
            block_hdr_time = packet_times[j].timestamp;
            block_boundary = packet_times[j].timestamp;
        }
        
        if (((packet_times[j].timestamp - last_chan_timestamp) >= DISCONTINUITY_TIME_THRESHOLD) ||
            ((packet_times[j].timestamp - block_boundary) >= header_ptr->block_interval))
        {
            // Block needs to be compressed and written
            
            // See if data exists in the buffer before processing it.  Data might not exist if
            // this is the first sample we've processed so far.
            if ((raw_data_ptr_current - raw_data_ptr_start) > 0)
            {
               // process block of previously collected data
               if (process_filled_block(channel_state, raw_data_ptr_start, (raw_data_ptr_current - raw_data_ptr_start),
                                    block_len, discontinuity_flag, block_hdr_time, header_ptr->sampling_frequency) != 0)
                   return_value = 1;
            }
            
            // mark next block as being discontinuous if discontinuity is found
            if ((packet_times[j].timestamp - last_chan_timestamp) >= DISCONTINUITY_TIME_THRESHOLD)
            {
                //fprintf(stderr, "Found DISCONTINUITY \nprev= %lu, \nnew=  %lu \n", last_chan_timestamp, packet_times[j].timestamp);
                discontinuity_flag = 1;
                block_boundary = packet_times[j].timestamp;
            }
            else
            {
                discontinuity_flag = 0;
                block_boundary += header_ptr->block_interval;
            }
            
            // set next block's timestamp
            block_hdr_time = packet_times[j].timestamp;
            
            // move back to the beginning of the raw block
            raw_data_ptr_current = raw_data_ptr_start;
        }
        
		*raw_data_ptr_current++ = *samps++;
        
        last_chan_timestamp = packet_times[j].timestamp;
        if (channel_state->first_chan_timestamp == 0)
            channel_state->first_chan_timestamp = packet_times[j].timestamp;
    }
    
    // save state of channel for next time
    channel_state->raw_data_ptr_current = raw_data_ptr_current;
    channel_state->last_chan_timestamp  = last_chan_timestamp;
    channel_state->block_hdr_time       = block_hdr_time;
    channel_state->block_boundary       = block_boundary;
    channel_state->discontinuity_flag   = discontinuity_flag;
    
    return(return_value);
}
        

si4 close_mef_channel_file(CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, 
                           si1* session_password, si1* subject_password, sf8 secs_per_block)
{
    FILE *ofp, *ofp_mtf;
    BLOCK_INDEX_ELEMENT *index_ptr, *temp_ptr;
    DISCONTINUITY_INDEX_ELEMENT *discontinuity_ptr, *temp_discontinuity_ptr;
    ui8 nr, block_len, block_hdr_time;
    ui1 *out_header;
    ui1 *index_buffer;
    si4 *raw_data_ptr_start, *raw_data_ptr_current;
    si4 discontinuity_flag;
    ui4 checksum;
    si4 i;
    ui1 *ui1_p1, *ui1_p2;
    ui8 discontinuity_data_offset;     
    ui1 *file_uid_array;
    
    // set local constants
    block_len = (ui8) ceil(secs_per_block * header_ptr->sampling_frequency); //user-defined block size (s), convert to # of samples
    
    index_buffer = (ui1*) malloc(sizeof(ui8) * 3);
    
    // bring in data from channel_state struct
    ofp                  = channel_state->out_file;
    ofp_mtf              = channel_state->out_file_mtf;
    index_ptr            = channel_state->block_index_head;
    discontinuity_ptr    = channel_state->discontinuity_index_head;
    raw_data_ptr_start   = channel_state->raw_data_ptr_start;
    raw_data_ptr_current = channel_state->raw_data_ptr_current;
    discontinuity_flag   = channel_state->discontinuity_flag;
    block_hdr_time       = channel_state->block_hdr_time;
    	
    // finish and write the last block with leftover buffers
    process_filled_block(channel_state, raw_data_ptr_start, (raw_data_ptr_current - raw_data_ptr_start), 
                         block_len, discontinuity_flag, block_hdr_time, header_ptr->sampling_frequency);
    
    // update remaining unfilled mef header fields
    discontinuity_data_offset = channel_state->outfile_data_offset + (channel_state->number_of_index_entries * 3 * sizeof(ui8));
    header_ptr->maximum_compressed_block_size   = channel_state->max_block_size;
    header_ptr->maximum_block_length            = channel_state->max_block_len;
    header_ptr->maximum_data_value              = channel_state->max_data_value_file;
    header_ptr->minimum_data_value              = channel_state->min_data_value_file;
    header_ptr->index_data_offset               = channel_state->outfile_data_offset;
    header_ptr->number_of_index_entries         = channel_state->number_of_index_entries;
    header_ptr->discontinuity_data_offset       = discontinuity_data_offset;
    header_ptr->number_of_discontinuity_entries = channel_state->number_of_discontinuity_entries;
    header_ptr->number_of_samples               = channel_state->number_of_samples;
    header_ptr->recording_end_time              = channel_state->last_chan_timestamp;
    header_ptr->recording_start_time =            channel_state->first_chan_timestamp;
    
    // build file unique UID
    file_uid_array = (ui1 *)calloc(sizeof(ui1), FILE_UNIQUE_ID_LENGTH);
    if (generate_unique_ID(file_uid_array) == 0)
    {
        fprintf(stderr, "Error generating file UID\n");
        return(1);
    }
    memcpy(header_ptr->file_unique_ID, file_uid_array, FILE_UNIQUE_ID_LENGTH);
    free(file_uid_array);
    
    // append block index
    while (index_ptr != NULL)
    {
        // write first three elements (24 bytes) of block index
        memcpy(index_buffer,    &(index_ptr->block_hdr_time),        sizeof(ui8));
        memcpy(index_buffer+8,  &(index_ptr->outfile_data_offset),   sizeof(ui8));
        memcpy(index_buffer+16, &(index_ptr->num_entries_processed), sizeof(ui8));
        

            nr = fwrite(index_buffer, sizeof(ui8), (size_t) 3, ofp);
            if (nr != 3)
            {
                fprintf(stderr, "Error writing block index element to file.\n");
                return(1);
            }
        
#ifdef _LOCAL_COPY
        if (recording_to_local)
        {
            nr = fwrite(index_buffer, sizeof(ui8), (size_t) 3, channel_state->local_out_file);
            if (nr != 3)
            {
                fprintf(stderr, "Error writing block index element to file.\n");
                return(1);
            }
        }
#endif
        
        index_ptr = index_ptr->next;
    }
    
    // append discontinuity index
    while (discontinuity_ptr != NULL)
    {
        // write element of discontinuity index element
        memcpy(index_buffer, &(discontinuity_ptr->block_index), sizeof(ui8));
        

            nr = fwrite(index_buffer, sizeof(ui8), (size_t) 1, ofp);
            if (nr != 1)
            {
                fprintf(stderr, "Error writing discontinuity index element to file.\n");
                return(1);
            }
        
#ifdef _LOCAL_COPY
        if (recording_to_local)
        {
            nr = fwrite(index_buffer, sizeof(ui8), (size_t) 1, channel_state->local_out_file);
            if (nr != 1)
            {
                fprintf(stderr, "Error writing discontinuity index element to file.\n");
                return(1);
            }
        }
#endif
        
        discontinuity_ptr = discontinuity_ptr->next;
    }
    
    // build memory block of header using header struct
    // calloc rather than declare to ensure memory boundry alignment
    out_header = calloc((size_t) MEF_HEADER_LENGTH, sizeof(ui1));
    build_mef_header_block(out_header, header_ptr, subject_password);
    
    // encrypt parts of header that need encrypting
    if (header_ptr->subject_encryption_used)
	{
		if (validate_password(out_header, subject_password) != 1)
		{
			fprintf(stderr, "\n[%s]: subject password encryption failed\n", __FUNCTION__);
			return(1);
		}
		//mayo_encode_protected(out_header, "lighten up"); //encode subject password hint
	}
    
	if (header_ptr->session_encryption_used)
	{
		if (validate_password(out_header, session_password) != 2)
		{
			fprintf(stderr, "\n[%s]: session password encryption failed\n", __FUNCTION__);
			return(1);
		}
	}
    
    //calculate header CRC checksum and save in header- skip last 4 bytes
	checksum = 0xffffffff;
	for (i = 0; i < MEF_HEADER_LENGTH - HEADER_CRC_LENGTH; i++)
		checksum = update_crc_32(checksum, *(out_header+i));
	
	ui1_p1 = out_header + HEADER_CRC_OFFSET;
	ui1_p2 = (ui1 *) &checksum;
	for (i = 0; i < 4; ++i)
		*ui1_p1++ = *ui1_p2++;
    

        // rewrite header, with completely filled in data
        rewind(ofp);
    
        nr = fwrite(out_header, sizeof(ui1), (size_t) MEF_HEADER_LENGTH, ofp);
        if (nr != MEF_HEADER_LENGTH)
        {
            fprintf(stderr, "Error writing file\n");
            return(1);
        }
    
        fclose(ofp);
    
        // close and delete temp index file
        fclose(ofp_mtf);
        remove(channel_state->temp_file_name);
    
#ifdef _LOCAL_COPY
    if (recording_to_local)
    {
        // rewrite header, with completely filled in data
        rewind(channel_state->local_out_file);
        
        nr = fwrite(out_header, sizeof(ui1), (size_t) MEF_HEADER_LENGTH, channel_state->local_out_file);
        if (nr != MEF_HEADER_LENGTH)
        {
            fprintf(stderr, "Error writing file\n");
            return(1);
        }
        
        fclose(channel_state->local_out_file);
        
        // close and delete temp index file
        fclose(channel_state->local_out_file_mtf);
        remove(channel_state->local_temp_file_name);
    }
#endif
    
    // free memory
    free(channel_state->raw_data_ptr_start);
    index_ptr = channel_state->block_index_head;
    while (index_ptr != NULL)
    {
        temp_ptr = index_ptr;
        index_ptr = index_ptr->next;
        free (temp_ptr);
    }
    discontinuity_ptr = channel_state->discontinuity_index_head;
    while (discontinuity_ptr != NULL)
    {
        temp_discontinuity_ptr = discontinuity_ptr;
        discontinuity_ptr = discontinuity_ptr->next;
        free (temp_discontinuity_ptr);
    }
    free(out_header);
    free(index_buffer);
    free(channel_state->out_data);
    free(channel_state->temp_mtf_index);
    
    return(0);
}

/*ui1	line_noise_score(CHANNEL_STATE *channel_state, si4 *in_samps, si8 n_samps, sf8 samp_freq, sf8 line_freq, si4 normal_block_size)
{
    si4		mib[2];
    ui1		score;
    si8		i, fft_bins, p1, p2, n_bins;
    ui8		len;
    sf8		*fft_buf, s1, s2, s3, f_score;
    sf8      *Dr, *Di;
    FILE		*fp;
    sf8  *temp_data_ptr, *data;
    si4  *temp_samp_ptr;
    si4 upper_limit_copy;
    
    si8 fft_len;
    
    // for email purposes, we don't care about abnormal blocks
    if (n_samps != normal_block_size)
    {
        return (255);
    }
    
    // get fft buffer
    fft_buf = (sf8 *) fftw_malloc(FFT_LEN * sizeof(sf8));
    

    
    
    // set upper limit - so if block is slightly larger than normal, then ignore the spill-over
    upper_limit_copy = n_samps;
    if (upper_limit_copy > normal_block_size)
        upper_limit_copy = normal_block_size;
    
    // copy data, converting sf8 to si4
    data = (sf8 *) malloc(normal_block_size * sizeof(sf8));
    temp_data_ptr = data;
    temp_samp_ptr = in_samps;
    for (i = 0; i < upper_limit_copy; i++)
        *temp_data_ptr++ = (sf8) *temp_samp_ptr++;
    
    // if data block is shorter than normal, fill in missing data for FFT purposes
    if (n_samps < normal_block_size)
    {
        for (i=n_samps;i<normal_block_size;i++)
        {
            if (n_samps > 0)  // this should always be true?
                *temp_data_ptr++ = (sf8) in_samps[n_samps-1];  // stretch out last sample to the end
            else
                *temp_data_ptr++ = 0.0;  // fill in whole block of zeros for quality purposes?
        }
    }
    

    
    // forward fft of data
    if (normal_block_size == 30000)
        fftw_execute_r2r(global_pf32k, data, fft_buf);
    else if (normal_block_size == 5000)
        fftw_execute_r2r(global_pf5k, data, fft_buf);
    else if (normal_block_size == 1000)
        fftw_execute_r2r(global_pf1k, data, fft_buf);
    
    free (data);
    
    fft_len = normal_block_size;
    
    // generate score
    fft_bins = fft_len << 1;
    if (fft_len % 2)
        ++fft_bins;  // There is no Nyquist bin in an odd numbered fft in FFTW.  Add one to pretend it exists.
    
    // (line_freq - 7.5) to (line_freq - 2.5) band
    p1 = (si8) ((((line_freq - 7.5) / samp_freq) * fft_bins) + 0.5);
    p2 = (si8) ((((line_freq - 2.5) / samp_freq) * fft_bins) + 0.5);
    
    Dr = fft_buf + p1;
    Di = fft_buf + fft_len - p1;  // There is only no imaginary compent to the DC (0 frequency) bin.
    
    n_bins = p2 - p1 + 1;
    s1 = 0.0;
    for (i = n_bins; i--;) {
        s1 += sqrt((*Dr * *Dr) + (*Di * *Di));
        ++Dr;
        --Di;
    }
    s1 /= (sf8) n_bins;
    
    // (line_freq - 2.5) to (line_freq + 2.5) band
    p1 = (si8) ((((line_freq - 2.5) / samp_freq) * fft_bins) + 0.5);
    p2 = (si8) ((((line_freq + 2.5) / samp_freq) * fft_bins) + 0.5);
    
    Dr = fft_buf + p1;
    Di = fft_buf + fft_len - p1;  // There is only no imaginary compent to the DC (0 frequency) bin.
    
    n_bins = p2 - p1 + 1;
    s2 = 0.0;
    for (i = n_bins; i--;) {
        s2 += sqrt((*Dr * *Dr) + (*Di * *Di));
        ++Dr;
        --Di;
    }
    s2 /= (sf8) n_bins;
    
    // (line_freq + 2.5) to (line_freq + 7.5) band
    p1 = (si8) ((((line_freq + 2.5) / samp_freq) * fft_bins) + 0.5);
    p2 = (si8) ((((line_freq + 7.5) / samp_freq) * fft_bins) + 0.5);
    
    Dr = fft_buf + p1;
    Di = fft_buf + fft_len - p1;  // There is only no imaginary compent to the DC (0 frequency) bin.
    
    n_bins = p2 - p1 + 1;
    s3 = 0.0;
    for (i = n_bins; i--;) {
        s3 += sqrt((*Dr * *Dr) + (*Di * *Di));
        ++Dr;
        --Di;
    }
    s3 /= (sf8) n_bins;
    
    if (s2 == 0)
        return 255;
    
    // calculate ratio
    f_score = (s1 + s3) / (s2 * 2.0);
    //fprintf(stderr, "s1: %f s2: %f s3: %f\n", s1, s2, s3);
    if (f_score > 1.0)
        f_score = 1.0;
    f_score *= f_score;
    score = (ui1) ((f_score * 254.999) + 0.5);
    //if (channel_state->chan_num == 0 || channel_state->chan_num == 4 || channel_state->chan_num == 5)
    //    fprintf(stderr,"\nscore = %d chan = %d", score, channel_state->chan_num);
    
    // clean up
    free(fft_buf);
    
    
    return(score);
}*/


