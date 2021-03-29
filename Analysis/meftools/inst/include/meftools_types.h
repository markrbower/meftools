#include <RcppCommon.h>

  /******************** header fields *************************/
  /************* header version & constants *******************/
  
  #define HEADER_MAJOR_VERSION				2
  #define HEADER_MINOR_VERSION				1
  #define MEF_HEADER_LENGTH					1024
  #define DATA_START_OFFSET					MEF_HEADER_LENGTH
  #define UNENCRYPTED_REGION_OFFSET			0
  #define UNENCRYPTED_REGION_LENGTH			176
  #define SUBJECT_ENCRYPTION_OFFSET			176
  #define SUBJECT_ENCRYPTION_LENGTH			160
  #define SESSION_ENCRYPTION_OFFSET			352
  #define SESSION_ENCRYPTION_LENGTH			512 //maintain multiple of 16
  #define ENCRYPTION_BLOCK_BITS				128
  #define ENCRYPTION_BLOCK_BYTES				(ENCRYPTION_BLOCK_BITS / 8)
  
  #define SAMPLE_VALUE_NAN             (si4) 0xFF800000
  #define SAMPLE_VALUE_NEG_INFINITY    (si4) 0xFF800001
  #define SAMPLE_VALUE_POS_INFINITY    (si4) 0x007FFFFF
  
  #define DMA_HIGH_FREQUENCY_FILTER           9000.0
  
  
  /* 64-bit typedefs - clearer nomenclature */
  typedef char			        si1;
  typedef unsigned char		  ui1;
  typedef short			        si2;
  typedef unsigned short		ui2;
  typedef int			          si4;
  typedef unsigned int		  ui4;
  typedef float			        sf4;
  typedef long int		      si8;
  typedef long unsigned int	ui8;
  typedef double			      sf8;
  
  // Begin Unencrypted Block
  #define INSTITUTION_OFFSET					0
  #define INSTITUTION_LENGTH					64		// $(63)
  #define UNENCRYPTED_TEXT_FIELD_OFFSET		64
  #define UNENCRYPTED_TEXT_FIELD_LENGTH		64		// $(63)
  #define ENCRYPTION_ALGORITHM_OFFSET			128
  #define ENCRYPTION_ALGORITHM_LENGTH			32		// $(29)
  #define SUBJECT_ENCRYPTION_USED_OFFSET		160
  #define SUBJECT_ENCRYPTION_USED_LENGTH		1		// ui1
  #define SESSION_ENCRYPTION_USED_OFFSET		161
  #define SESSION_ENCRYPTION_USED_LENGTH		1		// ui1
  #define DATA_ENCRYPTION_USED_OFFSET			162
  #define DATA_ENCRYPTION_USED_LENGTH			1		// ui1
  #define BYTE_ORDER_CODE_OFFSET				163
  #define BYTE_ORDER_CODE_LENGTH				1		// ui1
  #define HEADER_MAJOR_VERSION_OFFSET			164
  #define HEADER_MAJOR_VERSION_LENGTH			1		// ui1
  #define HEADER_MINOR_VERSION_OFFSET			165
  #define HEADER_MINOR_VERSION_LENGTH			1		// ui1
  #define HEADER_LENGTH_OFFSET				166
  #define HEADER_LENGTH_LENGTH				2		// ui2
  #define SESSION_UNIQUE_ID_OFFSET			168
  #define SESSION_UNIQUE_ID_LENGTH			8		// ui1
  // End Unencrypted Block
  
  // Begin Subject Encrypted Block
  #define SUBJECT_FIRST_NAME_OFFSET			176
  #define SUBJECT_FIRST_NAME_LENGTH			32		// $(31)
  #define SUBJECT_SECOND_NAME_OFFSET			208
  #define SUBJECT_SECOND_NAME_LENGTH			32		// $(31)
  #define SUBJECT_THIRD_NAME_OFFSET			240
  #define SUBJECT_THIRD_NAME_LENGTH			32		// $(31)
  #define SUBJECT_ID_OFFSET					272
  #define SUBJECT_ID_LENGTH					32		// $(31)
  #define SESSION_PASSWORD_OFFSET				304
  #define SESSION_PASSWORD_LENGTH				ENCRYPTION_BLOCK_BYTES		// $(15)
  #define SUBJECT_VALIDATION_FIELD_OFFSET		320
  #define SUBJECT_VALIDATION_FIELD_LENGTH		16
  // End Subject Encrypted Block
  
  // Begin Protected Block
  #define PROTECTED_REGION_OFFSET				336
  #define PROTECTED_REGION_LENGTH				16
  // End Protected Block
  
  // Begin Session Encrypted Block
  #define SESSION_VALIDATION_FIELD_OFFSET		352
  #define SESSION_VALIDATION_FIELD_LENGTH		16		// ui1
  #define NUMBER_OF_SAMPLES_OFFSET			368
  #define NUMBER_OF_SAMPLES_LENGTH			8		// ui8
  #define CHANNEL_NAME_OFFSET					376
  #define CHANNEL_NAME_LENGTH					32		// $(31)
  #define RECORDING_START_TIME_OFFSET			408
  #define RECORDING_START_TIME_LENGTH			8		// ui8
  #define RECORDING_END_TIME_OFFSET			416
  #define RECORDING_END_TIME_LENGTH			8		// ui8
  #define SAMPLING_FREQUENCY_OFFSET			424
  #define SAMPLING_FREQUENCY_LENGTH			8		// sf8
  #define LOW_FREQUENCY_FILTER_SETTING_OFFSET		432
  #define LOW_FREQUENCY_FILTER_SETTING_LENGTH		8		// sf8
  #define HIGH_FREQUENCY_FILTER_SETTING_OFFSET	440
  #define HIGH_FREQUENCY_FILTER_SETTING_LENGTH	8		// sf8
  #define NOTCH_FILTER_FREQUENCY_OFFSET		448
  #define NOTCH_FILTER_FREQUENCY_LENGTH		8		// sf8
  #define VOLTAGE_CONVERSION_FACTOR_OFFSET	456
  #define VOLTAGE_CONVERSION_FACTOR_LENGTH	8		// sf8
  #define ACQUISITION_SYSTEM_OFFSET			464
  #define ACQUISITION_SYSTEM_LENGTH			32		// $(31)
  #define CHANNEL_COMMENTS_OFFSET				496
  #define CHANNEL_COMMENTS_LENGTH				128		// $(127)
  #define STUDY_COMMENTS_OFFSET				624
  #define STUDY_COMMENTS_LENGTH				128		// $(127)
  #define PHYSICAL_CHANNEL_NUMBER_OFFSET		752
  #define PHYSICAL_CHANNEL_NUMBER_LENGTH		4		// si4
  #define COMPRESSION_ALGORITHM_OFFSET		756
  #define COMPRESSION_ALGORITHM_LENGTH		32		// $(31)
  #define MAXIMUM_COMPRESSED_BLOCK_SIZE_OFFSET	788
  #define MAXIMUM_COMPRESSED_BLOCK_SIZE_LENGTH	4		// ui4
  #define MAXIMUM_BLOCK_LENGTH_OFFSET			792
  #define MAXIMUM_BLOCK_LENGTH_LENGTH			8		// ui8
  #define BLOCK_INTERVAL_OFFSET				800
  #define BLOCK_INTERVAL_LENGTH				8		// sf8
  #define MAXIMUM_DATA_VALUE_OFFSET			808
  #define MAXIMUM_DATA_VALUE_LENGTH			4		// si4
  #define MINIMUM_DATA_VALUE_OFFSET			812
  #define MINIMUM_DATA_VALUE_LENGTH			4		// si4
  #define INDEX_DATA_OFFSET_OFFSET			816
  #define	INDEX_DATA_OFFSET_LENGTH			8		// ui8
  #define NUMBER_OF_INDEX_ENTRIES_OFFSET		824
  #define NUMBER_OF_INDEX_ENTRIES_LENGTH		8		// ui8
  #define BLOCK_HEADER_LENGTH_OFFSET			832
  #define BLOCK_HEADER_LENGTH_LENGTH			2		// ui2
  #define GMT_OFFSET_OFFSET					836
  #define GMT_OFFSET_LENGTH					4		//sf4
  #define DISCONTINUITY_DATA_OFFSET_OFFSET	840
  #define DISCONTINUITY_DATA_OFFSET_LENGTH	8		//ui8
  #define NUMBER_OF_DISCONTINUITY_ENTRIES_OFFSET	848
  #define NUMBER_OF_DISCONTINUITY_ENTRIES_LENGTH	8		//ui8
  // End Session Encrypted Block
  
  // Begin Unencrypted Block
  #define FILE_UNIQUE_ID_OFFSET               948
  #define FILE_UNIQUE_ID_LENGTH               8
  #define ANONYMIZED_SUBJECT_NAME_OFFSET		956
  #define ANONYMIZED_SUBJECT_NAME_LENGTH		64		//$(63)
  #define HEADER_CRC_OFFSET					1020
  #define HEADER_CRC_LENGTH					4		//ui4
  // End Unencrypted Block
  
  /******************** structure & type definitions *****************/
  
  typedef struct {
    ui8	time;
    ui8	file_offset;
    ui8	sample_number;
  } INDEX_DATA;

  // BEGIN --- endian_functions.c
  
  namespace Rcpp {
  // [[Rcpp::export]]
  typedef struct {
    si1	institution[INSTITUTION_LENGTH];
    si1	unencrypted_text_field[UNENCRYPTED_TEXT_FIELD_LENGTH];
    si1	encryption_algorithm[ENCRYPTION_ALGORITHM_LENGTH];
    ui1	subject_encryption_used;
    ui1	session_encryption_used;
    ui1	data_encryption_used;
    ui1	byte_order_code;
    ui1	header_version_major;
    ui1	header_version_minor;
    ui1	session_unique_ID[SESSION_UNIQUE_ID_LENGTH];
    ui2	header_length;
    si1	subject_first_name[SUBJECT_FIRST_NAME_LENGTH];
    si1	subject_second_name[SUBJECT_SECOND_NAME_LENGTH];
    si1	subject_third_name[SUBJECT_THIRD_NAME_LENGTH];
    si1	subject_id[SUBJECT_ID_LENGTH];
    si1	session_password[SESSION_PASSWORD_LENGTH];
    si1	subject_validation_field[SUBJECT_VALIDATION_FIELD_LENGTH];
    si1	session_validation_field[SESSION_VALIDATION_FIELD_LENGTH];
    si1	protected_region[PROTECTED_REGION_LENGTH];
    ui8	number_of_samples;
    si1	channel_name[CHANNEL_NAME_LENGTH];
    ui8	recording_start_time;
    ui8	recording_end_time;
    sf8	sampling_frequency;
    sf8	low_frequency_filter_setting;
    sf8	high_frequency_filter_setting;
    sf8	notch_filter_frequency;
    sf8	voltage_conversion_factor;
    si1	acquisition_system[ACQUISITION_SYSTEM_LENGTH];
    si1	channel_comments[CHANNEL_COMMENTS_LENGTH];
    si1	study_comments[STUDY_COMMENTS_LENGTH];
    si4	physical_channel_number;
    si1	compression_algorithm[COMPRESSION_ALGORITHM_LENGTH];
    ui4	maximum_compressed_block_size;
    ui8 maximum_block_length;
    ui8	block_interval;
    si4 maximum_data_value;
    si4 minimum_data_value;
    ui8	index_data_offset;
    ui8	number_of_index_entries;
    ui2 block_header_length;
    sf4 GMT_offset;
    ui8 discontinuity_data_offset;
    ui8 number_of_discontinuity_entries;
    ui1 file_unique_ID[FILE_UNIQUE_ID_LENGTH];
    si1 anonymized_subject_name[ANONYMIZED_SUBJECT_NAME_LENGTH];
    ui4 header_crc;
    INDEX_DATA *file_index;
    ui8 *discontinuity_data;
  } MEF_HEADER_INFO;
  
  template <> SEXP wrap(const MEF_HEADER_INFO& x);
  
  template<> MEF_HEADER_INFO* as(SEXP x);
  
  } 
  
  typedef struct {
    ui4 CRC_32;
    ui1 CRC_validated;
    si4 compressed_bytes;
    ui8 block_start_time;
    si4 difference_count;
    si4 sample_count;
    si4 max_value; //NOTE: max and min are stored in block header as si3's
    si4 min_value;
    ui1 discontinuity;
  } RED_BLOCK_HDR_INFO;
  
  
  typedef struct {
    ui4	low_bound;
    ui4	range;
    ui1	out_byte;
    ui4	underflow_bytes;
    ui1	*ob_p;
  } RANGE_STATS;

  // #include "size_types.h" // already listed above
  // #include "endian_functions.h"
  // BEGIN --- endian_functions.h ---
#ifndef __ENDIAN_FUNCTIONS
#define __ENDIAN_FUNCTIONS
  
  // get cpu endianness: 0 = big, 1 = little */
  static ui1	cpu_endianness();
  static void init_crc32_tab( void );

  static void	reverse_in_place(void *x, si4 len);
  static int	getSBoxValue(int num);
  static int	getSBoxInvert(int num);
  static void	AES_KeyExpansion(int Nk, int Nr, unsigned char *RoundKey, unsigned char *Key);
  static void	AddRoundKey(int round, unsigned char state[][4], unsigned char *RoundKey);
  static void	SubBytes(unsigned char state[][4]);
  static void	InvSubBytes(unsigned char state[][4]);
  static void	ShiftRows(unsigned char state[][4]);
  static void	InvShiftRows(unsigned char state[][4]);
  static void	MixColumns(unsigned char state[][4]);
  static void	InvMixColumns(unsigned char state[][4]);
  static void Cipher(int Nr, unsigned char *in, unsigned char *out, unsigned char state[][4], unsigned char *RoundKey);
  static void	InvCipher(int Nr, unsigned char *in, unsigned char *out, unsigned char state[][4], unsigned char *RoundKey);
  static void	AES_encrypt(unsigned char *in, unsigned char *out, char *password);
  static void	AES_encryptWithKey(unsigned char *in, unsigned char *out, unsigned char *RoundKey);
  static void	AES_decrypt(unsigned char *in, unsigned char *out, char *password);
  static void	AES_decryptWithKey(unsigned char *in, unsigned char *out, unsigned char *RoundKey);
  static si4 check_header_block_alignment(ui1 *header_block, si4 verbose);
  static void strncpy2(si1 *s1, si1 *s2, si4 n);
  static void init_hdr_struct(Rcpp::MEF_HEADER_INFO *header);
  static si4	write_mef(si4 *samps, Rcpp::MEF_HEADER_INFO *mef_header, ui8 len, si1 *out_file, si1 *subject_password);
  static si4	write_mef_ind(si4 *samps, Rcpp::MEF_HEADER_INFO *mef_header, ui8 len, si1 *out_file, si1 *subject_password, INDEX_DATA *index_block, si4 num_blocks, ui1 *discontinuity_array);
  static si4	build_RED_block_header(ui1 *header_block, RED_BLOCK_HDR_INFO *header_struct);
  static ui8 set_session_unique_ID(char *file_name, ui1 *array);
  static void set_hdr_unique_ID(Rcpp::MEF_HEADER_INFO *header, ui1 *array);
  static ui8 generate_unique_ID(ui1 *array);
  static ui4 calculate_CRC(ui1 *data_block);  
  static si4	validate_password(ui1 *header_block, si1 *password);
  static si4	build_mef_header_block(ui1 *encrypted_hdr_block, Rcpp::MEF_HEADER_INFO *hdr_struct, si1 *password);
  static si4	read_mef_header_block(ui1 *header_block, Rcpp::MEF_HEADER_INFO *header_struct, si1 *password);
  static void set_block_hdr_unique_ID(ui1 *block_header, ui1 *array);
  static si4 validate_mef(char *mef_filename, char *log_filename, char *password);
  static void encode_symbol(ui1 symbol, ui4 symbol_cnts, ui4 cnts_lt_symbol, ui4 tot_cnts, RANGE_STATS *rstats );
  static void done_encoding(RANGE_STATS *rstats);
  static void enc_normalize(RANGE_STATS *rstats);
  static si4	read_RED_block_header(ui1 *header_block, RED_BLOCK_HDR_INFO *header_struct);
  static ui8 RED_compress_block(si4 *in_buffer, ui1 *out_buffer, ui4 num_entries, ui8 uUTC_time, ui1 discontinuity, si1 *key, RED_BLOCK_HDR_INFO *block_hdr);
  static void showHeader(Rcpp::MEF_HEADER_INFO *headerStruct);
  
  
  
  static si2	rev_si2(si2 x);
  static ui2	rev_ui2(ui2 x);
  static si4	rev_si4(si4 x);
  static ui4	rev_ui4(ui4 x);
  static sf4	rev_sf4(sf4 x);
  static si8	rev_si8(si8 x);
  static ui8	rev_ui8(ui8 x);
  static sf8	rev_sf8(sf8 x);
  
#endif
  // END --- endian_functions.h ---
  
  
  
