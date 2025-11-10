


MEF_HEADER converRcppHeader( Rcpp::MEF_HEADER_INFO rcpp_header ) {
    MEF_HEADER mef_header;

    mef_header.institution = rcpp_header.institution;



    si1 unencrypted_text_field[UNENCRYPTED_TEXT_FIELD_LENGTH];
    si1 encryption_algorithm[ENCRYPTION_ALGORITHM_LENGTH];
    ui1 subject_encryption_used;
    ui1 session_encryption_used;
    ui1 data_encryption_used;
    ui1 byte_order_code;
    ui1 header_version_major;
    ui1 header_version_minor;
    ui1 session_unique_ID[SESSION_UNIQUE_ID_LENGTH];
    ui2 header_length;
    si1 subject_first_name[SUBJECT_FIRST_NAME_LENGTH];
    si1 subject_second_name[SUBJECT_SECOND_NAME_LENGTH];
    si1 subject_third_name[SUBJECT_THIRD_NAME_LENGTH];
    si1 subject_id[SUBJECT_ID_LENGTH];
    si1 session_password[SESSION_PASSWORD_LENGTH];
    si1 subject_validation_field[SUBJECT_VALIDATION_FIELD_LENGTH];
    si1 session_validation_field[SESSION_VALIDATION_FIELD_LENGTH];
    si1 protected_region[PROTECTED_REGION_LENGTH];
    ui8 number_of_samples;
    si1 channel_name[CHANNEL_NAME_LENGTH];
    ui8 recording_start_time;
    ui8 recording_end_time;
    sf8 sampling_frequency;
    sf8 low_frequency_filter_setting;
    sf8 high_frequency_filter_setting;
    sf8 notch_filter_frequency;
    sf8 voltage_conversion_factor;
    si1 acquisition_system[ACQUISITION_SYSTEM_LENGTH];
    si1 channel_comments[CHANNEL_COMMENTS_LENGTH];
    si1 study_comments[STUDY_COMMENTS_LENGTH];
    si4 physical_channel_number;
    si1 compression_algorithm[COMPRESSION_ALGORITHM_LENGTH];
    ui4 maximum_compressed_block_size;
    ui8 maximum_block_length;
    ui8 block_interval;
    si4 maximum_data_value;
    si4 minimum_data_value;
    ui8 index_data_offset;
    ui8 number_of_index_entries;
    ui2 block_header_length;
    sf4 GMT_offset;
    ui8 discontinuity_data_offset;
    ui8 number_of_discontinuity_entries;
    ui1 file_unique_ID[FILE_UNIQUE_ID_LENGTH];
    si1 anonymized_subject_name[ANONYMIZED_SUBJECT_NAME_LENGTH];
    ui4 header_crc;
    INDEX_DATA *file_index;
    ui8 *discontinuity_data;



    return( mef_header );
}



