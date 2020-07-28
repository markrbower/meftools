/*									mef_header_2_1.h
 *
 * Specification for Mayo EEG Format (MEF) version 2.1, 
# Copyright 2012, Mayo Foundation, Rochester MN. All rights reserved
# Written by Ben Brinkmann, Matt Stead, and Dan Crepeau
# usage and modification of this source code is governed by the Apache 2.0 license
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 
 
 Thanks to all who acknowledge the Mayo Systems Electrophysiology Laboratory, Rochester, MN
 in academic publications of their work facilitated by this software.
 
 This file specifies the offsets to and sizes of (in bytes) all header values needed for generation of .mef files, as
 well as a structure for the header. Data types are specified in comments where applicable, shorthand notation as follows 
 (see size_types.h):
 signed char		si1
 unsigned char		ui1
 signed short		si2
 unsigned short	ui2
 signed int		si4
 unsigned int		ui4
 float			sf4
 long signed int	si8
 long unsigned int	ui8
 double		sf8
 long double		sf16
 n-char string		$(n)  -allow 1 space for termination character
 
 Header Encryption:
 The header begins with 176 unencrypted bytes, including two text fields and a series of numeric values defining the file’s 
 format and characteristics. The remainder of the header is encrypted with either a “subject” or “file” password. The passwords 
 are zero-terminated strings with a 15 character limit at this time. The subject password is used to access the file password 
 for decryption. The file password decrypts all but subject identifying header fields. The encryption / decryption algorithm 
 is the 128-bit AES standard (http://www.csrc.nist.gov/publications/fips/fips197/fips-197.pdf).
 
 Header Alignment:
 Fields in the header have required byte alignments relative to its start. 16-byte alignment facilitates encryption/decryption 
 beginning at that offset. Other alignment requirements are determined by the data-types: e.g. 8-byte alignment facilitates 
 reading si8, ui8, and sf8 data types
 
 Time Data:
 Each mef file ends in a block of recording time data. The offset to this data and the number of data entries are given in the file header. 
 This block contains triplets of times, file offsets, and sample indices of EEG data. Triplets are ui8 values containing the elapsed microseconds 
 since January 1, 1970 at 00:00:00 in the GMT (Greenwich, England) time zone. These values are easily converted to UTC time format (seconds since 
 1/1/1970 at 00:00:00 GMT), referred to hereafter as uUTC for "micro-UTC"
 
 */
#define OUTPUT_TO_MEF2

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
#include <dirent.h>
#include <errno.h>
#include <libgen.h>

#include <Rcpp.h>

#ifndef _MEF_H
#define _MEF_H

#ifndef SIZE_TYPES_IN
#define SIZE_TYPES_IN
/* 64-bit typedefs - clearer nomenclature */
typedef char			si1;
typedef unsigned char		ui1;
typedef short			si2;
typedef unsigned short		ui2;
typedef int			si4;
typedef unsigned int		ui4;
typedef long int		si8;
typedef long unsigned int	ui8;
typedef float			sf4;
typedef double			sf8;
typedef long double		sf16;
#endif

#define MEF_FALSE 0
#define MEF_TRUE 1


/************* header version & constants *******************/

#define HEADER_MAJOR_VERSION		2
#define HEADER_MINOR_VERSION		1
#define MEF_HEADER_LENGTH		1024
#define DATA_START_OFFSET		MEF_HEADER_LENGTH
#define UNENCRYPTED_REGION_OFFSET	0
#define UNENCRYPTED_REGION_LENGTH	176
#define SUBJECT_ENCRYPTION_OFFSET	176
#define SUBJECT_ENCRYPTION_LENGTH	160
#define SESSION_ENCRYPTION_OFFSET	352
#define SESSION_ENCRYPTION_LENGTH	512 //maintain multiple of 16
#define ENCRYPTION_BLOCK_BITS		128
#define ENCRYPTION_BLOCK_BYTES		(ENCRYPTION_BLOCK_BITS / 8)

#define SAMPLE_VALUE_NAN		(si4) 0xFF800000
#define SAMPLE_VALUE_NEG_INFINITY	(si4) 0xFF800001
#define SAMPLE_VALUE_POS_INFINITY	(si4) 0x007FFFFF

#define DMA_HIGH_FREQUENCY_FILTER	9000.0


/******************** header fields *************************/

// Begin Unencrypted Block
#define INSTITUTION_OFFSET			0
#define INSTITUTION_LENGTH			64		// $(63)
#define UNENCRYPTED_TEXT_FIELD_OFFSET		64
#define UNENCRYPTED_TEXT_FIELD_LENGTH		64		// $(63)
#define ENCRYPTION_ALGORITHM_OFFSET		128
#define ENCRYPTION_ALGORITHM_LENGTH		32		// $(29)
#define SUBJECT_ENCRYPTION_USED_OFFSET		160
#define SUBJECT_ENCRYPTION_USED_LENGTH		1		// ui1
#define SESSION_ENCRYPTION_USED_OFFSET		161
#define SESSION_ENCRYPTION_USED_LENGTH		1		// ui1
#define DATA_ENCRYPTION_USED_OFFSET		162
#define DATA_ENCRYPTION_USED_LENGTH		1		// ui1
#define BYTE_ORDER_CODE_OFFSET			163
#define BYTE_ORDER_CODE_LENGTH			1		// ui1
#define HEADER_MAJOR_VERSION_OFFSET		164
#define HEADER_MAJOR_VERSION_LENGTH		1		// ui1
#define HEADER_MINOR_VERSION_OFFSET		165
#define HEADER_MINOR_VERSION_LENGTH		1		// ui1
#define HEADER_LENGTH_OFFSET			166
#define HEADER_LENGTH_LENGTH			2		// ui2
#define SESSION_UNIQUE_ID_OFFSET		168
#define SESSION_UNIQUE_ID_LENGTH		8		// ui1
// End Unencrypted Block

// Begin Subject Encrypted Block
#define SUBJECT_FIRST_NAME_OFFSET		176
#define SUBJECT_FIRST_NAME_LENGTH		32		// $(31)
#define SUBJECT_SECOND_NAME_OFFSET		208
#define SUBJECT_SECOND_NAME_LENGTH		32		// $(31)
#define SUBJECT_THIRD_NAME_OFFSET		240
#define SUBJECT_THIRD_NAME_LENGTH		32		// $(31)
#define SUBJECT_ID_OFFSET			272
#define SUBJECT_ID_LENGTH			32		// $(31)
#define SESSION_PASSWORD_OFFSET			304
#define SESSION_PASSWORD_LENGTH			ENCRYPTION_BLOCK_BYTES		// $(15)
#define SUBJECT_VALIDATION_FIELD_OFFSET		320
#define SUBJECT_VALIDATION_FIELD_LENGTH		16
// End Subject Encrypted Block

// Begin Protected Block
#define PROTECTED_REGION_OFFSET			336
#define PROTECTED_REGION_LENGTH			16
// End Protected Block

// Begin Session Encrypted Block
#define SESSION_VALIDATION_FIELD_OFFSET		352
#define SESSION_VALIDATION_FIELD_LENGTH		16		// ui1
#define NUMBER_OF_SAMPLES_OFFSET		368
#define NUMBER_OF_SAMPLES_LENGTH		8		// ui8
#define CHANNEL_NAME_OFFSET			376
#define CHANNEL_NAME_LENGTH			32		// $(31)	
#define RECORDING_START_TIME_OFFSET		408
#define RECORDING_START_TIME_LENGTH		8		// ui8
#define RECORDING_END_TIME_OFFSET		416
#define RECORDING_END_TIME_LENGTH		8		// ui8
#define SAMPLING_FREQUENCY_OFFSET		424
#define SAMPLING_FREQUENCY_LENGTH		8		// sf8
#define LOW_FREQUENCY_FILTER_SETTING_OFFSET	432
#define LOW_FREQUENCY_FILTER_SETTING_LENGTH	8		// sf8
#define HIGH_FREQUENCY_FILTER_SETTING_OFFSET	440
#define HIGH_FREQUENCY_FILTER_SETTING_LENGTH	8		// sf8
#define NOTCH_FILTER_FREQUENCY_OFFSET		448
#define NOTCH_FILTER_FREQUENCY_LENGTH		8		// sf8
#define VOLTAGE_CONVERSION_FACTOR_OFFSET	456
#define VOLTAGE_CONVERSION_FACTOR_LENGTH	8		// sf8
#define ACQUISITION_SYSTEM_OFFSET		464
#define ACQUISITION_SYSTEM_LENGTH		32		// $(31)
#define CHANNEL_COMMENTS_OFFSET			496
#define CHANNEL_COMMENTS_LENGTH			128		// $(127)
#define STUDY_COMMENTS_OFFSET			624
#define STUDY_COMMENTS_LENGTH			128		// $(127)
#define PHYSICAL_CHANNEL_NUMBER_OFFSET		752
#define PHYSICAL_CHANNEL_NUMBER_LENGTH		4		// si4
#define COMPRESSION_ALGORITHM_OFFSET		756
#define COMPRESSION_ALGORITHM_LENGTH		32		// $(31)
#define MAXIMUM_COMPRESSED_BLOCK_SIZE_OFFSET	788
#define MAXIMUM_COMPRESSED_BLOCK_SIZE_LENGTH	4		// ui4
#define MAXIMUM_BLOCK_LENGTH_OFFSET		792
#define MAXIMUM_BLOCK_LENGTH_LENGTH		8		// ui8
#define BLOCK_INTERVAL_OFFSET			800
#define BLOCK_INTERVAL_LENGTH			8		// sf8
#define MAXIMUM_DATA_VALUE_OFFSET		808
#define MAXIMUM_DATA_VALUE_LENGTH		4		// si4
#define MINIMUM_DATA_VALUE_OFFSET		812
#define MINIMUM_DATA_VALUE_LENGTH		4		// si4
#define INDEX_DATA_OFFSET_OFFSET		816
#define	INDEX_DATA_OFFSET_LENGTH		8		// ui8
#define NUMBER_OF_INDEX_ENTRIES_OFFSET		824
#define NUMBER_OF_INDEX_ENTRIES_LENGTH		8		// ui8
#define BLOCK_HEADER_LENGTH_OFFSET		832
#define BLOCK_HEADER_LENGTH_LENGTH		2		// ui2
#define GMT_OFFSET_OFFSET			836
#define GMT_OFFSET_LENGTH			4		//sf4
#define DISCONTINUITY_DATA_OFFSET_OFFSET	840
#define DISCONTINUITY_DATA_OFFSET_LENGTH	8		//ui8
#define NUMBER_OF_DISCONTINUITY_ENTRIES_OFFSET	848
#define NUMBER_OF_DISCONTINUITY_ENTRIES_LENGTH	8		//ui8
// End Session Encrypted Block

// Begin Unencrypted Block
#define FILE_UNIQUE_ID_OFFSET			948
#define FILE_UNIQUE_ID_LENGTH			8
#define ANONYMIZED_SUBJECT_NAME_OFFSET		956
#define ANONYMIZED_SUBJECT_NAME_LENGTH		64		//$(63)
#define HEADER_CRC_OFFSET			1020
#define HEADER_CRC_LENGTH			4		//ui4
// End Unencrypted Block

/******************** structure & type definitions *****************/


typedef struct {
  ui8	time;
  ui8	file_offset;
  ui8	sample_number;
} INDEX_DATA;


typedef struct {
  si1		institution[INSTITUTION_LENGTH];
  si1		unencrypted_text_field[UNENCRYPTED_TEXT_FIELD_LENGTH];
  si1		encryption_algorithm[ENCRYPTION_ALGORITHM_LENGTH];
  ui1		subject_encryption_used;
  ui1		session_encryption_used;
  ui1		data_encryption_used;
  ui1		byte_order_code;
  ui1		header_version_major;
  ui1		header_version_minor;
  ui1		session_unique_ID[SESSION_UNIQUE_ID_LENGTH];
  ui2		header_length;
  si1		subject_first_name[SUBJECT_FIRST_NAME_LENGTH];
  si1		subject_second_name[SUBJECT_SECOND_NAME_LENGTH];
  si1		subject_third_name[SUBJECT_THIRD_NAME_LENGTH];
  si1		subject_id[SUBJECT_ID_LENGTH];
  si1		session_password[SESSION_PASSWORD_LENGTH];
  si1		subject_validation_field[SUBJECT_VALIDATION_FIELD_LENGTH];
  si1		session_validation_field[SESSION_VALIDATION_FIELD_LENGTH];
  si1		protected_region[PROTECTED_REGION_LENGTH];
  ui8		number_of_samples;
  si1		channel_name[CHANNEL_NAME_LENGTH];
  ui8		recording_start_time;
  ui8		recording_end_time;
  sf8		sampling_frequency;
  sf8		low_frequency_filter_setting;
  sf8		high_frequency_filter_setting;
  sf8		notch_filter_frequency;
  sf8		voltage_conversion_factor;
  si1		acquisition_system[ACQUISITION_SYSTEM_LENGTH];
  si1		channel_comments[CHANNEL_COMMENTS_LENGTH];
  si1		study_comments[STUDY_COMMENTS_LENGTH];
  si4		physical_channel_number;
  si1		compression_algorithm[COMPRESSION_ALGORITHM_LENGTH];
  ui4		maximum_compressed_block_size;
  ui8		maximum_block_length; 
  ui8		block_interval;
  si4		maximum_data_value;
  si4		minimum_data_value;
  ui8		index_data_offset;
  ui8		number_of_index_entries;
  ui2		block_header_length;
  sf4		GMT_offset;
  ui8		discontinuity_data_offset;
  ui8		number_of_discontinuity_entries;
  ui1		file_unique_ID[FILE_UNIQUE_ID_LENGTH];
  si1		anonymized_subject_name[ANONYMIZED_SUBJECT_NAME_LENGTH];
  ui4		header_crc;
  INDEX_DATA	*file_index;
  ui8		*discontinuity_data;
} MEF_HEADER_INFO;

//RED Codec
#define TOP_VALUE		(ui4) 0x80000000
#define TOP_VALUE_M_1		(ui4) 0x7FFFFFFF
#define CARRY_CHECK		(ui4) 0x7F800000
#define SHIFT_BITS		23
#define EXTRA_BITS		7
#define BOTTOM_VALUE		(ui4) 0x800000
#define BOTTOM_VALUE_M_1	(ui4) 0x7FFFFF
#define FILLER_BYTE		(ui1) 0x55 

//
/* 4 byte checksum, 4 byte compressed byte count, 8 byte time value, 4 byte difference count,  */
/* 4 byte sample count, 3 byte data maximum, 3 byte data minimum, 1 byte discontinuity flag, 256 byte model counts */
#define BLOCK_HEADER_BYTES			287
#define RED_CHECKSUM_OFFSET			0
#define RED_CHECKSUM_LENGTH			4
#define RED_COMPRESSED_BYTE_COUNT_OFFSET	4
#define RED_UUTC_TIME_OFFSET			8
#define RED_DIFFERENCE_COUNT_OFFSET		16
#define RED_SAMPLE_COUNT_OFFSET			20
#define RED_DATA_MAX_OFFSET			24
#define RED_DATA_MIN_OFFSET			27
#define RED_DISCONTINUITY_OFFSET		30
#define RED_STAT_MODEL_OFFSET			31
#define RED_DATA_OFFSET				BLOCK_HEADER_BYTES

/****************************************************************************************************/
/***  block size defines desired packet spacing - do not exceed 2^23 = 8388608 samples per block  ***/
/****************************************************************************************************/

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

// Globals
//static int crc_tab32_init = 0;//initialize to FALSE

//CRC polynomial and expanded key
#define	Koopman32	0xEB31D82E
#define CRC_KOOPMAN32_KEY {0x0, 0x09695c4ca, 0xfb4839c9, 0x6dddfd03, 0x20f3c3cf, 0xb6660705, 0xdbbbfa06, 0x4d2e3ecc, 0x41e7879e, 0xd7724354, 0xbaafbe57, 0x2c3a7a9d, 0x61144451, 0xf781809b, 0x9a5c7d98, 0xcc9b952, 0x83cf0f3c, 0x155acbf6, 0x788736f5, 0xee12f23f, 0xa33cccf3, 0x35a90839, 0x5874f53a, 0xcee131f0, 0xc22888a2, 0x54bd4c68, 0x3960b16b, 0xaff575a1, 0xe2db4b6d, 0x744e8fa7, 0x199372a4, 0x8f06b66e, 0xd1fdae25, 0x47686aef, 0x2ab597ec, 0xbc205326, 0xf10e6dea, 0x679ba920, 0xa465423, 0x9cd390e9, 0x901a29bb, 0x68fed71, 0x6b521072, 0xfdc7d4b8, 0xb0e9ea74, 0x267c2ebe, 0x4ba1d3bd, 0xdd341777, 0x5232a119, 0xc4a765d3, 0xa97a98d0, 0x3fef5c1a, 0x72c162d6, 0xe454a61c, 0x89895b1f, 0x1f1c9fd5, 0x13d52687, 0x8540e24d, 0xe89d1f4e, 0x7e08db84, 0x3326e548, 0xa5b32182, 0xc86edc81, 0x5efb184b, 0x7598ec17, 0xe30d28dd, 0x8ed0d5de, 0x18451114, 0x556b2fd8, 0xc3feeb12, 0xae231611, 0x38b6d2db, 0x347f6b89, 0xa2eaaf43, 0xcf375240, 0x59a2968a, 0x148ca846, 0x82196c8c, 0xefc4918f, 0x79515545, 0xf657e32b, 0x60c227e1, 0xd1fdae2, 0x9b8a1e28, 0xd6a420e4, 0x4031e42e, 0x2dec192d, 0xbb79dde7, 0xb7b064b5, 0x2125a07f, 0x4cf85d7c, 0xda6d99b6, 0x9743a77a, 0x1d663b0, 0x6c0b9eb3, 0xfa9e5a79, 0xa4654232, 0x32f086f8, 0x5f2d7bfb, 0xc9b8bf31, 0x849681fd, 0x12034537, 0x7fdeb834, 0xe94b7cfe, 0xe582c5ac, 0x73170166, 0x1ecafc65, 0x885f38af, 0xc5710663, 0x53e4c2a9, 0x3e393faa, 0xa8acfb60, 0x27aa4d0e, 0xb13f89c4, 0xdce274c7, 0x4a77b00d, 0x7598ec1, 0x91cc4a0b, 0xfc11b708, 0x6a8473c2, 0x664dca90, 0xf0d80e5a, 0x9d05f359, 0xb903793, 0x46be095f, 0xd02bcd95, 0xbdf63096, 0x2b63f45c, 0xeb31d82e, 0x7da41ce4, 0x1079e1e7, 0x86ec252d, 0xcbc21be1, 0x5d57df2b, 0x308a2228, 0xa61fe6e2, 0xaad65fb0, 0x3c439b7a, 0x519e6679, 0xc70ba2b3, 0x8a259c7f, 0x1cb058b5, 0x716da5b6, 0xe7f8617c, 0x68fed712, 0xfe6b13d8, 0x93b6eedb, 0x5232a11, 0x480d14dd, 0xde98d017, 0xb3452d14, 0x25d0e9de, 0x2919508c, 0xbf8c9446, 0xd2516945, 0x44c4ad8f, 0x9ea9343, 0x9f7f5789, 0xf2a2aa8a, 0x64376e40, 0x3acc760b, 0xac59b2c1, 0xc1844fc2, 0x57118b08, 0x1a3fb5c4, 0x8caa710e, 0xe1778c0d, 0x77e248c7, 0x7b2bf195, 0xedbe355f, 0x8063c85c, 0x16f60c96, 0x5bd8325a, 0xcd4df690, 0xa0900b93, 0x3605cf59, 0xb9037937, 0x2f96bdfd, 0x424b40fe, 0xd4de8434, 0x99f0baf8, 0xf657e32, 0x62b88331, 0xf42d47fb, 0xf8e4fea9, 0x6e713a63, 0x3acc760, 0x953903aa, 0xd8173d66, 0x4e82f9ac, 0x235f04af, 0xb5cac065, 0x9ea93439, 0x83cf0f3, 0x65e10df0, 0xf374c93a, 0xbe5af7f6, 0x28cf333c, 0x4512ce3f, 0xd3870af5, 0xdf4eb3a7, 0x49db776d, 0x24068a6e, 0xb2934ea4, 0xffbd7068, 0x6928b4a2, 0x4f549a1, 0x92608d6b, 0x1d663b05, 0x8bf3ffcf, 0xe62e02cc, 0x70bbc606, 0x3d95f8ca, 0xab003c00, 0xc6ddc103, 0x504805c9, 0x5c81bc9b, 0xca147851, 0xa7c98552, 0x315c4198, 0x7c727f54, 0xeae7bb9e, 0x873a469d, 0x11af8257, 0x4f549a1c, 0xd9c15ed6, 0xb41ca3d5, 0x2289671f, 0x6fa759d3, 0xf9329d19, 0x94ef601a, 0x27aa4d0, 0xeb31d82, 0x9826d948, 0xf5fb244b, 0x636ee081, 0x2e40de4d, 0xb8d51a87, 0xd508e784, 0x439d234e, 0xcc9b9520, 0x5a0e51ea, 0x37d3ace9, 0xa1466823, 0xec6856ef, 0x7afd9225, 0x17206f26, 0x81b5abec, 0x8d7c12be, 0x1be9d674, 0x76342b77, 0xe0a1efbd, 0xad8fd171, 0x3b1a15bb, 0x56c7e8b8, 0xc0522c72}
static ui4 crc_tab32[256] = CRC_KOOPMAN32_KEY;

// Range Encoding
typedef struct {
  ui4	low_bound;
  ui4	range;
  ui1	out_byte;
  ui4	underflow_bytes;
  ui1	*ob_p;
} RANGE_STATS;

// AES
#define ENCRYPTION_KEY_LENGTH	240

// mef_lib function prototypes
si4		build_mef_header_block(ui1 *, MEF_HEADER_INFO *, si1 *);
si4		read_mef_header_block(ui1 *, MEF_HEADER_INFO *, si1 *);
ui4		calculate_header_CRC(ui1*);
si4		validate_password(ui1 *, si1 *);
void		showHeader(MEF_HEADER_INFO *);
ui8		generate_unique_ID(ui1 *);
void		set_hdr_unique_ID(MEF_HEADER_INFO *, ui1 *);
void		set_block_hdr_unique_ID(ui1 *, ui1 *);
ui8		set_session_unique_ID(char *, ui1 *);
si4		check_header_block_alignment(ui1 *, si4);
void		strncpy2(si1 *, si1 *, si4);
void		init_hdr_struct(MEF_HEADER_INFO *);
si4		write_mef(si4 *, MEF_HEADER_INFO *, ui8, si1 *, si1 *);
si4		build_RED_block_header(ui1 *, RED_BLOCK_HDR_INFO *);
si4		read_RED_block_header(ui1 *, RED_BLOCK_HDR_INFO *);
ui4		calculate_compressed_block_CRC(ui1 *);
ui4		update_crc_32(ui4, si1);
void		init_crc32_tab(void);
ui8		RED_decompress_block(ui1 *, si4 *, si1 *, ui1 *, ui1, ui1,  RED_BLOCK_HDR_INFO *);
inline void	dec_normalize(ui4 *, ui4 *, ui1 *, ui1 **);
ui8		RED_compress_block(si4 *, ui1 *, ui4, ui8, ui1, ui1 *, ui1, RED_BLOCK_HDR_INFO *);
void		done_encoding(RANGE_STATS *);
inline void	encode_symbol(ui1, ui4, ui4, ui4, RANGE_STATS *);
inline void	enc_normalize(RANGE_STATS *);
ui1		cpu_endianness();
void		reverse_in_place(void *, si4);
si2		rev_si2(si2);
ui2		rev_ui2(ui2);
ui8		rev_ui8(ui8);
sf8		rev_sf8(sf8);
si4		rev_si4(si4);
ui4		rev_ui4(ui4);
sf4		rev_sf4(sf4);
void		AES_decryptWithKey(ui1 *, ui1 *, ui1 *);
void		AES_decrypt(ui1 *, ui1 *, si1 *);
void		AES_encryptWithKey(ui1 *, ui1 *, ui1 *);
void		AES_encrypt(ui1 *, ui1 *, si1 *);
void		InvCipher(si4, ui1 *, ui1 *, ui1 [][4], ui1 *);
void		Cipher(si4, ui1 *, ui1 *, ui1 [][4], ui1 *);
void		InvMixColumns(ui1 [][4]);
void		MixColumns(ui1 [][4]);
void		InvShiftRows(ui1 [][4]);
void		ShiftRows(ui1 [][4]);
void		SubBytes(ui1 [][4]);
void		AddRoundKey(si4, ui1 [][4], ui1*);
void		AES_KeyExpansion(si4, si4, ui1 *, si1 *);
si4		getSBoxInvert(si4);
si4		getSBoxValue(si4);

#endif




#ifndef __NCS_2_MEF
#define __NCS_2_MEF

// Subroutine declarations
ui8 read_ncs_file(si1 *inFileName, si1 *uid, si1 *session_password, si1 *subject_password, si4 anonymize_flag, ui8 uutc_passed_in);
void uutc_time_from_date(sf8 yr, sf8 mo, sf8 dy, sf8 hr, sf8 mn, sf8 sc, ui8 *uutc_time);
si4 read_nev_file(si1 *inFileName, si1 *mef_path, ui8 time_correction_factor);


#define DISCARD_BITS			4
#define NCS_HEADER_SIZE         16384

#endif





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









/***********************************************************************************/
/*** This version runs on little endian machines and writes out big endian files ***/
/***********************************************************************************/

#define DBUG 1
#define SECS_PER_BLOCK 1

si4	ctrl_c_hit;

//	MEF library
/*
# Copyright 2012, Mayo Foundation, Rochester MN. All rights reserved
# Written by Ben Brinkmann, Matt Stead, Dan Crepeau, and Vince Vasoli
# usage and modification of this source code is governed by the Apache 2.0 license
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>


#define EXPORT __attribute__((visibility("default")))
#define EPSILON 0.0001
#define FLOAT_EQUAL(x,y) ( ((y - EPSILON) < x) && (x <( y + EPSILON)) )


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

#include <time.h>


#define SECS_PER_BLOCK 1

#ifndef __NCS_2_MEF
#define __NCS_2_MEF

// Subroutine declarations
ui8 read_ncs_file(si1 *inFileName, si1 *uid, si1 *session_password, si1 *subject_password, si4 anonymize_flag, ui8 uutc_passed_in);
void uutc_time_from_date(sf8 yr, sf8 mo, sf8 dy, sf8 hr, sf8 mn, sf8 sc, ui8 *uutc_time);
si4 read_nev_file(si1 *inFileName, si1 *mef_path, ui8 time_correction_factor);


#define DISCARD_BITS			4
#define NCS_HEADER_SIZE         16384

#endif


typedef struct BLOCK_INDEX_ELEMENT BLOCK_INDEX_ELEMENT;

typedef struct DISCONTINUITY_INDEX_ELEMENT DISCONTINUITY_INDEX_ELEMENT;






void pack_mef_header(MEF_HEADER_INFO *out_header_struct, sf8 secs_per_block, si1 *subject_password, si1 *session_password,
                     si1 *uid, si4 anonymize_flag, si4 dst_flag, si4 bit_shift_flag, sf8 sampling_frequency);

si4 initialize_mef_channel_data ( CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, sf8 secs_per_block,
                                  si1 *chan_map_name, si1 *subject_password, si4 bit_shift_flag, si1 *path, si4 chan_num);
si4 write_mef_channel_data(CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, PACKET_TIME *packet_times, si4 *samps,
                           ui8 n_packets_to_process, sf8 secs_per_block);
si4 close_mef_channel_file(CHANNEL_STATE *channel_state, MEF_HEADER_INFO *header_ptr, si1* session_password,
                           si1 *subject_password, sf8 secs_per_block);


#define DISCONTINUITY_TIME_THRESHOLD 100000



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
  
  out_header = (ui1*)calloc((size_t) MEF_HEADER_LENGTH, sizeof(ui1));  // calloc rather than declare to ensure memory boundry alignment
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


ui8 RED_compress_block(si4 *in_buffer, ui1 *out_buffer, ui4 num_entries, ui8 uUTC_time, ui1 discontinuity, ui1 *key, ui1 data_encryption, RED_BLOCK_HDR_INFO *block_hdr)
{
  ui4	cum_cnts[256], cnts[256], max_cnt, scaled_tot_cnts, extra_bytes;
  ui4	diff_cnts, comp_block_len, comp_len, checksum;
  ui1	diff_buffer[num_entries * 4], *ui1_p1, *ui1_p2, *ehbp;
  si1	*si1_p1, *si1_p2;
  si4	i, diff, max_data_value, min_data_value;
  sf8	stats_scale;
  RANGE_STATS rng_st;

  
  /*** generate differences ***/
  si1_p1 = (si1 *) diff_buffer;
  si1_p2 = (si1 *) in_buffer;
  *si1_p1++ = *si1_p2++;
  *si1_p1++ = *si1_p2++;
  *si1_p1++ = *si1_p2;	// first entry is full value (3 bytes)
  
  max_data_value = min_data_value = in_buffer[0];
  
  for (i = 1; i < num_entries; i++) {
    diff = in_buffer[i] - in_buffer[i - 1];
    if (in_buffer[i] > max_data_value) max_data_value = in_buffer[i];
    else if (in_buffer[i] < min_data_value) min_data_value = in_buffer[i];
    if (diff > 127 || diff < -127) {				// for little endian input
      si1_p2 = (si1 *) (in_buffer + i);
      *si1_p1++ = -128;
      *si1_p1++ = *si1_p2++;
      *si1_p1++ = *si1_p2++;
      *si1_p1++ = *si1_p2;
    } else
      *si1_p1++ = (si1) diff;
  }
  diff_cnts = (si4) (si1_p1 - (si1 *) diff_buffer);
  
  /*** generate statistics ***/
  memset((void *)cnts, 0, 1024);
  ui1_p1 = diff_buffer;
  for (i = diff_cnts; i--;)
    ++cnts[*ui1_p1++];
  
  max_cnt = 0;
  for (i = 0; i < 256; ++i)
    if (cnts[i] > max_cnt)
      max_cnt = cnts[i];
    if (max_cnt > 255) {
      stats_scale = (sf8) 254.999 / (sf8) max_cnt;
      for (i = 0; i < 256; ++i)
        cnts[i] = (ui4) ceil((sf8) cnts[i] * stats_scale);
    }
    cum_cnts[0] = 0;
    for (i = 0; i < 255; ++i)
      cum_cnts[i + 1] = cnts[i] + cum_cnts[i];
    scaled_tot_cnts = cnts[255] + cum_cnts[255];
    
    
    /*** range encode ***/
    rng_st.low_bound = rng_st.out_byte = rng_st.underflow_bytes = 0;
    rng_st.range = TOP_VALUE;
    rng_st.ob_p = out_buffer + BLOCK_HEADER_BYTES; //NOTE: ob_p is declared GLOBAL
    ui1_p1 = diff_buffer;
    for(i = diff_cnts; i--; ++ui1_p1)
      encode_symbol(*ui1_p1, cnts[*ui1_p1], cum_cnts[*ui1_p1], scaled_tot_cnts, &rng_st);
    done_encoding(&rng_st);
    
    
    //ensure 8-byte alignment for next block
    comp_len = (ui4)(rng_st.ob_p - out_buffer);
    extra_bytes = 8 - comp_len % 8;
    
    if (extra_bytes < 8) {
      for (i=0; i<extra_bytes; i++)
        *(rng_st.ob_p++) = FILLER_BYTE; 
    }
    
    /*** write the packet & packet header ***/
    /* 4 byte checksum, 8 byte time value, 4 byte compressed byte count, 4 byte difference count,  */
    /* 4 byte sample count, 3 byte data maximum, 3 byte data minimum, 256 byte model counts */
    
    ui1_p1 = out_buffer;
    
    //fill checksum with zero as a placeholder
    *(ui4 *)(ui1_p1) = 0; ui1_p1 += 4;
    
    comp_block_len = (ui4)((rng_st.ob_p - out_buffer) - BLOCK_HEADER_BYTES);
    if (block_hdr != NULL) block_hdr->compressed_bytes = comp_block_len;
    *(ui4 *)(ui1_p1) = comp_block_len; ui1_p1 += 4;
    
    if (block_hdr != NULL) block_hdr->block_start_time = uUTC_time;
    *(ui8 *)(ui1_p1) = uUTC_time; ui1_p1 += 8;
    
    if (block_hdr != NULL) block_hdr->difference_count = diff_cnts;
    *(ui4 *)(ui1_p1) = diff_cnts; ui1_p1 += 4;
    
    if (block_hdr != NULL) block_hdr->sample_count = num_entries;
    *(ui4 *)(ui1_p1) = num_entries; ui1_p1 += 4;
    
    if (block_hdr != NULL) block_hdr->max_value = max_data_value;
    ui1_p2 = (ui1 *) &max_data_value; //encode max and min values as si3
    for (i = 0; i < 3; ++i)
      *ui1_p1++ = *ui1_p2++;
    
    if (block_hdr != NULL) block_hdr->min_value = min_data_value;
    ui1_p2 = (ui1 *) &min_data_value; //encode max and min values as si3
    for (i = 0; i < 3; ++i) 
      *ui1_p1++ = *ui1_p2++;
    
    if (block_hdr != NULL) block_hdr->discontinuity = discontinuity; 
    *ui1_p1++ = discontinuity;
    
    ehbp = ui1_p1;
    
    for (i = 0; i < 256; ++i)
      *ui1_p1++ = (ui1) cnts[i];
    
    if (data_encryption==MEF_TRUE) {
      if (key==NULL) {
        fprintf(stderr, "[%s] Error: Null Encryption Key with encrypted block header\n", __FUNCTION__);
        return(-1);
      }
      else
        AES_encryptWithKey(ehbp, ehbp, key); //expanded key
    }
    
    
    //calculate CRC checksum and save in block header- skip first 4 bytes
    checksum = 0xffffffff;
    for (i = 4; i < comp_block_len + BLOCK_HEADER_BYTES; i++)
      checksum = update_crc_32(checksum, *(out_buffer+i));
    
    if (block_hdr != NULL) block_hdr->CRC_32 = checksum;
    ui1_p1 = out_buffer;
    ui1_p2 = (ui1 *) &checksum;
    for (i = 0; i < 4; ++i)
      *ui1_p1++ = *ui1_p2++;
    
    return(comp_block_len + BLOCK_HEADER_BYTES);
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
                                      block_hdr_time, (ui1)discontinuity_flag, (ui1*)data_key, MEF_FALSE, &block_hdr);
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
  out_header = (ui1*)calloc((size_t) MEF_HEADER_LENGTH, sizeof(ui1));
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




EXPORT
  si4	build_mef_header_block(ui1 *encrypted_hdr_block, MEF_HEADER_INFO *hdr_struct, si1 *password)
  {
    MEF_HEADER_INFO	*hs;
    si4		i, encrypted_segments, l, *rn;
    ui1		*ehbp, *ehb;

    //check inputs
    if (hdr_struct == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL structure pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    if (encrypted_hdr_block == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL header block pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    if (password == NULL && hdr_struct->subject_encryption_used)
    {
      fprintf(stderr, "[%s] Error: NULL password pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    if (check_header_block_alignment(encrypted_hdr_block, 1)) //verbose mode- error msg built into function
    {
      return(1);
    }	
    
    ehb = encrypted_hdr_block;
    hs = hdr_struct;
    
    /* check passwords */
    if (hs->subject_encryption_used) 
    {
      l = (si4) strlen(password); 
      if (l >= ENCRYPTION_BLOCK_BYTES || l == 0) {
        (void) printf("\n%s: subject password error\n", __FUNCTION__);
        return(1);
      }
    }
    if (hs->session_encryption_used)
    {
      if (hs->subject_encryption_used) //subject AND session encryption used:
        l = (si4) strlen(hs->session_password);   // session password taken from the mef header structure
      else 
      {
        l = (si4) strlen(password); 
        if (l == 0)
        {
          l = (si4) strlen(hs->session_password);
          if (l) strncpy2(password, hs->session_password, SESSION_PASSWORD_LENGTH);
        }
        else
        {
          if (hs->session_password[0] == 0)
            strncpy2(hs->session_password, password, SESSION_PASSWORD_LENGTH);
        } 
      }
      if (l >= ENCRYPTION_BLOCK_BYTES || l == 0) {
        (void) printf("\n%s: session password error\n", __FUNCTION__);
        return(1);
      }
    }
    
    if (hs->subject_encryption_used || hs->session_encryption_used) {
      /* fill header with random numbers */
      //srandomdev();
      srand(time(NULL));
      rn = (si4 *) ehb;
      for (i = MEF_HEADER_LENGTH / 4; i--;)
        *rn++ = (si4)random();
    }
    
    /* build unencrypted block */
    strncpy2((si1 *) (ehb + INSTITUTION_OFFSET), hs->institution, INSTITUTION_LENGTH);
    strncpy2((si1 *) (ehb + UNENCRYPTED_TEXT_FIELD_OFFSET), hs->unencrypted_text_field, UNENCRYPTED_TEXT_FIELD_LENGTH);	
    sprintf((si1 *) (ehb + ENCRYPTION_ALGORITHM_OFFSET), "%d-bit AES", ENCRYPTION_BLOCK_BITS);	
    *((ui1 *) (ehb + SUBJECT_ENCRYPTION_USED_OFFSET)) = hs->subject_encryption_used;		
    *((ui1 *) (ehb + SESSION_ENCRYPTION_USED_OFFSET)) = hs->session_encryption_used;		
    *((ui1 *) (ehb + DATA_ENCRYPTION_USED_OFFSET)) = hs->data_encryption_used;		
    *(ehb + BYTE_ORDER_CODE_OFFSET) = hs->byte_order_code;				
    //	strncpy2((si1 *) (ehb + FILE_TYPE_OFFSET), hs->file_type, FILE_TYPE_LENGTH);		
    *(ehb + HEADER_MAJOR_VERSION_OFFSET) = hs->header_version_major;	
    *(ehb + HEADER_MINOR_VERSION_OFFSET) = hs->header_version_minor;			
    memcpy(ehb + SESSION_UNIQUE_ID_OFFSET, hs->session_unique_ID, SESSION_UNIQUE_ID_LENGTH);
    *((ui2 *) (ehb + HEADER_LENGTH_OFFSET)) = hs->header_length;
    
    /* build subject encrypted block */
    strncpy2((si1 *) (ehb + SUBJECT_FIRST_NAME_OFFSET), hs->subject_first_name, SUBJECT_FIRST_NAME_LENGTH);	
    strncpy2((si1 *) (ehb + SUBJECT_SECOND_NAME_OFFSET), hs->subject_second_name, SUBJECT_SECOND_NAME_LENGTH);
    strncpy2((si1 *) (ehb + SUBJECT_THIRD_NAME_OFFSET), hs->subject_third_name, SUBJECT_THIRD_NAME_LENGTH);
    strncpy2((si1 *) (ehb + SUBJECT_ID_OFFSET), hs->subject_id, SUBJECT_ID_LENGTH);
    
    if (hs->session_encryption_used && hs->subject_encryption_used)
      strncpy2((si1 *) (ehb + SESSION_PASSWORD_OFFSET), hs->session_password, SESSION_PASSWORD_LENGTH);
    else
      *(si1 *) (ehb + SESSION_PASSWORD_OFFSET) = 0;
    
    /* apply subject encryption to subject block */
    if (hs->subject_encryption_used) {
      //copy subject password into validation field in pascal format string
      l = (ui1) strlen(password);
      *(ehb + SUBJECT_VALIDATION_FIELD_OFFSET) = l;
      memcpy(ehb + SUBJECT_VALIDATION_FIELD_OFFSET + 1, password, l);  //memcpy doesn't add a trailing zero		
      
      encrypted_segments = SUBJECT_ENCRYPTION_LENGTH / ENCRYPTION_BLOCK_BYTES;
      ehbp = ehb + SUBJECT_ENCRYPTION_OFFSET;
      for (i = encrypted_segments; i--;) {
        AES_encrypt(ehbp, ehbp, password);
        ehbp += ENCRYPTION_BLOCK_BYTES;
      }
    }
    
    /* build session encrypted block */
    *((ui8 *) (ehb + NUMBER_OF_SAMPLES_OFFSET)) = hs->number_of_samples;
    strncpy2((si1 *) (ehb + CHANNEL_NAME_OFFSET), hs->channel_name, CHANNEL_NAME_LENGTH);	
    *((ui8 *) (ehb + RECORDING_START_TIME_OFFSET)) = hs->recording_start_time;
    *((ui8 *) (ehb + RECORDING_END_TIME_OFFSET)) = hs->recording_end_time;
    *((sf8 *) (ehb + SAMPLING_FREQUENCY_OFFSET)) = hs->sampling_frequency;	
    *((sf8 *) (ehb + LOW_FREQUENCY_FILTER_SETTING_OFFSET)) = hs->low_frequency_filter_setting;
    *((sf8 *) (ehb + HIGH_FREQUENCY_FILTER_SETTING_OFFSET)) = hs->high_frequency_filter_setting;
    *((sf8 *) (ehb + NOTCH_FILTER_FREQUENCY_OFFSET)) = hs->notch_filter_frequency;
    *((sf8 *) (ehb + VOLTAGE_CONVERSION_FACTOR_OFFSET)) = hs->voltage_conversion_factor;	
    strncpy2((si1 *) (ehb + ACQUISITION_SYSTEM_OFFSET), hs->acquisition_system, ACQUISITION_SYSTEM_LENGTH);
    strncpy2((si1 *) (ehb + CHANNEL_COMMENTS_OFFSET), hs->channel_comments, CHANNEL_COMMENTS_LENGTH);
    strncpy2((si1 *) (ehb + STUDY_COMMENTS_OFFSET), hs->study_comments, STUDY_COMMENTS_LENGTH);
    *((si4 *) (ehb + PHYSICAL_CHANNEL_NUMBER_OFFSET)) = hs->physical_channel_number;
    strncpy2((si1 *) (ehb + COMPRESSION_ALGORITHM_OFFSET), hs->compression_algorithm, COMPRESSION_ALGORITHM_LENGTH);
    *((ui4 *) (ehb + MAXIMUM_COMPRESSED_BLOCK_SIZE_OFFSET)) = hs->maximum_compressed_block_size;
    *((ui8 *) (ehb + MAXIMUM_BLOCK_LENGTH_OFFSET)) = hs->maximum_block_length;
    *((ui8 *) (ehb + BLOCK_INTERVAL_OFFSET)) = hs->block_interval;
    *((si4 *) (ehb + MAXIMUM_DATA_VALUE_OFFSET)) = hs->maximum_data_value;
    *((si4 *) (ehb + MINIMUM_DATA_VALUE_OFFSET)) = hs->minimum_data_value;
    *((ui8 *) (ehb + INDEX_DATA_OFFSET_OFFSET)) = hs->index_data_offset;
    *((ui8 *) (ehb + NUMBER_OF_INDEX_ENTRIES_OFFSET)) = hs->number_of_index_entries;
    *((ui2 *) (ehb + BLOCK_HEADER_LENGTH_OFFSET)) = hs->block_header_length;
    *((sf4 *) (ehb + GMT_OFFSET_OFFSET)) = hs->GMT_offset;
    *((ui8 *) (ehb + DISCONTINUITY_DATA_OFFSET_OFFSET)) = hs->discontinuity_data_offset;
    *((ui8 *) (ehb + NUMBER_OF_DISCONTINUITY_ENTRIES_OFFSET)) = hs->number_of_discontinuity_entries;
    memcpy(ehb + FILE_UNIQUE_ID_OFFSET, hs->file_unique_ID, FILE_UNIQUE_ID_LENGTH);
    strncpy2((si1 *) (ehb + ANONYMIZED_SUBJECT_NAME_OFFSET), hs->anonymized_subject_name, ANONYMIZED_SUBJECT_NAME_LENGTH);
    
    // apply session encryption to session block
    if (hs->session_encryption_used) {
      //copy session password into password validation field in pascal format string
      l = (ui1) strlen(hs->session_password);
      *(ehb + SESSION_VALIDATION_FIELD_OFFSET) = l;
      memcpy(ehb + SESSION_VALIDATION_FIELD_OFFSET + 1, hs->session_password, l);  //memcpy doesn't add a trailing zero		
      
      encrypted_segments = SESSION_ENCRYPTION_LENGTH / ENCRYPTION_BLOCK_BYTES;
      ehbp = ehb + SESSION_ENCRYPTION_OFFSET;
      for (i = encrypted_segments; i--;) {
        AES_encrypt(ehbp, ehbp, hs->session_password);
        ehbp += ENCRYPTION_BLOCK_BYTES;
      }
    }
    
    //calculate header CRC over the encoded and encrypted header
    *((ui4 *) (ehb + HEADER_CRC_OFFSET)) = calculate_header_CRC(ehb);
    
    
    return(0);
  }


EXPORT
  si4	read_mef_header_block(ui1 *header_block, MEF_HEADER_INFO *header_struct, si1 *password)
  {
    MEF_HEADER_INFO	*hs;
    si4		i, privileges, encrypted_segments, session_is_readable, subject_is_readable;
    ui4		crc;
    si1		*encrypted_string;
    ui1		*hb, *dhbp, dhb[MEF_HEADER_LENGTH];
    si1		dummy;

    
    //check inputs
    if (header_struct == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL structure pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    if (header_block == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL header block pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    if (check_header_block_alignment(header_block, 1)) //verbose mode- error msg included in function
      return(1);
    
    hb = header_block;
    hs = header_struct;
    subject_is_readable = 0; session_is_readable = 0;
    encrypted_string = "encrypted";
    
    /* check to see if encryption algorithm matches that assumed by this function */
    (void) sprintf((si1 *) dhb, "%d-bit AES", ENCRYPTION_BLOCK_BITS);
    if (strcmp((si1 *) hb + ENCRYPTION_ALGORITHM_OFFSET, (si1 *) dhb)) {
      (void) fprintf(stderr, "%s: unknown encryption algorithm\n", __FUNCTION__);
      return(1);
    }
    
    memcpy(dhb, header_block, MEF_HEADER_LENGTH);
    memset(header_struct, 0, sizeof(MEF_HEADER_INFO));
    
    //read unencrypted fields
    strncpy2(hs->institution, (si1 *) (dhb + INSTITUTION_OFFSET), INSTITUTION_LENGTH);
    strncpy2(hs->unencrypted_text_field, (si1 *) (dhb + UNENCRYPTED_TEXT_FIELD_OFFSET), UNENCRYPTED_TEXT_FIELD_LENGTH);
    strncpy2(hs->encryption_algorithm, (si1 *) (dhb + ENCRYPTION_ALGORITHM_OFFSET), ENCRYPTION_ALGORITHM_LENGTH);
    hs->byte_order_code = *(dhb + BYTE_ORDER_CODE_OFFSET);
    hs->header_version_major = *(dhb + HEADER_MAJOR_VERSION_OFFSET);
    hs->header_version_minor = *(dhb + HEADER_MINOR_VERSION_OFFSET);
    for(i=0; i<SESSION_UNIQUE_ID_LENGTH; i++)
      hs->session_unique_ID[i] = *(dhb + SESSION_UNIQUE_ID_OFFSET + i*sizeof(ui1));
    
    if (hs->byte_order_code ^ cpu_endianness()) 
      hs->header_length = rev_ui2(*((ui2 *) (dhb + HEADER_LENGTH_OFFSET)));
    else
      hs->header_length = *((ui2 *) (dhb + HEADER_LENGTH_OFFSET));
    
    hs->subject_encryption_used = *(dhb + SUBJECT_ENCRYPTION_USED_OFFSET);
    hs->session_encryption_used = *(dhb + SESSION_ENCRYPTION_USED_OFFSET);
    hs->data_encryption_used = *(dhb + DATA_ENCRYPTION_USED_OFFSET);
    
    
    if(hs->subject_encryption_used==0) subject_is_readable = 1;
    if(hs->session_encryption_used==0) session_is_readable = 1;
    
    if (password == NULL)
    {
      password = &dummy;
      *password = 0;
      privileges = 0;
    }
    else if (hs->subject_encryption_used || hs->session_encryption_used)
    {
      // get password privileges
      privileges = validate_password(hb, password);
      if ( (privileges==0) && (password[0]!=0) ) { 
        (void) fprintf(stderr, "%s: unrecognized password %s\n", __FUNCTION__, password);
      }
    }
    
    if (hs->subject_encryption_used && (privileges == 1)) //subject encryption case
    {
      //decrypt subject encryption block, fill in structure fields
      encrypted_segments = SUBJECT_ENCRYPTION_LENGTH / ENCRYPTION_BLOCK_BYTES;
      dhbp = dhb + SUBJECT_ENCRYPTION_OFFSET;
      for (i = encrypted_segments; i--;) 
      {
        AES_decrypt(dhbp, dhbp, password);
        dhbp += ENCRYPTION_BLOCK_BYTES;
      }
      subject_is_readable = 1;
    }
    
    if(subject_is_readable) {
      strncpy2(hs->subject_first_name, (si1 *) (dhb + SUBJECT_FIRST_NAME_OFFSET), SUBJECT_FIRST_NAME_LENGTH);
      strncpy2(hs->subject_second_name, (si1 *) (dhb + SUBJECT_SECOND_NAME_OFFSET), SUBJECT_SECOND_NAME_LENGTH);
      strncpy2(hs->subject_third_name, (si1 *) (dhb + SUBJECT_THIRD_NAME_OFFSET), SUBJECT_THIRD_NAME_LENGTH);
      strncpy2(hs->subject_id, (si1 *) (dhb + SUBJECT_ID_OFFSET), SUBJECT_ID_LENGTH);
      if (hs->session_encryption_used && hs->subject_encryption_used ) //if both subject and session encryptions used, session password should be in hdr
        strncpy2(hs->session_password, (si1 *) (dhb + SESSION_PASSWORD_OFFSET), SESSION_PASSWORD_LENGTH);
      else if (hs->session_encryption_used)
        strncpy2(hs->session_password, password, SESSION_PASSWORD_LENGTH);
    } 
    else { 
      //subject encryption used but not decoded
      strncpy2(hs->subject_first_name, encrypted_string, SUBJECT_FIRST_NAME_LENGTH);
      strncpy2(hs->subject_second_name, encrypted_string, SUBJECT_SECOND_NAME_LENGTH);
      strncpy2(hs->subject_third_name, encrypted_string, SUBJECT_THIRD_NAME_LENGTH);
      strncpy2(hs->subject_id, encrypted_string, SUBJECT_ID_LENGTH);
      strncpy2(hs->session_password, password, SESSION_PASSWORD_LENGTH); //session password must be passed in if no subject encryption used
    }
    
    if (hs->session_encryption_used && privileges > 0)
    {
      // decrypt session password encrypted fields 
      encrypted_segments = SESSION_ENCRYPTION_LENGTH / ENCRYPTION_BLOCK_BYTES;
      dhbp = dhb + SESSION_ENCRYPTION_OFFSET;
      for (i = encrypted_segments; i--;) 
      {
        AES_decrypt(dhbp, dhbp, hs->session_password);
        dhbp += ENCRYPTION_BLOCK_BYTES;
      }
      session_is_readable = 1;
    }
    
    if (session_is_readable)
    {
      // session password encrypted fields 
      strncpy2(hs->channel_name, (si1 *) (dhb + CHANNEL_NAME_OFFSET), CHANNEL_NAME_LENGTH);
      strncpy2(hs->acquisition_system, (si1 *) (dhb + ACQUISITION_SYSTEM_OFFSET), ACQUISITION_SYSTEM_LENGTH);
      strncpy2(hs->channel_comments, (si1 *) (dhb + CHANNEL_COMMENTS_OFFSET), CHANNEL_COMMENTS_LENGTH);
      strncpy2(hs->study_comments, (si1 *) (dhb + STUDY_COMMENTS_OFFSET), STUDY_COMMENTS_LENGTH);
      strncpy2(hs->compression_algorithm, (si1 *) (dhb + COMPRESSION_ALGORITHM_OFFSET), COMPRESSION_ALGORITHM_LENGTH);
      
      // reverse bytes in some fields for endian mismatch 
      if (hs->byte_order_code ^ cpu_endianness()) {
        hs->number_of_samples = rev_ui8(*((ui8 *) (dhb + NUMBER_OF_SAMPLES_OFFSET)));
        hs->recording_start_time = rev_ui8(*((ui8 *) (dhb + RECORDING_START_TIME_OFFSET)));
        hs->recording_end_time = rev_ui8(*((ui8 *) (dhb + RECORDING_END_TIME_OFFSET)));
        hs->sampling_frequency = rev_sf8(*((sf8 *) (dhb + SAMPLING_FREQUENCY_OFFSET)));
        hs->low_frequency_filter_setting = rev_sf8(*((sf8 *) (dhb + LOW_FREQUENCY_FILTER_SETTING_OFFSET)));
        hs->high_frequency_filter_setting = rev_sf8(*((sf8 *) (dhb + HIGH_FREQUENCY_FILTER_SETTING_OFFSET)));
        hs->notch_filter_frequency = rev_sf8(*((sf8 *) (dhb + NOTCH_FILTER_FREQUENCY_OFFSET)));
        hs->voltage_conversion_factor = rev_sf8(*((sf8 *) (dhb + VOLTAGE_CONVERSION_FACTOR_OFFSET)));
        hs->block_interval = rev_ui8(*((ui8 *) (dhb + BLOCK_INTERVAL_OFFSET)));
        hs->physical_channel_number = rev_si4(*((si4 *) (dhb + PHYSICAL_CHANNEL_NUMBER_OFFSET)));
        hs->maximum_compressed_block_size = rev_ui4(*((ui4 *) (dhb + MAXIMUM_COMPRESSED_BLOCK_SIZE_OFFSET)));
        hs->maximum_block_length = rev_ui8( *((ui8 *) (dhb + MAXIMUM_BLOCK_LENGTH_OFFSET)) );
        hs->maximum_data_value = rev_si4( *((si4 *) (dhb + MAXIMUM_DATA_VALUE_OFFSET)) );
        hs->minimum_data_value = rev_si4( *((si4 *) (dhb + MINIMUM_DATA_VALUE_OFFSET)) );
        hs->index_data_offset = rev_si4(*((ui8 *) (dhb + INDEX_DATA_OFFSET_OFFSET)));
        hs->number_of_index_entries = rev_si4(*((ui8 *) (dhb + NUMBER_OF_INDEX_ENTRIES_OFFSET)));
        hs->block_header_length = rev_ui2(*((ui2 *) (dhb + BLOCK_HEADER_LENGTH_OFFSET)));
        hs->GMT_offset = rev_sf4(*((sf4 *) (dhb + GMT_OFFSET_OFFSET)));
        hs->discontinuity_data_offset= rev_ui8(*((ui8 *) (dhb + DISCONTINUITY_DATA_OFFSET_OFFSET)));
        hs->number_of_discontinuity_entries = rev_ui8(*((ui8 *) (dhb + NUMBER_OF_DISCONTINUITY_ENTRIES_OFFSET)));
      } else {
        hs->number_of_samples = *((ui8 *) (dhb + NUMBER_OF_SAMPLES_OFFSET));
        hs->recording_start_time = *((ui8 *) (dhb + RECORDING_START_TIME_OFFSET));
        hs->recording_end_time = *((ui8 *) (dhb + RECORDING_END_TIME_OFFSET));
        hs->sampling_frequency = *((sf8 *) (dhb + SAMPLING_FREQUENCY_OFFSET));
        hs->low_frequency_filter_setting = *((sf8 *) (dhb + LOW_FREQUENCY_FILTER_SETTING_OFFSET));
        hs->high_frequency_filter_setting = *((sf8 *) (dhb + HIGH_FREQUENCY_FILTER_SETTING_OFFSET));
        hs->notch_filter_frequency = *((sf8 *) (dhb + NOTCH_FILTER_FREQUENCY_OFFSET));
        hs->voltage_conversion_factor = *((sf8 *) (dhb + VOLTAGE_CONVERSION_FACTOR_OFFSET));
        hs->block_interval = *((ui8 *) (dhb + BLOCK_INTERVAL_OFFSET));
        hs->physical_channel_number = *((si4 *) (dhb + PHYSICAL_CHANNEL_NUMBER_OFFSET));
        hs->maximum_compressed_block_size = *((ui4 *) (dhb + MAXIMUM_COMPRESSED_BLOCK_SIZE_OFFSET));
        hs->maximum_block_length = *((ui8 *) (dhb + MAXIMUM_BLOCK_LENGTH_OFFSET));
        hs->maximum_data_value = *((si4 *) (dhb + MAXIMUM_DATA_VALUE_OFFSET));
        hs->minimum_data_value = *((si4 *) (dhb + MINIMUM_DATA_VALUE_OFFSET));
        hs->index_data_offset = *((ui8 *) (dhb + INDEX_DATA_OFFSET_OFFSET));
        hs->number_of_index_entries = *((ui8 *) (dhb + NUMBER_OF_INDEX_ENTRIES_OFFSET));
        hs->block_header_length = *((ui2 *) (dhb + BLOCK_HEADER_LENGTH_OFFSET));
        hs->GMT_offset = *((sf4 *) (dhb + GMT_OFFSET_OFFSET));
        hs->discontinuity_data_offset= *((ui8 *) (dhb + DISCONTINUITY_DATA_OFFSET_OFFSET));
        hs->number_of_discontinuity_entries = *((ui8 *) (dhb + NUMBER_OF_DISCONTINUITY_ENTRIES_OFFSET));
      }
    }
    else {
      //session not readable - fill with encrypted strings
      strncpy2(hs->channel_name, encrypted_string, CHANNEL_NAME_LENGTH);
      strncpy2(hs->acquisition_system, encrypted_string, ACQUISITION_SYSTEM_LENGTH);
      strncpy2(hs->channel_comments, encrypted_string, CHANNEL_COMMENTS_LENGTH);
      strncpy2(hs->study_comments, encrypted_string, STUDY_COMMENTS_LENGTH);
      strncpy2(hs->compression_algorithm, encrypted_string, COMPRESSION_ALGORITHM_LENGTH);
      
      hs->number_of_samples = 0;
      hs->recording_start_time = 0;
      hs->recording_end_time = 0;
      hs->sampling_frequency = -1.0;
      hs->low_frequency_filter_setting = -1.0;
      hs->high_frequency_filter_setting = -1.0;
      hs->notch_filter_frequency = -1.0;
      hs->voltage_conversion_factor = 0.0;
      hs->block_interval = 0;
      hs->physical_channel_number = -1;
      hs->maximum_compressed_block_size = 0;
      hs->maximum_block_length = 0;
      hs->index_data_offset = 0;
      hs->number_of_index_entries = 0;
      hs->block_header_length = 0;
      hs->GMT_offset = 0.0;
      hs->discontinuity_data_offset=0;
      hs->number_of_discontinuity_entries = 0;
    }
    
    //unencrypted tail section of header
    for(i=0; i<FILE_UNIQUE_ID_LENGTH; i++)
      hs->file_unique_ID[i] = *(dhb + FILE_UNIQUE_ID_OFFSET + i*sizeof(ui1));
    
    strncpy2(hs->anonymized_subject_name, (si1 *) (dhb + ANONYMIZED_SUBJECT_NAME_OFFSET), ANONYMIZED_SUBJECT_NAME_LENGTH);
    
    hs->header_crc = *((ui4 *) (dhb + HEADER_CRC_OFFSET));
    
    crc = calculate_header_CRC(header_block);
    if (crc != hs->header_crc) {
      fprintf(stderr, "[%s] Stored header CRC and calculated CRC conflict. Header may be corrupt.\n\n", __FUNCTION__);
    }
    
    return(0);
  }

ui4 calculate_header_CRC(ui1 *header)
{
  int i;
  ui4 checksum;
  
  if (header == NULL) {
    fprintf(stderr, "[%s] Error: NULL data pointer passed in\n", __FUNCTION__);
    return(1);
  }
  
  
  //calculate CRC checksum - skip first 4 bytes
  checksum = 0xffffffff;
  for (i = 0; i < HEADER_CRC_OFFSET; i++) //skip first 4 bytes- don't include the CRC itself in calculation
    checksum = update_crc_32(checksum, *(header + i));
  
  return checksum;
}



//=================================================================================================================
//si4	validate_password(ui1 *header_block, si1 *password)
//
//check password for validity - returns 1 for subject password, 2 for session password, 0 for no match
//
EXPORT
  si4	validate_password(ui1 *header_block, si1 *password)
  {	
    ui1	decrypted_header[MEF_HEADER_LENGTH], *hbp, *dhbp;
    si1 temp_str[SESSION_PASSWORD_LENGTH];
    si4	encrypted_segments, l, i;

    //check for null pointers
    if (header_block == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL header pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    if (password == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL string pointer passed\n", __FUNCTION__);
      return(1);
    }
    
    //check password length
    l = (si4) strlen(password);
    if (l >= ENCRYPTION_BLOCK_BYTES) {
      fprintf(stderr, "%s: Error- password length cannot exceed %d characters\n", __FUNCTION__, ENCRYPTION_BLOCK_BYTES);
      return(0);
    }
    
    // try password as subject pwd
    encrypted_segments = SUBJECT_VALIDATION_FIELD_LENGTH / ENCRYPTION_BLOCK_BYTES;
    hbp = header_block + SUBJECT_VALIDATION_FIELD_OFFSET;
    dhbp = decrypted_header + SUBJECT_VALIDATION_FIELD_OFFSET;
    for (i = encrypted_segments; i--;) {
      AES_decrypt(hbp, dhbp, password);
      hbp += ENCRYPTION_BLOCK_BYTES;
      dhbp += ENCRYPTION_BLOCK_BYTES;
    }
    
    // convert from pascal string
    dhbp = decrypted_header + SUBJECT_VALIDATION_FIELD_OFFSET;
    l = (si4) dhbp[0];
    if (l < ENCRYPTION_BLOCK_BYTES) {
      strncpy(temp_str, (const char *)(dhbp + 1), l);
      temp_str[l] = 0;
      // compare subject passwords
      if (strcmp(temp_str, password) == 0)
        return(1);
    }
    
    
    // try using passed password to decrypt session encrypted key
    encrypted_segments = SESSION_VALIDATION_FIELD_LENGTH / ENCRYPTION_BLOCK_BYTES;
    hbp = header_block + SESSION_VALIDATION_FIELD_OFFSET;
    dhbp = decrypted_header + SESSION_VALIDATION_FIELD_OFFSET;
    for (i = encrypted_segments; i--;) {
      AES_decrypt(hbp, dhbp, password);
      hbp += ENCRYPTION_BLOCK_BYTES;
      dhbp += ENCRYPTION_BLOCK_BYTES;
    }
    
    // convert from pascal string
    dhbp = decrypted_header + SESSION_VALIDATION_FIELD_OFFSET;
    l = (si4) dhbp[0];
    if (l < ENCRYPTION_BLOCK_BYTES) {
      strncpy(temp_str, (const char *)(dhbp + 1), l);
      temp_str[l] = 0;
      // compare session passwords
      if (strcmp(temp_str, password) == 0)
        return(2);
    }
    
    return(0);
  }


//==============================================================================================
//
//	void showHeader(MEF_HEADER_INFO *headerStruct)
//

EXPORT
  void showHeader(MEF_HEADER_INFO *headerStruct)
  {
    si8	long_file_time;
    si1	*time_str, temp_str[256];
    int i;
    
    //check input
    if (headerStruct == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL structure pointer passed\n", __FUNCTION__);
      return;
    }
    
    sprintf(temp_str, "not entered");
    if (headerStruct->institution[0]) (void) fprintf(stdout, "institution = %s\n", headerStruct->institution);
    else (void) fprintf(stdout, "institution = %s\n", temp_str);
    
    if (headerStruct->unencrypted_text_field[0]) (void) fprintf(stdout, "unencrypted_text_field = %s\n", headerStruct->unencrypted_text_field);
    else (void) fprintf(stdout, "unencrypted_text_field = %s\n", temp_str);
    
    (void) fprintf(stdout, "encryption_algorithm = %s\n", headerStruct->encryption_algorithm);
    
    if (headerStruct->byte_order_code) sprintf(temp_str, "little"); else sprintf(temp_str, "big");
    (void) fprintf(stdout, "byte_order_code = %s endian\n", temp_str);
    
    if (headerStruct->subject_encryption_used) sprintf(temp_str, "yes"); else sprintf(temp_str, "no");
    (void) fprintf(stdout, "subject_encryption_used = %s\n", temp_str);
    
    if (headerStruct->session_encryption_used) sprintf(temp_str, "yes"); else sprintf(temp_str, "no");
    (void) fprintf(stdout, "session_encryption_used = %s\n", temp_str);
    
    if (headerStruct->data_encryption_used) sprintf(temp_str, "yes"); else sprintf(temp_str, "no");
    (void) fprintf(stdout, "data_encryption_used = %s\n", temp_str);
    
    //	(void) fprintf(stdout, "file_type = %s\n", headerStruct->file_type);
    (void) fprintf(stdout, "header_version_major = %d\n", headerStruct->header_version_major);
    (void) fprintf(stdout, "header_version_minor = %d\n", headerStruct->header_version_minor);
    
    (void) fprintf(stdout, "session UID = ");
    for(i=0; i<SESSION_UNIQUE_ID_LENGTH; i++)
      (void) fprintf(stdout, "%d ", headerStruct->session_unique_ID[i]);
    (void) fprintf(stdout, "\n");
    
    (void) fprintf(stdout, "header_length = %hd\n", headerStruct->header_length);
    
    sprintf(temp_str, "not entered");
    if (headerStruct->subject_first_name[0]) (void) fprintf(stdout, "subject_first_name = %s\n", headerStruct->subject_first_name);
    else (void) fprintf(stdout, "subject_first_name = %s\n", temp_str);	
    
    if (headerStruct->subject_second_name[0]) (void) fprintf(stdout, "subject_second_name = %s\n", headerStruct->subject_second_name);
    else (void) fprintf(stdout, "subject_second_name = %s\n", temp_str);
    
    if (headerStruct->subject_third_name[0]) (void) fprintf(stdout, "subject_third_name = %s\n", headerStruct->subject_third_name);
    else (void) fprintf(stdout, "subject_third_name = %s\n", temp_str);
    
    if (headerStruct->subject_id[0]) (void) fprintf(stdout, "subject_id = %s\n", headerStruct->subject_id);
    else (void) fprintf(stdout, "subject_id = %s\n", temp_str);
    
    if (headerStruct->session_password[0]) (void) fprintf(stdout, "session_password = %s\n", headerStruct->session_password);
    else (void) fprintf(stdout, "session_password = %s\n", temp_str);
    
    if (headerStruct->number_of_samples) (void) fprintf(stdout, "number_of_samples = %lu\n", headerStruct->number_of_samples);
    else (void) fprintf(stdout, "number_of_samples = %s\n", temp_str);
    
    if (headerStruct->channel_name[0]) (void) fprintf(stdout, "channel_name = %s\n", headerStruct->channel_name);
    else (void) fprintf(stdout, "channel_name = %s\n", temp_str);
    
    long_file_time = (si8) (headerStruct->recording_start_time + 500000) / 1000000;
    time_str = ctime((time_t *) &long_file_time); time_str[24] = 0;
    if (headerStruct->recording_start_time) {
      (void) fprintf(stdout, "recording_start_time = %lu\t(%s)\n", headerStruct->recording_start_time, time_str);
    } else
      (void) fprintf(stdout, "recording_start_time = %s  (default value: %s)\n", temp_str, time_str);
    
    long_file_time = (si8) (headerStruct->recording_end_time + 500000) / 1000000;
    time_str = ctime((time_t *) &long_file_time); time_str[24] = 0;
    if (headerStruct->recording_start_time && headerStruct->recording_end_time) {
      (void) fprintf(stdout, "recording_end_time = %lu\t(%s)\n", headerStruct->recording_end_time, time_str);
    } else
      (void) fprintf(stdout, "recording_end_time = %s  (default value: %s)\n", temp_str, time_str);
    
    if (FLOAT_EQUAL (headerStruct->sampling_frequency, -1.0)) fprintf(stdout, "sampling_frequency = %s\n", temp_str);
    else (void) fprintf(stdout, "sampling_frequency = %lf\n", headerStruct->sampling_frequency);
    
    if (FLOAT_EQUAL (headerStruct->low_frequency_filter_setting, -1.0))  sprintf(temp_str, "not entered");
    else if (headerStruct->low_frequency_filter_setting < EPSILON) sprintf(temp_str, "no low frequency filter");
    else sprintf(temp_str, "%lf", headerStruct->low_frequency_filter_setting);
    (void) fprintf(stdout, "low_frequency_filter_setting = %s\n", temp_str);
    
    if (FLOAT_EQUAL (headerStruct->high_frequency_filter_setting, -1.0)) sprintf(temp_str, "not entered");
    else if (headerStruct->high_frequency_filter_setting < EPSILON) sprintf(temp_str, "no high frequency filter");
    else sprintf(temp_str, "%lf", headerStruct->high_frequency_filter_setting);
    (void) fprintf(stdout, "high_frequency_filter_setting = %s\n", temp_str);
    
    if (FLOAT_EQUAL (headerStruct->notch_filter_frequency, -1.0)) sprintf(temp_str, "not entered");
    else if (headerStruct->notch_filter_frequency < EPSILON) sprintf(temp_str, "no notch filter");
    else sprintf(temp_str, "%lf", headerStruct->notch_filter_frequency);
    (void) fprintf(stdout, "notch_filter_frequency = %s\n", temp_str);
    
    if (FLOAT_EQUAL(headerStruct->voltage_conversion_factor, 0.0)) sprintf(temp_str, "not entered");
    else sprintf(temp_str, "%lf", headerStruct->voltage_conversion_factor);
    (void) fprintf(stdout, "voltage_conversion_factor = %s (microvolts per A/D unit)", temp_str);
    if (headerStruct->voltage_conversion_factor < 0.0)
      (void) fprintf(stdout, " (negative indicates voltages are inverted)\n");
    else
      (void) fprintf(stdout, "\n");
    
    (void) fprintf(stdout, "block_interval = %lu (microseconds)\n", headerStruct->block_interval);
    
    (void) fprintf(stdout, "acquisition_system = %s\n", headerStruct->acquisition_system);
    
    if(headerStruct->physical_channel_number == -1)  (void) fprintf(stdout, "physical_channel_number = %s\n", temp_str);
    else (void) fprintf(stdout, "physical_channel_number = %d\n", headerStruct->physical_channel_number);
    
    
    sprintf(temp_str, "not entered");
    if (headerStruct->channel_comments[0]) (void) fprintf(stdout, "channel_comments = %s\n", headerStruct->channel_comments);
    else (void) fprintf(stdout, "channel_comments = %s\n", temp_str);
    
    if (headerStruct->study_comments[0]) (void) fprintf(stdout, "study_comments = %s\n", headerStruct->study_comments);
    else (void) fprintf(stdout, "study_comments = %s\n", temp_str);
    
    (void) fprintf(stdout, "compression_algorithm = %s\n", headerStruct->compression_algorithm);
    
    if(headerStruct->maximum_compressed_block_size) (void) fprintf(stdout, "maximum_compressed_block_size = %d\n", headerStruct->maximum_compressed_block_size);
    else fprintf(stdout, "maximum_compressed_block_size = %s\n", temp_str);
    
    if(headerStruct->maximum_block_length) (void) fprintf(stdout, "maximum_block_length = %lu\n", headerStruct->maximum_block_length);	
    else (void) fprintf(stdout, "maximum_block_length = %s\n", temp_str);
    
    if(headerStruct->maximum_data_value != headerStruct->minimum_data_value) {
      (void) fprintf(stdout, "maximum_data_value = %d\n", headerStruct->maximum_data_value);
      (void) fprintf(stdout, "minimum_data_value = %d\n", headerStruct->minimum_data_value);	
    }
    else {
      (void) fprintf(stdout, "maximum_data_value = %s\n", temp_str);
      (void) fprintf(stdout, "minimum_data_value = %s\n", temp_str);
    }
    
    if(headerStruct->index_data_offset) (void) fprintf(stdout, "index_data_offset = %lu\n", headerStruct->index_data_offset);
    else (void) fprintf(stdout, "index_data_offset = %s\n", temp_str);
    
    if(headerStruct->number_of_index_entries) (void) fprintf(stdout, "number_of_index_entries = %lu\n", headerStruct->number_of_index_entries);
    else (void) fprintf(stdout, "number_of_index_entries = %s\n", temp_str);
    
    if(headerStruct->block_header_length) (void) fprintf(stdout, "block_header_length = %d\n", headerStruct->block_header_length);
    else (void) fprintf(stdout, "block_header_length = %s\n", temp_str);
    
    if (headerStruct->header_version_minor > 0)
    {
      (void) fprintf(stdout, "GMT_offset = %f\n", headerStruct->GMT_offset);
      
      if(headerStruct->discontinuity_data_offset) (void) fprintf(stdout, "discontinuity_data_offset = %lu\n", headerStruct->discontinuity_data_offset);
      else (void) fprintf(stdout, "discontinuity_data_offset = %s\n", temp_str);
      
      (void) fprintf(stdout, "number_of_discontinuity_entries = %lu\n", headerStruct->number_of_discontinuity_entries);
      
      (void) fprintf(stdout, "file UID = ");
      for(i=0; i<FILE_UNIQUE_ID_LENGTH; i++)
        (void) fprintf(stdout, "%u ", headerStruct->file_unique_ID[i]);
      (void) fprintf(stdout, "\n");
      
      if (headerStruct->anonymized_subject_name[0]) (void) fprintf(stdout, "anonymized_subject_name = %s\n", headerStruct->anonymized_subject_name);
      else (void) fprintf(stdout, "anonymized_subject_name= %s\n", temp_str);
      
      if (headerStruct->header_crc) (void) fprintf(stdout, "header_crc = %u\n", headerStruct->header_crc);
      else (void) fprintf(stdout, "header_crc = %s\n", temp_str);
    }
    
    return;
  }


EXPORT
  ui8 generate_unique_ID(ui1 *array)
  {
    static ui1 first_time = 1; 
    ui8 long_output = 0;
    si4 i;
    
    if (array == NULL) 
    {
      array = (ui1*)calloc(SESSION_UNIQUE_ID_LENGTH, sizeof(ui1));
    }
    
    if (first_time)
    {
      srandom(time(NULL));
      first_time = 0;
    }
    
    for (i=0; i<SESSION_UNIQUE_ID_LENGTH; i++) 
    {
      array[i] = (ui1)(random() % 255);
      long_output += array[i] >> i; 
    }
    
    return (long_output);
  }


EXPORT
  void set_hdr_unique_ID(MEF_HEADER_INFO *header, ui1 *array)
  {
    //check input
    if (header == NULL)
    {
      fprintf(stderr, "[%s] Error: NULL structure pointer passed\n", __FUNCTION__);
      return;
    }
    
    if (array == NULL) //generate new uid
    {
      array = (ui1*)calloc(SESSION_UNIQUE_ID_LENGTH, sizeof(ui1));
      (void)generate_unique_ID(array);
    }
    
    memcpy(header->session_unique_ID, array, SESSION_UNIQUE_ID_LENGTH);
    return;
  }


EXPORT
  void set_block_hdr_unique_ID(ui1 *block_header, ui1 *array)
  {
    
    if (array == NULL) //generate new uid
    {
      array = (ui1*)calloc(SESSION_UNIQUE_ID_LENGTH, sizeof(ui1));
      (void)generate_unique_ID(array);
    }
    
    memcpy((block_header + SESSION_UNIQUE_ID_OFFSET), array, SESSION_UNIQUE_ID_LENGTH);
    return;
  }


EXPORT
  ui8 set_session_unique_ID(char *file_name, ui1 *array)
  {
    FILE *mef_fp;
    si4 read_mef_header_block(), validate_password();
    
    
    //Open file
    mef_fp = fopen(file_name, "r+");
    if (mef_fp == NULL) {
      fprintf(stderr, "%s: Could not open file %s\n", __FUNCTION__, file_name);
      return(1);
    }
    
    
    if (array == NULL) {	
      array = (ui1*)calloc(SESSION_UNIQUE_ID_LENGTH, sizeof(ui1));
      (void)generate_unique_ID(array);
    }
    
    //write file unique ID to header
    fseek(mef_fp, SESSION_UNIQUE_ID_OFFSET, SEEK_SET);
    fwrite(array, sizeof(ui1), SESSION_UNIQUE_ID_LENGTH, mef_fp);
    
    fseek(mef_fp, 0, SEEK_END);
    
    fclose(mef_fp);
    
    return(0);
  }


EXPORT
  si4 check_header_block_alignment(ui1 *header_block, si4 verbose)
  {
    if ((ui8) header_block % 8) {
      if (verbose)
        (void) fprintf(stderr, "Header block is not 8 byte boundary aligned [use malloc() rather than heap declaration] ==> exiting\n");
      return(1);
    }
    
    return(0);
  }


EXPORT
  void strncpy2(si1 *s1, si1 *s2, si4 n)
  {
    si4      len;
    
    for (len = 1; len < n; ++len) {
      if ((*s1++ = *s2++))
        continue;
      return;
    }
    s1[n-1] = 0;
    
    return;
  }


void init_hdr_struct(MEF_HEADER_INFO *header)
{	
  
  memset(header, 0, sizeof(MEF_HEADER_INFO));
  
  header->header_version_major=HEADER_MAJOR_VERSION;
  header->header_version_minor=HEADER_MINOR_VERSION;
  header->header_length=MEF_HEADER_LENGTH;
  header->block_header_length=BLOCK_HEADER_BYTES;
  
  sprintf(header->compression_algorithm, "Range Encoded Differences (RED)");
  sprintf(header->encryption_algorithm,  "AES %d-bit", ENCRYPTION_BLOCK_BITS);
  
  if (cpu_endianness())
    header->byte_order_code = 1;
  else
    header->byte_order_code = 0;
  
  return; 
}

EXPORT
  si4	write_mef(si4 *samps, MEF_HEADER_INFO *mef_header, ui8 len, si1 *out_file, si1 *subject_password)
  {
    ui1	*header, byte_padding[8], discontinuity_flag;
    ui1	*compressed_buffer, *cbp, encryption_key[240];
    si4	sl, max_value, min_value, byte_offset, *sp;
    ui4	samps_per_block;
    si8	i;
    ui8	curr_time, nr, samps_left, index_data_offset, dataCounter;
    ui8	entryCounter, num_blocks, max_block_size, RED_block_size;
    FILE	*fp;
    RED_BLOCK_HDR_INFO RED_bk_hdr;
    INDEX_DATA *index_block, *ip;
    
    if ( mef_header==NULL ) {
      fprintf(stderr, "[%s] NULL header pointer passed into function\n", __FUNCTION__);
      return(1);
    }
    
    header = (ui1*)calloc(sizeof(ui1), (size_t)MEF_HEADER_LENGTH);
    curr_time = mef_header->recording_start_time;
    
    //Check input header values for validity
    if ( mef_header->sampling_frequency < 0.001) {
      fprintf(stderr, "[%s] Improper sampling frequency (%lf Hz) in header %s\n", __FUNCTION__,  mef_header->sampling_frequency, 
              mef_header->channel_name);
      return(1);
    }
    
    if ( mef_header->block_interval < 0.001) {
      fprintf(stderr, "[%s] Improper block interval (%lu microseconds) in header %s\n", __FUNCTION__,  mef_header->block_interval, 
              mef_header->channel_name);
      return(1);
    }	
    samps_per_block = (ui4)((sf8)mef_header->block_interval * mef_header->sampling_frequency/ 1000000.0); 
    
    if (samps_per_block < 1) {
      fprintf(stderr, "[%s] Improper header info- must encode 1 or more samples in each block\n", __FUNCTION__);
      return(1);
    }
    if (samps_per_block > mef_header->number_of_samples) {
      fprintf(stderr, "[%s] Improper header info- samples per block %u greater than total entries %lu for %s\n", __FUNCTION__, samps_per_block,
              mef_header->number_of_samples, mef_header->channel_name);
      return(1);
    }
    num_blocks = ceil( (sf8)len / (sf8)samps_per_block  );
    
    if (num_blocks < 1) {
      fprintf(stderr, "[%s] Improper header info- must encode 1 or more blocks\n", __FUNCTION__);
      return(1);
    }
    
    mef_header->number_of_samples = (ui8) len;  //number of samples may be different from original file
    mef_header->maximum_block_length = samps_per_block;
    
    encryption_key[0] = 0;
    if (mef_header->data_encryption_used)
      AES_KeyExpansion(4, 10, encryption_key, mef_header->session_password); 
    
    
    index_block = (INDEX_DATA *)calloc(num_blocks, sizeof(INDEX_DATA));
    compressed_buffer = (ui1*)calloc(num_blocks*samps_per_block, sizeof(si4)); //we'll assume at least 50% compression
    
    if (index_block == NULL || compressed_buffer == NULL) {
      fprintf(stderr, "[%s] malloc error\n", __FUNCTION__);
      return(1);
    }
    
    sl = (si4)strlen(out_file);
    if ((strcmp((out_file + sl - 4), ".mef"))) {
      fprintf(stderr, "no \".mef\" on input name => exiting\n");
      return(1);
    }
    fp = fopen(out_file, "w");
    if (fp == NULL) {fprintf(stderr, "Error [%s]: Can't open file %s for writing\n\n", __FUNCTION__, out_file); exit(1);}
    
    
    memset(header, 0, MEF_HEADER_LENGTH); //fill mef header space with zeros - will write real info after writing blocks and indices
    fwrite(header, 1, MEF_HEADER_LENGTH, fp);
    
    sp = samps;	
    cbp = compressed_buffer; 
    ip = index_block;
    dataCounter = MEF_HEADER_LENGTH; 
    entryCounter=0; 
    discontinuity_flag = 1;
    max_value = INT_MAX; min_value = INT_MIN; 
    max_block_size = 0;
    
    samps_left = len;
    for (i=0; i<num_blocks; i++) {
      ip->time = mef_header->recording_start_time + i * mef_header->block_interval;
      ip->file_offset = dataCounter; 
      ip->sample_number = i * samps_per_block;
      
      if (samps_left < samps_per_block) samps_per_block = (ui4)samps_left;		
      
      RED_block_size = RED_compress_block(sp, cbp, samps_per_block, ip->time, (ui1)discontinuity_flag, encryption_key, mef_header->data_encryption_used, &RED_bk_hdr);
      
      dataCounter += RED_block_size;
      cbp += RED_block_size;
      entryCounter += RED_bk_hdr.sample_count;
      samps_left -= RED_bk_hdr.sample_count;
      sp += RED_bk_hdr.sample_count;
      ip++;
      
      if (RED_bk_hdr.max_value > max_value) max_value = RED_bk_hdr.max_value;
      if (RED_bk_hdr.min_value < min_value) min_value = RED_bk_hdr.min_value;
      if (RED_block_size > max_block_size) max_block_size = RED_block_size;
      
      discontinuity_flag = 0; //only the first block has a discontinuity		
    }
    
    //update mef header with new values
    mef_header->maximum_data_value = max_value;
    mef_header->minimum_data_value = min_value;
    mef_header->maximum_compressed_block_size = (ui4)max_block_size;
    mef_header->number_of_index_entries = num_blocks;
    
    // write mef entries
    nr = fwrite(compressed_buffer, sizeof(si1), (size_t) dataCounter - MEF_HEADER_LENGTH, fp); 
    if (nr != dataCounter - MEF_HEADER_LENGTH) { fprintf(stderr, "Error writing file\n"); fclose(fp); return(1); }
    
    //byte align index data if needed
    index_data_offset = ftell(fp);
    byte_offset = (si4)(index_data_offset % 8);
    if (byte_offset) {
      memset(byte_padding, 0, 8);
      fwrite(byte_padding, sizeof(ui1), 8 - byte_offset, fp);
      index_data_offset += 8 - byte_offset;
    }
    mef_header->index_data_offset = index_data_offset;
    
    //write index offset block to end of file
    nr = fwrite(index_block, sizeof(INDEX_DATA), (size_t) num_blocks, fp); 
    
    //build mef header from structure
    nr = build_mef_header_block(header, mef_header, subject_password); //recycle nr
    if (nr) { fprintf(stderr, "Error building mef header\n"); return(1); }
    
    fseek(fp, 0, SEEK_SET); //reset fp to beginning of file to write mef header
    nr = fwrite(header, sizeof(ui1), (size_t) MEF_HEADER_LENGTH, fp);
    if (nr != MEF_HEADER_LENGTH) { fprintf(stderr, "Error writing mef header\n"); return(1); }
    
    fclose(fp);
    
    free(compressed_buffer);
    free(index_block);
    
    return(0);
  }

EXPORT
  si4	write_mef_ind(si4 *samps, MEF_HEADER_INFO *mef_header, ui8 len, si1 *out_file, si1 *subject_password, INDEX_DATA *index_block, si4 num_blocks, ui1 *discontinuity_array)
  {
    ui1 *header, byte_padding[8], *compressed_buffer, *cbp, encryption_key[240];
    si1	free_index=0, free_discont=0;
    si4	sl, max_value, min_value, byte_offset, *sp, samps_to_encode;
    ui4 samps_per_block;
    si8	i;
    ui8 curr_time, nr, samps_left, index_data_offset, dataCounter;
    ui8 entryCounter, max_block_size, RED_block_size;
    sf8 dt, ds;
    FILE	*fp;
    RED_BLOCK_HDR_INFO RED_bk_hdr;
    INDEX_DATA *ip;
    
    //check inputs
    if ( mef_header==NULL ) {
      fprintf(stderr, "[%s] NULL header pointer passed into function\n", __FUNCTION__);
      return(1);
    }
    //Check input header values for validity
    if ( mef_header->sampling_frequency < 0.001 || mef_header->sampling_frequency > 1000000) {
      fprintf(stderr, "[%s] Improper sampling frequency (%lf Hz) in header %s\n", __FUNCTION__,  mef_header->sampling_frequency, 
              mef_header->channel_name);
      return(1);
    }	
    if ( mef_header->block_interval < (ui8)(2.0 * 1000000.0 / mef_header->sampling_frequency)) { //must encode at least 2 samples per block
      fprintf(stderr, "[%s] Improper block interval (%lu microseconds) in header %s for stated sampling frequency %lf\n", __FUNCTION__,  mef_header->block_interval, mef_header->channel_name, mef_header->sampling_frequency);
      return(1);
    }
    
    header = (ui1*)calloc(sizeof(ui1), (size_t)MEF_HEADER_LENGTH);
    curr_time = mef_header->recording_start_time;
    samps_per_block = (ui4)((sf8)mef_header->block_interval * mef_header->sampling_frequency/ 1000000.0); 
    
    if (samps_per_block < 2) {
      fprintf(stderr, "[%s] Improper header info- must encode 2 or more samples in each block\n", __FUNCTION__);
      return(1);
    }
    if (samps_per_block > mef_header->number_of_samples) {
      fprintf(stderr, "[%s] Improper header info- samples per block %u greater than total entries %lu for %s\n", __FUNCTION__, samps_per_block,
              mef_header->number_of_samples, mef_header->channel_name);
      return(1);
    }
    
    mef_header->number_of_samples = (ui8) len;  
    mef_header->maximum_block_length = samps_per_block;
    
    if (mef_header->high_frequency_filter_setting > mef_header->sampling_frequency)
      mef_header->high_frequency_filter_setting = mef_header->sampling_frequency;
    
    encryption_key[0] = 0;
    if (mef_header->data_encryption_used)
      AES_KeyExpansion(4, 10, encryption_key, mef_header->session_password); 
    
    sl = (si4)strlen(out_file);
    if ((strcmp((out_file + sl - 4), ".mef"))) {
      fprintf(stderr, "no \".mef\" on input name => exiting\n");
      return(1);
    }
    fp = fopen(out_file, "w");
    if (fp == NULL) {fprintf(stderr, "Error [%s]: Can't open file %s for writing\n\n", __FUNCTION__, out_file); exit(1);}
    
    memset(header, 0, MEF_HEADER_LENGTH); //fill mef header space with zeros - will write real info after writing blocks and indices
    fwrite(header, 1, MEF_HEADER_LENGTH, fp);
    
    //calculate index entries if not passed in
    if (index_block == NULL) {
      num_blocks = ceil( (sf8)len / (sf8)samps_per_block  );
      if (num_blocks < 1) {
        fprintf(stderr, "[%s] Improper header info- must encode 1 or more blocks\n", __FUNCTION__);
        return(1);
      }
      ip = index_block = (INDEX_DATA *)calloc(num_blocks, sizeof(INDEX_DATA));
      free_index = 1;
      for (i=0; i<num_blocks; i++) {
        ip->time = mef_header->recording_start_time + i * mef_header->block_interval;
        ip->sample_number = i * samps_per_block;
      }
    }
    compressed_buffer = (ui1*)calloc(num_blocks*samps_per_block, sizeof(si4)); 
    
    if (index_block == NULL || compressed_buffer == NULL) {
      fprintf(stderr, "[%s] malloc error %d\n", __FUNCTION__, __LINE__);
      return(1);
    }    
    
    //calculate discontinuity arry if none was passed in
    if (discontinuity_array == NULL) {
      discontinuity_array = (ui1 *)calloc(num_blocks, sizeof(ui1));
      free_discont = 1;
      if (discontinuity_array== NULL) {
        fprintf(stderr, "[%s] malloc error %d\n", __FUNCTION__, __LINE__);
        free(header);
        if (free_index) {free(index_block); index_block=NULL;}
        return(1);
      }
      discontinuity_array[0] = 1;
      for (i=1; i<num_blocks; i++) {
        ds = (sf8)(index_block[i].sample_number - index_block[i-1].sample_number);
        dt = (sf8)(index_block[i].time - index_block[i-1].time)/1000000.0;
        if (fabs(dt - ds/mef_header->sampling_frequency) > (sf8)mef_header->block_interval/(2.0*1000000.0))
          discontinuity_array[i]=1;
      }
    }
    
    max_value = INT_MIN; min_value = INT_MAX; 
    sp = samps;	
    cbp = compressed_buffer; 
    ip = index_block;
    dataCounter = MEF_HEADER_LENGTH; 
    entryCounter=0; 
    max_block_size = 0;
    
    samps_left = len;
    for (i=0; i<num_blocks; i++) {
      ip->file_offset = dataCounter; 
      
      if (i==num_blocks-1)
        samps_to_encode = mef_header->number_of_samples - index_block[i].sample_number;
      else
        samps_to_encode = index_block[i+1].sample_number - index_block[i].sample_number;
      
      if (samps_left < samps_to_encode) samps_to_encode = (ui4)samps_left;		
      
      RED_block_size = RED_compress_block(sp, cbp, samps_to_encode, ip->time, (ui1)discontinuity_array[i], encryption_key, mef_header->data_encryption_used, &RED_bk_hdr);
      
      dataCounter += RED_block_size;
      cbp += RED_block_size;
      entryCounter += RED_bk_hdr.sample_count;
      samps_left -= RED_bk_hdr.sample_count;
      sp += RED_bk_hdr.sample_count;
      ip++;
      
      if (RED_bk_hdr.max_value > max_value) max_value = RED_bk_hdr.max_value;
      if (RED_bk_hdr.min_value < min_value) min_value = RED_bk_hdr.min_value;
      if (RED_block_size > max_block_size) max_block_size = RED_block_size;
      if (samps_to_encode > mef_header->maximum_block_length) mef_header->maximum_block_length = samps_to_encode;
    }
    
    //update mef header with new values
    mef_header->maximum_data_value = max_value;
    mef_header->minimum_data_value = min_value;
    mef_header->maximum_compressed_block_size = (ui4)max_block_size;
    mef_header->number_of_index_entries = num_blocks;
    
    // write mef entries
    nr = fwrite(compressed_buffer, sizeof(si1), (size_t) dataCounter - MEF_HEADER_LENGTH, fp); 
    if (nr != dataCounter - MEF_HEADER_LENGTH) { fprintf(stderr, "Error writing file\n"); fclose(fp); return(1); }
    
    //byte align index data if needed
    index_data_offset = ftell(fp);
    byte_offset = (si4)(index_data_offset % 8);
    if (byte_offset) {
      memset(byte_padding, 0, 8);
      fwrite(byte_padding, sizeof(ui1), 8 - byte_offset, fp);
      index_data_offset += 8 - byte_offset;
    }
    mef_header->index_data_offset = index_data_offset;
    
    //write index offset block to end of file
    nr = fwrite(index_block, sizeof(INDEX_DATA), (size_t) num_blocks, fp); 
    
    //build mef header from structure
    nr = build_mef_header_block(header, mef_header, subject_password); //recycle nr
    if (nr) { fprintf(stderr, "Error building mef header\n"); return(1); }
    
    fseek(fp, 0, SEEK_SET); //reset fp to beginning of file to write mef header
    nr = fwrite(header, sizeof(ui1), (size_t) MEF_HEADER_LENGTH, fp);
    if (nr != MEF_HEADER_LENGTH) { fprintf(stderr, "Error writing mef header\n"); return(1); }
    
    fclose(fp);
    
    free(compressed_buffer); compressed_buffer = NULL;
    
    if (free_index) {
      free(index_block);
      index_block = NULL;
    }
    
    return(0);
  }


EXPORT
  si4	build_RED_block_header(ui1 *header_block, RED_BLOCK_HDR_INFO *header_struct)
  {
    ui1	*ui1_p, *ui1_p2;
    ui4	block_len, comp_block_len, diff_cnts, crc, *ui4_p;
    si1 discontinuity, *si1_p;
    si4 i, max_data_value, min_data_value;
    ui8 time_value, *ui8_p;	

    //check inputs
    if (header_block==NULL) {
      fprintf(stderr, "[%s] NULL header block passed in\n", __FUNCTION__);
      return(1);
    }
    if (header_struct==NULL) {
      fprintf(stderr, "[%s] NULL block header structure pointer passed in\n", __FUNCTION__);
      return(1);
    }
    
    //perhaps this is overly cautious, but we'll copy structure values to intermediate variables to avoid
    //the possibility of overwriting the structure fields by mistake
    comp_block_len = header_struct->compressed_bytes; 
    time_value = header_struct->block_start_time;
    diff_cnts = header_struct->difference_count;
    block_len = header_struct->sample_count;
    max_data_value = header_struct->max_value; 
    min_data_value = header_struct->min_value;
    discontinuity = header_struct->discontinuity;
    
    ui4_p = (ui4*)(header_block + RED_CHECKSUM_OFFSET); //this should be unnecessary as we skip the first 4 bytes in the CRC calculation
    *ui4_p = 0;
    
    ui4_p = (ui4*)(header_block + RED_COMPRESSED_BYTE_COUNT_OFFSET);
    *ui4_p = comp_block_len;
    
    ui8_p = (ui8*)(header_block + RED_UUTC_TIME_OFFSET);
    *ui8_p = time_value;
    
    ui4_p = (ui4*)(header_block + RED_DIFFERENCE_COUNT_OFFSET);
    *ui4_p = diff_cnts;
    
    ui4_p = (ui4*)(header_block + RED_SAMPLE_COUNT_OFFSET);
    *ui4_p = block_len;
    
    ui1_p = (ui1*)(header_block + RED_DATA_MAX_OFFSET);
    ui1_p2 = (ui1*) &max_data_value; //encode max and min values as si3
    for (i = 0; i < 3; ++i)
      *ui1_p++ = *ui1_p2++;
    
    ui1_p = (ui1*)(header_block + RED_DATA_MIN_OFFSET);
    ui1_p2 = (ui1 *) &min_data_value; //encode max and min values as si3
    for (i = 0; i < 3; ++i) 
      *ui1_p++ = *ui1_p2++;
    
    
    si1_p = (si1*)(header_block + RED_DISCONTINUITY_OFFSET);
    *si1_p = discontinuity;
    
    //Now that all the values are copied, update the CRC
    crc = calculate_compressed_block_CRC(header_block);
    ui4_p = (ui4*)(header_block + RED_CHECKSUM_OFFSET); 
    *ui4_p = crc;
    
    return(0);
  }

EXPORT
  si4	read_RED_block_header(ui1 *header_block, RED_BLOCK_HDR_INFO *header_struct)
  {
    ui1	*ib_p, *ui1_p;
    ui4	block_len, comp_block_len, diff_cnts, checksum_read;
    si1 discontinuity;
    si4 i, max_data_value, min_data_value;
    ui8 time_value;	
    
    //check inputs
    if (header_block==NULL) {
      fprintf(stderr, "[%s] NULL header block passed in\n", __FUNCTION__);
      return(1);
    }
    
    if (header_struct==NULL) {
      fprintf(stderr, "[%s] NULL block header structure pointer passed in\n", __FUNCTION__);
      return(1);
    }
    
    /*** parse block header ***/
    ib_p = header_block;
    
    checksum_read = *(ui4 *)(ib_p + RED_CHECKSUM_OFFSET); 
    comp_block_len = *(ui4 *)(ib_p + RED_COMPRESSED_BYTE_COUNT_OFFSET); 
    time_value = *(ui8 *)(ib_p + RED_UUTC_TIME_OFFSET);
    diff_cnts = *(ui4 *)(ib_p + RED_DIFFERENCE_COUNT_OFFSET);
    block_len = *(ui4 *)(ib_p + RED_SAMPLE_COUNT_OFFSET);
    
    max_data_value = 0; min_data_value = 0;
    
    ib_p = header_block + RED_DATA_MAX_OFFSET;
    ui1_p = (ui1 *) &max_data_value; 
    for (i = 0; i < 3; ++i) { *ui1_p++ = *ib_p++; }	
    *ui1_p++ = (*(si1 *)(ib_p-1)<0) ? -1 : 0; //sign extend
    
    ib_p = header_block + RED_DATA_MIN_OFFSET;
    ui1_p = (ui1 *) &min_data_value; 
    for (i = 0; i < 3; ++i) { *ui1_p++ = *ib_p++; }	
    *ui1_p++ = (*(si1 *)(ib_p-1)<0) ? -1 : 0; //sign extend
    
    ib_p = header_block;
    discontinuity = *(ib_p + RED_DISCONTINUITY_OFFSET);
    
    header_struct->CRC_32 = checksum_read;
    header_struct->compressed_bytes = comp_block_len;
    header_struct->block_start_time = time_value;
    header_struct->sample_count = block_len;
    header_struct->difference_count = diff_cnts;
    header_struct->max_value = max_data_value;
    header_struct->min_value = min_data_value;
    header_struct->discontinuity = discontinuity;
    
    header_struct->CRC_validated = 0; //This function performs no CRC validation
    
    return(0);
  }

ui4 calculate_compressed_block_CRC(ui1 *data_block) 
{
  int i, result;
  ui4 checksum, block_len;
  RED_BLOCK_HDR_INFO bk_hdr;
  
  if (data_block == NULL) {
    fprintf(stderr, "[%s] Error: NULL data pointer passed in\n", __FUNCTION__);
    return(1);
  }
  
  result = read_RED_block_header(data_block, &bk_hdr);
  if (result) {
    fprintf(stderr, "[%s] Error reading RED block header\n", __FUNCTION__);
    return(1);
  }
  
  block_len = bk_hdr.compressed_bytes + BLOCK_HEADER_BYTES;
  
  //calculate CRC checksum - skip first 4 bytes
  checksum = 0xffffffff;
  for (i = RED_CHECKSUM_LENGTH; i < block_len; i++) //skip first 4 bytes- don't include the CRC itself in calculation
    checksum = update_crc_32(checksum, *(data_block + i));
  
  return checksum;
}


si4 validate_mef(char *mef_filename, char *log_filename, char *password)
{
  int i, blocks_per_read;
  ui1 encr_hdr[MEF_HEADER_LENGTH], *data, logfile, bad_index;
  ui8 n, data_end, data_start, calc_end_time, num_errors;
  si8 offset, dt, ds, max_dt;
  ui4 crc, block_size;
  char message[200], *time_str;
  FILE *mfp, *lfp;
  MEF_HEADER_INFO header;
  RED_BLOCK_HDR_INFO bk_hdr;
  INDEX_DATA *indx_array;
  time_t now;
  
  blocks_per_read = 3000;
  num_errors = 0;
  bad_index = 0;
  
  if (mef_filename == NULL) {
    fprintf(stderr, "[%s] Error: NULL mef filename pointer passed in\n", __FUNCTION__);
    return(1);
  }
  
  //NULL or empty log_filename directs output to stdout only
  if ((log_filename == NULL)||(*log_filename==0)) {
    lfp = NULL;
    logfile = 0;
  }
  else {
    //check to see if log file exists
    logfile = 1;
    lfp = fopen(log_filename, "r");
    if (lfp != NULL)
      fprintf(stdout, "[%s] Appending to existing logfile %s\n", __FUNCTION__, log_filename);
    fclose(lfp);
    lfp = fopen(log_filename, "a+");
    if (lfp == NULL) {
      fprintf(stderr, "[%s] Error opening %s for writing\n", __FUNCTION__, log_filename);
      return(1);
    }
  }
  
  mfp = fopen(mef_filename, "r");
  if (mfp == NULL) {fprintf(stderr, "[%s] Error opening mef file %s\n", __FUNCTION__, mef_filename); return(1);}
  
  n = fread(encr_hdr, 1, MEF_HEADER_LENGTH, mfp);
  if (n != MEF_HEADER_LENGTH || ferror(mfp)) {fprintf(stderr, "[%s] Error reading mef header %s\n", __FUNCTION__, mef_filename); return(1); }
  
  //Check that this is a valid mef2 file
  if (*(ui1*)(encr_hdr + HEADER_MAJOR_VERSION_OFFSET) != 2) {fprintf(stderr, "[%s] Error: file %s does not appear to be a valid MEF file\n", __FUNCTION__, mef_filename); return(1);}
  
  n = read_mef_header_block(encr_hdr, &header, password);
  if (n) {fprintf(stderr, "[%s] Error decrypting mef header %s\n", __FUNCTION__, mef_filename); return(1);}
  
  n = fseek(mfp, header.index_data_offset, SEEK_SET);
  if (n) {fprintf(stderr, "[%s] fseek error in %s\n", __FUNCTION__, mef_filename); return(1);}
  
  indx_array = (INDEX_DATA*)calloc(header.number_of_index_entries, sizeof(INDEX_DATA));
  if (indx_array==NULL) {fprintf(stderr, "[%s] index malloc error while checking %s\n", __FUNCTION__, mef_filename); return(1); }
  
  n = fread(indx_array, sizeof(INDEX_DATA), header.number_of_index_entries, mfp);
  if (n != header.number_of_index_entries || ferror(mfp)) {fprintf(stderr, "[%s] Error reading mef index array %s\n", __FUNCTION__, mef_filename); return(1); }
  
  if (blocks_per_read > header.number_of_index_entries) 
    blocks_per_read = (int)header.number_of_index_entries;
  data = (ui1*)calloc(header.maximum_compressed_block_size, blocks_per_read);
  if (indx_array==NULL) {fprintf(stderr, "[%s] data malloc error while checking %s\n", __FUNCTION__, mef_filename); return(1); }
  
  now = time(NULL);
  time_str = ctime(&now); time_str[24]=0;
  
  sprintf(message, "\n%s: Beginning MEF validation check of file %s\n", time_str, mef_filename);
  fprintf(stdout, "%s", message);
  if (logfile) fprintf(lfp, "%s", message);	
  
  //// Begin checking mef file ///
  //Check header recording times against index array
  if (header.recording_start_time != indx_array[0].time) {
    num_errors++;
    sprintf(message, "Header recording_start_time %lu does not match index array time %lu\n", header.recording_start_time, indx_array[0].time);
    fprintf(stdout, "%s", message);
    if (logfile) fprintf(lfp, "%s", message);
  }
  calc_end_time = header.recording_start_time + (ui8)(0.5 + 1000000.0 * (sf8)header.number_of_samples/header.sampling_frequency);
  if (header.recording_end_time < calc_end_time) {
    num_errors++;
    sprintf(message, "Header recording_end_time %lu does not match sampling freqency and number of samples\n", header.recording_end_time);
    fprintf(stdout, "%s", message);
    if (logfile) fprintf(lfp, "%s", message);
  }
  max_dt=0;
  for (i=1; i<header.number_of_index_entries; i++) {
    offset = (si8)(indx_array[i].file_offset - indx_array[i-1].file_offset);
    if (offset > header.maximum_compressed_block_size || offset < 0) {
      num_errors++; bad_index = 1;
      sprintf(message, "Bad block index offset %ld between block %d and %d\n", offset, i-1, i);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
    dt = (si8)(indx_array[i].time - indx_array[i-1].time);
    if (dt < 0) {
      num_errors++; bad_index = 1;
      sprintf(message, "Bad block timestamps: %lu in block %d and %lu in block %d (diff %ld)\n", indx_array[i-1].time, i-1, indx_array[i].time, i, dt);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
    ds = (si8)(indx_array[i].sample_number - indx_array[i-1].sample_number);
    if (ds > header.maximum_block_length || ds < 0) {
      num_errors++; bad_index = 1;
      sprintf(message, "Bad block sample numbers: %lu in block %d and %lu in block %d\n", indx_array[i-1].sample_number, i-1, indx_array[i].sample_number, i);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
  }	
  
  if (bad_index) return(num_errors);
  
  data_end = indx_array[0].file_offset;
  
  //Loop through data blocks
  for (i=0; i<header.number_of_index_entries; i++) {
    if(indx_array[i].file_offset == data_end) { //read data
      if (i + blocks_per_read >= header.number_of_index_entries) {
        blocks_per_read = header.number_of_index_entries - i;
        data_end = header.index_data_offset;
      }
      else {
        data_end = indx_array[blocks_per_read + i].file_offset;
      }
      
      data_start = indx_array[i].file_offset;
      fseek(mfp, indx_array[i].file_offset, SEEK_SET);
      n = fread(data, 1, data_end - indx_array[i].file_offset, mfp);
      if (n != data_end - indx_array[i].file_offset || ferror(mfp)) {
        fprintf(stderr, "[%s] Error reading mef data %s\n", __FUNCTION__, mef_filename); 
        fclose(mfp); fclose(lfp);
        return(1); 
      }
    }
    offset = indx_array[i].file_offset - data_start;
    n = read_RED_block_header(data + offset, &bk_hdr);
    
    //check that the block length agrees with index array to within 8 bytes
    //(differences less than 8 bytes caused by padding to maintain boundary alignment)
    if (i <  header.number_of_index_entries-1)
      block_size = indx_array[i+1].file_offset - indx_array[i].file_offset;
    else 
      block_size = header.index_data_offset - indx_array[i].file_offset;
    
    if ( fabs(block_size - (bk_hdr.compressed_bytes + BLOCK_HEADER_BYTES)) > 8 )
    {
      num_errors++;
      sprintf(message, "%s: Block %d size %u disagrees with index array offset %u\n", mef_filename, i, 
              (bk_hdr.compressed_bytes + BLOCK_HEADER_BYTES), block_size);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
    else //DON'T check CRC if block size is wrong- will crash the program
    {
      crc = calculate_compressed_block_CRC(data + offset);
      
      if (crc != bk_hdr.CRC_32) {
        num_errors++;
        sprintf(message, "%s: CRC error in block %d\n", mef_filename, i);
        fprintf(stdout, "%s", message);
        fprintf(stdout, "samples %d time %lu diff_count %d max %d min %d discontinuity %d\n", bk_hdr.sample_count, 
                bk_hdr.block_start_time, bk_hdr.difference_count, bk_hdr.max_value, bk_hdr.min_value, 
                bk_hdr.discontinuity);
        if (logfile) {
          fprintf(lfp, "%s", message);
          fprintf(lfp, "samples %d time %lu diff_count %d max %d min %d discontinuity %d\n", bk_hdr.sample_count, 
                  bk_hdr.block_start_time, bk_hdr.difference_count, bk_hdr.max_value, bk_hdr.min_value, 
                  bk_hdr.discontinuity);
        }
      }
    }
    //check data block boundary alignment in file
    if (indx_array[i].file_offset % 8) {
      num_errors++;
      sprintf(message, "%s: Block %d is not 8-byte boundary aligned \n", mef_filename, i);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
    if (bk_hdr.block_start_time < header.recording_start_time) {
      num_errors++;
      sprintf(message, "%s: Block %d start time %lu is earlier than recording start time\n", mef_filename, i, bk_hdr.block_start_time);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
    if (bk_hdr.block_start_time > header.recording_end_time) {
      num_errors++;
      sprintf(message, "%s: Block %d start time %lu is later than recording end time\n", mef_filename, i, bk_hdr.block_start_time);
      fprintf(stdout, "%s", message);
      if (logfile) fprintf(lfp, "%s", message);
    }
  }
  
  now = time(NULL);
  sprintf(message, "File %s check of %lu data blocks completed with %lu errors found.\n\n", mef_filename, header.number_of_index_entries, num_errors);
  fprintf(stdout, "%s", message);
  if (logfile) fprintf(lfp, "%s", message);
  
  free(indx_array); indx_array = NULL;
  free(data); data = NULL;
  
  fclose(mfp);
  if (logfile) fclose(lfp);
  
  return(num_errors);
}

ui4 update_crc_32(ui4 crc, si1 c ) {
  ui4	tmp, long_c;
  
  long_c = 0x000000ff & (ui4) c;
  tmp = crc ^ long_c;
  crc = (crc >> 8) ^ crc_tab32[ tmp & 0xff ];
  
  return crc;
  
}  /* update_crc_32 */


void dec_normalize(ui4 *range, ui4 *low_bound, ui1 *in_byte, ui1 **ib_p)
{
  ui4 low, rng;
  ui1 in, *ib;
  
  low = *low_bound; 
  in = *in_byte;
  rng = *range;
  ib = *ib_p;
  
  while (rng <= BOTTOM_VALUE)
  {   low = (low << 8) | ((in << EXTRA_BITS) & 0xff);
    in = *ib++;
    low |= in >> (8 - EXTRA_BITS);
    rng <<= 8;
  }
  *low_bound = low; 
  *in_byte = in;
  *range = rng;
  *ib_p = ib;
  
  return;
}


ui8 RED_decompress_block(ui1 *in_buffer, si4 *out_buffer, si1 *diff_buffer, ui1 *key, ui1 validate_CRC, ui1 data_encryption, RED_BLOCK_HDR_INFO *block_hdr_struct)
{
  ui4	cc, cnts[256], cum_cnts[257], block_len, comp_block_len, checksum;
  ui4	symbol, scaled_tot_cnts, tmp, range_per_cnt, diff_cnts, checksum_read;
  ui1	*ui1_p;
  si1	*si1_p1, *si1_p2, *db_p, discontinuity;
  si4	i, current_val, *ob_p, max_data_value, min_data_value;
  ui8 time_value;
  ui4	low_bound;
  ui4	range;
  ui1	in_byte;
  ui1	*ib_p;
  
  /*** parse block header ***/
  ib_p = in_buffer;
  checksum_read = *(ui4 *)ib_p; ib_p += 4;
  comp_block_len = *(ui4 *)ib_p; ib_p += 4;
  time_value = *(ui8 *)ib_p; ib_p += 8;
  diff_cnts = *(ui4 *)ib_p; ib_p += 4;
  block_len = *(ui4 *)ib_p; ib_p += 4;
  
  max_data_value = 0; min_data_value = 0;
  ui1_p = (ui1 *) &max_data_value; 
  for (i = 0; i < 3; ++i) { *ui1_p++ = *ib_p++; }	
  *ui1_p++ = (*(si1 *)(ib_p-1)<0) ? -1 : 0; //sign extend
  ui1_p = (ui1 *) &min_data_value; 
  for (i = 0; i < 3; ++i) { *ui1_p++ = *ib_p++; }	
  *ui1_p++ = (*(si1 *)(ib_p-1)<0) ? -1 : 0; //sign extend
  
  discontinuity = *ib_p++;
  
  if (validate_CRC==MEF_TRUE && block_hdr_struct != NULL) {
    //calculate CRC checksum to validate- skip first 4 bytes
    checksum = 0xffffffff;
    for (i = 4; i < comp_block_len + BLOCK_HEADER_BYTES; i++)
      checksum = update_crc_32(checksum, *(out_buffer+i));
    
    if (checksum != checksum_read) block_hdr_struct->CRC_validated = 0;
    else block_hdr_struct->CRC_validated = 1;
  }
  
  if (data_encryption==MEF_TRUE) {
    if (key==NULL) {
      fprintf(stderr, "[%s] Error: Null Encryption Key with encrypted block header\n", __FUNCTION__);
      return(-1);
    } else {
      AES_decryptWithKey(ib_p, ib_p, key); //pass in expanded key
    }
  }
  
  for (i = 0; i < 256; ++i) { cnts[i] = (ui4) *ib_p++; }
  
  if (block_hdr_struct != NULL) {	
    block_hdr_struct->CRC_32 = checksum_read;
    block_hdr_struct->block_start_time = time_value;
    block_hdr_struct->compressed_bytes = comp_block_len;
    block_hdr_struct->difference_count = diff_cnts;
    block_hdr_struct->sample_count = block_len;
    block_hdr_struct->max_value = max_data_value;
    block_hdr_struct->min_value = min_data_value;
    block_hdr_struct->discontinuity = discontinuity;
  }
  
  /*** generate statistics ***/
  cum_cnts[0] = 0;
  for (i = 0; i < 256; ++i)
    cum_cnts[i + 1] = cnts[i] + cum_cnts[i];
  scaled_tot_cnts = cum_cnts[256];
  
  
  /*** range decode ***/
  diff_buffer[0] = -128; db_p = diff_buffer + 1;	++diff_cnts;	// initial -128 not coded in encode (low frequency symbol)
  ib_p = in_buffer + BLOCK_HEADER_BYTES + 1;	// skip initial dummy byte from encode
  in_byte = *ib_p++;
  low_bound = in_byte >> (8 - EXTRA_BITS);
  range = (ui4) 1 << EXTRA_BITS;
  for (i = diff_cnts; i--;) {
    dec_normalize(&range, &low_bound, &in_byte, &ib_p);
    
    tmp = low_bound / (range_per_cnt = range / scaled_tot_cnts);			
    cc = (tmp >= scaled_tot_cnts ? (scaled_tot_cnts - 1) : tmp);
    if (cc > cum_cnts[128]) {
      for (symbol = 255; cum_cnts[symbol] > cc; symbol--);
    } else {
      for (symbol = 1; cum_cnts[symbol] <= cc; symbol++);
      --symbol;
    }
    low_bound -= (tmp = range_per_cnt * cum_cnts[symbol]);
    if (symbol < 255)
      range = range_per_cnt * cnts[symbol];
    else
      range -= tmp;
    *db_p++ = symbol;
  }
  dec_normalize(&range, &low_bound, &in_byte, &ib_p);
  
  /*** generate output data from differences ***/
  si1_p1 = (si1 *) diff_buffer;
  ob_p = out_buffer;
  for (current_val = 0, i = block_len; i--;) {
    if (*si1_p1 == -128) {					// assumes little endian input
      si1_p2 = (si1 *) &current_val;
      *si1_p2++ = *++si1_p1; *si1_p2++ = *++si1_p1; *si1_p2++ = *++si1_p1;
      *si1_p2 = (*si1_p1++ < 0) ? -1 : 0;
    } else
      current_val += (si4) *si1_p1++;
    *ob_p++ = current_val;
  }
  return(comp_block_len + BLOCK_HEADER_BYTES);
}



void enc_normalize(RANGE_STATS *rstats)
{
  while (rstats->range <= BOTTOM_VALUE) {
    if (rstats->low_bound < (ui4 ) CARRY_CHECK) {		// no carry possible => output
      *(rstats->ob_p++) = rstats->out_byte;
      for(; rstats->underflow_bytes; rstats->underflow_bytes--)
        *(rstats->ob_p++) = 0xff;
      rstats->out_byte = (ui1) (rstats->low_bound >> SHIFT_BITS);
    } else if (rstats->low_bound & TOP_VALUE) {		// carry now, no future carry
      *(rstats->ob_p++) = rstats->out_byte + 1;
      for(; rstats->underflow_bytes; rstats->underflow_bytes--)
        *(rstats->ob_p++) = 0;
      rstats->out_byte = (ui1) (rstats->low_bound >> SHIFT_BITS);
    } else						// pass on a potential carry
      rstats->underflow_bytes++;
    rstats->range <<= 8;
    rstats->low_bound = (rstats->low_bound << 8) & TOP_VALUE_M_1;
  }
  
  return;
}


void encode_symbol(ui1 symbol, ui4 symbol_cnts, ui4 cnts_lt_symbol, ui4 tot_cnts, RANGE_STATS *rstats )
{
  ui4	r, tmp;
  
  enc_normalize(rstats);
  rstats->low_bound += (tmp = (r = rstats->range / tot_cnts) * cnts_lt_symbol);
  if (symbol < 0xff)			// not last symbol
    rstats->range = r * symbol_cnts;
  else						// last symbol
    rstats->range -= tmp;	// special case improves compression
  // at expense of speed
  return;
}


void done_encoding(RANGE_STATS *rstats)
{
  ui4	tmp;
  
  enc_normalize(rstats);
  
  tmp = rstats->low_bound;
  tmp = (tmp >> SHIFT_BITS) + 1;
  if (tmp > 0xff) {
    *(rstats->ob_p++) = rstats->out_byte + 1;
    for(; rstats->underflow_bytes; rstats->underflow_bytes--)
      *(rstats->ob_p++) = 0;
  } else {
    *(rstats->ob_p++) = rstats->out_byte;
    for(; rstats->underflow_bytes; rstats->underflow_bytes--)
      *(rstats->ob_p++) = 0xff;
  }
  *(rstats->ob_p++) = tmp & 0xff; *(rstats->ob_p++) = 0; *(rstats->ob_p++) = 0; *(rstats->ob_p++) = 0;
  
  return;
}

/* get cpu endianness: 0 = big, 1 = little */
ui1	cpu_endianness()
{
  ui2	x = 1;
  
  return(*((ui1 *) &x));
}

/* in place */
void	reverse_in_place(void *x, si4 len)
{
  ui1	*pf, *pb, t;
  si4	i;
  
  pf = (ui1 *) x;
  pb = pf + len;
  for (i = len >> 1; i--;) {
    t = *pf;
    *pf++ = *--pb;
    *pb = t;
  }
}

/* value returning functions */
si2	rev_si2(si2 x)
{
  ui1	*pf, *pb;
  si2	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 1;
  
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
}

ui2	rev_ui2(ui2 x)
{
  ui1	*pf, *pb;
  ui2	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 1;
  
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
}

si4	rev_si4(si4 x)
{
  ui1	*pf, *pb;
  si4	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 3;
  
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
}

ui4	rev_ui4(ui4 x)
{
  ui1	*pf, *pb;
  ui4	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 3;
  
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
}

sf4	rev_sf4(sf4 x)
{
  ui1	*pf, *pb;
  sf4	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 3;
  
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
}

si8	rev_si8(si8 x)
{
  ui1	*pf, *pb;
  si8	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 7;
  
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
  
}

ui8	rev_ui8(ui8 x)
{
  ui1	*pf, *pb;
  ui8	xr;
  
  if (x)
  {
    pf = (ui1 *) &x;
    pb = (ui1 *) &xr + 7;
    
    *pb-- = *pf++;
    *pb-- = *pf++;
    *pb-- = *pf++;
    *pb-- = *pf++;
    *pb-- = *pf++;
    *pb-- = *pf++;
    *pb-- = *pf++;
    *pb = *pf; 
    
    return(xr);
  }
  else
  {
    return(x);
  }
}

sf8	rev_sf8(sf8 x)
{
  ui1	*pf, *pb;
  sf8	xr;
  
  pf = (ui1 *) &x;
  pb = (ui1 *) &xr + 7;
  
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb-- = *pf++;
  *pb = *pf;
  
  return(xr);
}


//
/*****************************************************************
 **       Advanced Encryption Standard implementation in C.      **
 **       By Niyaz PK                                            **
 **       E-mail: niyazlife@gmail.com                            **
 **       Downloaded from Website: www.hoozi.com                 **
 ******************************************************************
 This is the source code for encryption using the latest AES algorithm.
 AES algorithm is also called Rijndael algorithm. AES algorithm is 
 recommended for non-classified use by the National Institute of Standards 
 and Technology (NIST), USA. Now-a-days AES is being used for almost 
 all encryption applications all around the world.
 
 For the complete description of the algorithm, see:
 http://www.csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 
 Find the Wikipedia page of AES at:
 http://en.wikipedia.org/wiki/Advanced_Encryption_Standard
 *****************************************************************/


/***********************************************************/
/* THE CODE IN THIS FILE IS SET FOR 128-BIT ENCRYPTION ONLY */
/***********************************************************/

#include <string.h>

// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4
// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}  
#define xtime(x) ((x<<1) ^ (((x>>7) & 1) * 0x1b))
// Multiplty is a macro used to multiply numbers in the field GF(2^8)
#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))


si4	getSBoxValue(si4 num)
{
  si4	sbox[256] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
  
  return(sbox[num]);
}


si4	getSBoxInvert(si4 num)
{
  si4	rsbox[256] ={ 
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
  
  return(rsbox[num]);
}


// This function produces Nb(Nr+1) round keys. The round keys are used in each round to encrypt the states. 
//NOTE: make sure Key array is zeroed before copying password
void	AES_KeyExpansion(si4 Nk, si4 Nr, ui1 *RoundKey, si1 *Key)
{
  // The round constant word array, Rcon[i], contains the values given by 
  // x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(28)
  // Note that i starts at 1, not 0).
  si4		Rcon[255] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };
  si4	i, j;
  ui1	temp[4], k;
  
  // The first round key is the key itself.
  for (i = 0; i < Nk; i++) {
    RoundKey[i * 4] = Key[i * 4];
    RoundKey[i * 4 + 1] = Key[i * 4 + 1];
    RoundKey[i * 4 + 2] = Key[i * 4 + 2];
    RoundKey[i * 4 + 3] = Key[i * 4 + 3];
  }
  
  // All other round keys are found from the previous round keys.
  while (i < (Nb * (Nr + 1))) {
    
    for (j = 0; j < 4; j++) {
      temp[j] = RoundKey[(i - 1) * 4 + j];
    }
    
    if (i % Nk == 0) {
      // This rotates the 4 bytes in a word to the left once.
      // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]
      k = temp[0];
      temp[0] = temp[1];
      temp[1] = temp[2];
      temp[2] = temp[3];
      temp[3] = k;
      
      // This takes a four-byte input word and applies the S-box
      // to each of the four bytes to produce an output word.
      temp[0] = getSBoxValue(temp[0]);
      temp[1] = getSBoxValue(temp[1]);
      temp[2] = getSBoxValue(temp[2]);
      temp[3] = getSBoxValue(temp[3]);
      
      temp[0] = temp[0] ^ Rcon[i / Nk];
    } else if (Nk > 6 && i % Nk == 4) {
      // This takes a four-byte input word and applies the S-box
      // to each of the four bytes to produce an output word.
      temp[0] = getSBoxValue(temp[0]);
      temp[1] = getSBoxValue(temp[1]);
      temp[2] = getSBoxValue(temp[2]);
      temp[3] = getSBoxValue(temp[3]);
    }
    
    RoundKey[i * 4] = RoundKey[(i - Nk) * 4] ^ temp[0];
    RoundKey[i * 4 + 1] = RoundKey[(i - Nk) * 4 + 1] ^ temp[1];
    RoundKey[i * 4 + 2] = RoundKey[(i - Nk) * 4 + 2] ^ temp[2];
    RoundKey[i * 4 + 3] = RoundKey[(i - Nk) * 4 + 3] ^ temp[3];
    
    i++;
  }
  
  return;
}


// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void	AddRoundKey(si4 round, ui1 state[][4], ui1 *RoundKey) 
{
  si4	i, j;
  
  for (i = 0;i < 4; i++) {
    for (j = 0;j < 4; j++) {
      state[j][i] ^= RoundKey[round * Nb * 4 + i * Nb + j];
    }
  }
  
  return;
}


// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void	SubBytes(ui1 state[][4])
{
  si4	i, j;
  
  for (i = 0;i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state[i][j] = getSBoxValue(state[i][j]);
    }
  }
  
  return;
}


void	InvSubBytes(ui1 state[][4])
{
  si4	i, j;
  
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state[i][j] = getSBoxInvert(state[i][j]);
    }
  }
  
  return;
}


// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void	ShiftRows(ui1 state[][4])
{
  ui1	temp;
  
  // Rotate first row 1 columns to left    
  temp = state[1][0];
  state[1][0] = state[1][1];
  state[1][1] = state[1][2];
  state[1][2] = state[1][3];
  state[1][3] = temp;
  
  // Rotate second row 2 columns to left    
  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;
  
  // Rotate third row 3 columns to left
  temp = state[3][0];
  state[3][0] = state[3][3];
  state[3][3] = state[3][2];
  state[3][2] = state[3][1];
  state[3][1] = temp;
  
  return;
}


void	InvShiftRows(ui1 state[][4])
{
  ui1	temp;
  
  // Rotate first row 1 columns to right   
  temp = state[1][3];
  state[1][3] = state[1][2];
  state[1][2] = state[1][1];
  state[1][1] = state[1][0];
  state[1][0] = temp;
  
  // Rotate second row 2 columns to right   
  temp = state[2][0];
  state[2][0] = state[2][2];
  state[2][2] = temp;
  
  temp = state[2][1];
  state[2][1] = state[2][3];
  state[2][3] = temp;
  
  // Rotate third row 3 columns to right
  temp = state[3][0];
  state[3][0] = state[3][1];
  state[3][1] = state[3][2];
  state[3][2] = state[3][3];
  state[3][3] = temp;
  
  return;
}


// MixColumns function mixes the columns of the state matrix
// The method used may look complicated, but it is easy if you know the underlying theory.
// Refer the documents specified above.
void	MixColumns(ui1 state[][4])
{
  si4	i;
  ui1	Tmp, Tm, t;
  
  for (i = 0; i < 4; i++) {    
    t = state[0][i];
    Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i];
    Tm = state[0][i] ^ state[1][i];
    Tm = xtime(Tm);
    state[0][i] ^= Tm ^ Tmp;
    Tm = state[1][i] ^ state[2][i];
    Tm = xtime(Tm);
    state[1][i] ^= Tm ^ Tmp;
    Tm = state[2][i] ^ state[3][i];
    Tm = xtime(Tm);
    state[2][i] ^= Tm ^ Tmp;
    Tm = state[3][i] ^ t;
    Tm = xtime(Tm);
    state[3][i] ^= Tm ^ Tmp;
  }
  
  return;
}


// The method used to multiply may be difficult to understand.
// Please use the references to gain more information.
void	InvMixColumns(ui1 state[][4])
{
  si4	i;
  ui1	a, b, c, d;
  
  for (i = 0; i < 4; i++) {   		
    a = state[0][i];
    b = state[1][i];
    c = state[2][i];
    d = state[3][i];		
    state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
  }
  
  return;
}


// Cipher is the main function that encrypts the PlainText.
void	Cipher(si4 Nr, ui1 *in, ui1 *out, ui1 state[][4], ui1 *RoundKey)
{
  si4	i, j, round = 0;
  
  //Copy the input PlainText to state array.
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      state[j][i] = in[i * 4 + j];
    }
  }
  
  // Add the First round key to the state before starting the rounds.
  AddRoundKey(0, state, RoundKey); 
  
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for (round = 1; round < Nr; round++) {
    SubBytes(state);
    ShiftRows(state);
    MixColumns(state);
    AddRoundKey(round, state, RoundKey);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  SubBytes(state);
  ShiftRows(state);
  AddRoundKey(Nr, state, RoundKey);
  
  // The encryption process is over.
  // Copy the state array to output array.
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      out[i * 4 + j] = state[j][i];
    }
  }
  
  return;
}

// InvCipher is the main function that decrypts the CipherText.
void	InvCipher(si4 Nr, ui1 *in, ui1 *out, ui1 state[][4], ui1 *RoundKey)
{
  si4	i, j, round = 0;
  
  //Copy the input CipherText to state array.
  for (i = 0; i < 4; i++) {
    for (j = 0;j < 4; j++) {
      state[j][i] = in[i * 4 + j];
    }
  }
  
  // Add the First round key to the state before starting the rounds.
  AddRoundKey(Nr, state, RoundKey);
  
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for (round = Nr - 1; round > 0; round--) {
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(round, state, RoundKey);
    InvMixColumns(state);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  InvShiftRows(state);
  InvSubBytes(state);
  AddRoundKey(0, state, RoundKey);
  
  // The decryption process is over.
  // Copy the state array to output array.
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      out[i * 4 + j]=state[j][i];
    }
  }
  
  return;
}

// in is buffer to be encrypted (16 bytes)
// out is encrypted buffer (16 bytes)
void	AES_encrypt(ui1 *in, ui1 *out, si1 *password)
{
  si4	Nr = 10; // The number of rounds in AES Cipher
  si4	Nk = 4; // The number of 32 bit words in the key
  si1	Key[16] = {0};
  ui1	state[4][4]; // the array that holds the intermediate results during encryption
  ui1	RoundKey[240]; // The array that stores the round keys
  
  // password becomes the key (16 bytes, zero-padded if shorter, truncated if longer)
  strncpy((char *) Key, password, 16);
  
  // The KeyExpansion routine must be called before encryption.
  AES_KeyExpansion(Nk, Nr, RoundKey, Key);
  
  // The next function call encrypts the PlainText with the Key using AES algorithm.
  Cipher(Nr, in, out, state, RoundKey);
  
  return;
}

//Pass in expanded key externally - this is more efficient if encrypting multiple times with
//the same encryption key
void	AES_encryptWithKey(ui1 *in, ui1 *out, ui1 *RoundKey)
{
  si4	Nr = 10; // The number of rounds in AES Cipher
  ui1	state[4][4]; // the array that holds the intermediate results during encryption
  
  // The next function call encrypts the PlainText with the Key using AES algorithm.
  Cipher(Nr, in, out, state, RoundKey);
  
  return;
}

// in is encrypted buffer (16 bytes)
// out is decrypted buffer (16 bytes)
void	AES_decrypt(ui1 *in, ui1 *out, si1 *password)
{
  si4	Nr = 10; // The number of rounds in AES Cipher
  si4	Nk = 4; // The number of 32 bit words in the key
  si1	Key[16] = {0};
  ui1	state[4][4]; // the array that holds the intermediate results during encryption
  ui1	RoundKey[240]; // The array that stores the round keys
  
  // password becomes the key (16 bytes, zero-padded if shorter, truncated if longer)
  strncpy((char *) Key, password, 16);
  
  //The Key-Expansion routine must be called before the decryption routine.
  AES_KeyExpansion(Nk, Nr, RoundKey, Key);
  
  // The next function call decrypts the CipherText with the Key using AES algorithm.
  InvCipher(Nr, in, out, state, RoundKey);
  
  return;
}

//Pass in expanded key externally - this is more efficient if encrypting multiple times with
//the same encryption key
void	AES_decryptWithKey(ui1 *in, ui1 *out, ui1 *RoundKey)
{
  si4		Nr = 10; // The number of rounds in AES Cipher
  ui1	state[4][4]; // the array that holds the intermediate results during encryption
  
  // The next function call decrypts the CipherText with the Key using AES algorithm.
  InvCipher(Nr, in, out, state, RoundKey);
  
  return;
}






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
  char file_name[1024];
  si1 *ext;
  int month_format_type;
  
  fprintf( stderr, "Got to sub-function.\n");
  
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
  
  
  strcpy(file_name, inFileName);
  
  fprintf( stderr, "Copied.\n" );
  
  ext = strrchr((si1 *) file_name, '.');
  if (ext != NULL)
    *ext = 0;
  
  fprintf( stderr, "ext found.\n" );

#ifndef OUTPUT_TO_MEF2
  sprintf(dir_name, "mef3");
#else
  fprintf( stderr, "MEF2.\n" );
  sprintf(dir_name, "mef2");
  fprintf( stderr, "dir_name.\n" );
  DIR* dir = opendir("mef2");
  if (dir) {
    /* Directory exists. */
    fprintf( stderr, "Directory exists.");
    closedir(dir);
  } else if (ENOENT == errno) {
    /* Directory does not exist. */
    system("mkdir mef2");
    fprintf( stderr, "mkdir.\n" );
  } else {
    /* opendir() failed for some other reason. */
    fprintf( stderr, "Directory name oddity.");
  }
#endif
  
  /* Strip path from 'file_name' */
  char* chan_name = basename( file_name );
  
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

//' @importFrom Rcpp evalCpp
//' @useDynLib meftools
//' @export
// [[Rcpp::export]]
int ncs2mef (Rcpp::StringVector strings) {
  int dataFailed = 0;
  int numFiles, uid, anon_flag;
  ui1	*uid_array;
  char subject_password[32], session_password[32];
  ui8 uutc_time;
  int i;
  int nev_count, ncs_count;
  
  if (strings.size() < 1) 
  {
    Rcpp::Rcout << "Usage: ncs2mef filename(.ncs) [filename.nev]" << "\n";
    return(1);
  }
  
  //defaults
  numFiles = strings.size();
  uid = 1;
  anon_flag = 1;  // anonymize the data
  
  // check for input file
  if (numFiles==0) {
    fprintf(stderr, "Error: No file specified\n");
    return(1);
  }
  
  uid_array = NULL;
  
  
  //*subject_password = 0;
  //*session_password = 0;
  strcpy(subject_password, "blah");
  strcpy(session_password, "blah");
  
  uutc_time = 0;
  
  nev_count = 0;
  for (i=0;i<numFiles;i++)
  {
    
    if (!strstr(strings[i], ".nev") == NULL)
      nev_count++;
  }
  
  if (nev_count > 1)
  {
    fprintf(stderr, "More than one event (.nev) file is specified, please fix this so that there is only one event file!  Exiting.\n");
    return(1);
  }
  
  ncs_count = 0;
  for (i=0;i < numFiles;i++)
  {
    
    // main processing, this is where NCS is read and .MEF files are written
    if (!strstr(strings[i], ".ncs") == NULL)
    {
      ncs_count++;
      uutc_time = read_ncs_file((si1*) strings[i], (si1*) uid_array, session_password, subject_password, anon_flag, uutc_time);
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
  
  for (i=0;i<numFiles;i++)
  {
    if (!strstr(strings[i], ".nev") == NULL)
      read_nev_file((si1*) strings[i], "mef3", uutc_time);
    
  }
  
  if (dataFailed)
    return 1;
  
  return 0;
}
