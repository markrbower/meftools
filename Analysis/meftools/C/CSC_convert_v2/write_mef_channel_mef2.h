#include <time.h>


#define SECS_PER_BLOCK 1



typedef struct {
    ui8	timestamp;
    si4	*packet_start;
    ui8 packet_size;
} PACKET_TIME;



typedef struct BLOCK_INDEX_ELEMENT BLOCK_INDEX_ELEMENT;

struct BLOCK_INDEX_ELEMENT {
    ui8     block_hdr_time;
    ui8     outfile_data_offset;
    ui8     num_entries_processed;
    BLOCK_INDEX_ELEMENT *next;
};

typedef struct DISCONTINUITY_INDEX_ELEMENT DISCONTINUITY_INDEX_ELEMENT;

struct DISCONTINUITY_INDEX_ELEMENT {
    ui8     block_index;
    DISCONTINUITY_INDEX_ELEMENT *next;
};

typedef struct {
    si4     chan_num;
    si4     *raw_data_ptr_start;
    si4     *raw_data_ptr_current;
    ui8     block_hdr_time;
    ui8     block_boundary;
    ui8     last_chan_timestamp;
    ui8     first_chan_timestamp;
    ui8     max_block_size;
    ui8     max_block_len;
    si4     max_data_value_file;
    si4     min_data_value_file;
    ui8     outfile_data_offset;
    ui8     number_of_index_entries;
    ui8     number_of_discontinuity_entries;
    ui8     number_of_samples;
    ui8     block_sample_index;
    si4     discontinuity_flag;
    si4     bit_shift_flag;
    FILE    *out_file;
    FILE    *local_out_file;
    FILE    *out_file_mtf;
    FILE    *local_out_file_mtf;
    ui1*    out_data;
    ui1*    temp_mtf_index;
    si1     temp_file_name[1024];
    si1     local_temp_file_name[1024];
    BLOCK_INDEX_ELEMENT *block_index_head;
    BLOCK_INDEX_ELEMENT *block_index_current;
    DISCONTINUITY_INDEX_ELEMENT *discontinuity_index_head;
    DISCONTINUITY_INDEX_ELEMENT *discontinuity_index_current;
    si4 normal_block_size;
} CHANNEL_STATE;






void pack_mef_header(MEF_HEADER_INFO *out_header_struct, sf8 secs_per_block, si1 *subject_password, si1 *session_password,
                     si1 *uid, si4 anonymize_flag, si4 dst_flag, si4 bit_shift_flag, sf8 sampling_frequency);

si4 initialize_mef_channel_data ( CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, sf8 secs_per_block,
                                 si1 *chan_map_name, si1 *subject_password, si4 bit_shift_flag, si1 *path, si4 chan_num);
si4 write_mef_channel_data(CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, PACKET_TIME *packet_times, si4 *samps,
                           ui8 n_packets_to_process, sf8 secs_per_block);
si4 close_mef_channel_file(CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, si1* session_password,
                           si1 *subject_password, sf8 secs_per_block);


#define DISCONTINUITY_TIME_THRESHOLD 100000

