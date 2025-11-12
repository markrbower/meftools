


MEF_HEADER converRcppHeader( Rcpp::MEF_HEADER_INFO mef_header. {
    MEF_HEADER rcpp_header.

    copy( rcpp_header->institution, INSTITUTION_LENGTH, mef_header->institution );
    copy( rcpp_header->unencrypted_text_field, UNENCRYPTED_TEXT_FIELD_LENGTH , mef_header->unencrypted_text_field );
    copy( rcpp_header->encryption_algorithm, ENCRYPTION_ALGORITHM_LENGTH, mef_header->encryption_algorithm );
    mef_header.subject_encryption_used = rcpp_header.subject_encryption_used;
    mef_header.session_encryption_used = rcpp_header.session_encryption_used;
    mef_header.data_encryption_used = rcpp_header.data_encryption_used;
    mef_header.byte_order_code = rcpp_header.byte_order_code;
    mef_header.header_version_major = rcpp_header.header_version_major;
    mef_header.header_version_minor = rcpp_header.header_version_minor;
    copy( rcpp_header->session_unique_ID, SESSION_UNIQUE_ID_LENGTH, mef_header->session_unique_ID );
    mef_header.header_length = rcpp_header.header_length;
    copy( rcpp_header->subject_first_name, SUBJECT_FIRST_NAME_LENGTH, mef_header->subject_first_name );
    copy( rcpp_header->subject_second_name, SUBJECT_SECOND_NAME_LENGTH, mef_header->subject_second_name );
    copy( rcpp_header->subject_third_name, SUBJECT_THIRD_NAME_LENGTH, mef_header->subject_third_name );
    copy( rcpp_header->subject_id, SUBJECT_ID_LENGTH, mef_header->subject_id );
    copy( rcpp_header->session_password, SESSION_PASSWORD_LENGTH, mef_header->session_password );
    copy( rcpp_header->subject_validation_field, SUBJECT_VALIDATION_FIELD_LENGTH, mef_header->subject_validation_field );
    copy( rcpp_header->session_validation_field, SESSION_VALIDATION_FIELD_LENGTH, mef_header->session_validation_field );
    copy( rcpp_header->protected_region, PROTECTED_REGION_LENGTH, mef_header->protected_region );
    mef_header.number_of_samples = rcpp_header.number_of_samples;
    copy( rcpp_header->channel_name, CHANNEL_NAME_LENGTH, mef_header->channel_name );
    mef_header.recording_start_time = rcpp_header.recording_start_time;
    mef_header.recording_end_time = rcpp_header.recording_end_time;
    mef_header.sampling_frequency = rcpp_header.sampling_frequency;
    mef_header.low_frequency_filter_setting = rcpp_header.low_frequency_filter_setting;
    mef_header.high_frequency_filter_setting = rcpp_header.high_frequency_filter_setting;
    mef_header.notch_filter_frequency= rcpp_header.notch_filter_frequency;
    mef_header.noltage_conversion_factor = rcpp_header.voltage_conversion_factor;
    copy( rcpp_header->acquisition_system, ACQUISITION_SYSTEM_LENGTH, mef_header->acquisition_system );
    copy( rcpp_header->channel_comments, CHANNEL_COMMENTS_LENGTH, mef_header->channel_comments );
    copy( rcpp_header->study_comments, STUDY_COMMENTS_LENGTH, mef_header->study_comments );
    mef_header.physical_channel_number = rcpp_header.physical_channel_number;
    copy( rcpp_header->compression_algorithm, COMPRESSION_ALGORITHM_LENGTH, mef_header->compression_algorithm );
    mef_header.maximum_compressed_block_size = rcpp_header.maximum_compressed_block_size;
    mef_header.maximum_block_length = rcpp_header.maximum_block_length;
    mef_header.block_interval = rcpp_header.block_interval;
    mef_header.maximum_data_value = rcpp_header.maximum_data_value;
    mef_header.minimum_data_value = rcpp_header.minimum_data_value;
    mef_header.index_data_offset = rcpp_header.index_data_offset;
    mef_header.number_of_index_entries = rcpp_header.number_of_index_entries;
    mef_header.block_header_length = rcpp_header.block_header_length;
    mef_header.GMT_offset = rcpp_header.GMT_offset;
    mef_header.discontinuity_data_offset = rcpp_header.discontinuity_data_offset;
    mef_header.number_of_discontinuity_entries = rcpp_header.number_of_discontinuity_entries;
    copy( rcpp_header->file_unique_ID, FILE_UNIQUE_ID_LENGTH, mef_header->file_unique_ID );
    copy( rcpp_header->anonymized_subject_name, ANONYMIZED_SUBJECT_NAME_LENGTH, mef_header->anonymized_subject_name );
    mef_header.header_crc = rcpp_header.header_crc;


    mef_header.= rcpp_header.;

    copy( rcpp_header->, , mef_header-> );
  
//    ui4 header_crc;
    INDEX_DATA *file_index;
    ui8 *discontinuity_data;
  } MEF_HEADER; 

//  typedef struct {
//    si1 institution[INSTITUTION_LENGTH];
//    si1 unencrypted_text_field[UNENCRYPTED_TEXT_FIELD_LENGTH];
//    si1 encryption_algorithm[ENCRYPTION_ALGORITHM_LENGTH];
//    ui1 subject_encryption_used;
//    ui1 session_encryption_used;
//    ui1 data_encryption_used;
//    ui1 byte_order_code;
//    ui1 header_version_major;
//    ui1 header_version_minor;
//    ui1 session_unique_ID[SESSION_UNIQUE_ID_LENGTH];
//    ui2 header_length;
//    si1 subject_first_name[SUBJECT_FIRST_NAME_LENGTH];
//    si1 subject_second_name[SUBJECT_SECOND_NAME_LENGTH];
//    si1 subject_third_name[SUBJECT_THIRD_NAME_LENGTH];
//    si1 subject_id[SUBJECT_ID_LENGTH];
//    si1 session_password[SESSION_PASSWORD_LENGTH];
//    si1 subject_validation_field[SUBJECT_VALIDATION_FIELD_LENGTH];
//    si1 session_validation_field[SESSION_VALIDATION_FIELD_LENGTH];
//    si1 protected_region[PROTECTED_REGION_LENGTH];
//    ui8 number_of_samples;
//    si1 channel_name[CHANNEL_NAME_LENGTH];
//    ui8 recording_start_time;
//    ui8 recording_end_time;
//    sf8 sampling_frequency;
//    sf8 low_frequency_filter_setting;
//    sf8 high_frequency_filter_setting;
//    sf8 notch_filter_frequency;
//    sf8 voltage_conversion_factor;
//    si1 acquisition_system[ACQUISITION_SYSTEM_LENGTH];
//    si1 channel_comments[CHANNEL_COMMENTS_LENGTH];
//    si1 study_comments[STUDY_COMMENTS_LENGTH];
//    si4 physical_channel_number;
//    si1 compression_algorithm[COMPRESSION_ALGORITHM_LENGTH];
//    ui4 maximum_compressed_block_size;
//    ui8 maximum_block_length;
//    ui8 block_interval;
//    si4 maximum_data_value;
//    si4 minimum_data_value;
//    ui8 index_data_offset;
//    ui8 number_of_index_entries;
//    ui2 block_header_length;
//    sf4 GMT_offset;
//    ui8 discontinuity_data_offset;
//    ui8 number_of_discontinuity_entries;
//    ui1 file_unique_ID[FILE_UNIQUE_ID_LENGTH];
//    si1 anonymized_subject_name[ANONYMIZED_SUBJECT_NAME_LENGTH];






}



