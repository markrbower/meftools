getMEFwindow <- function( filename, UTC_start, UTC_stop, info ) {
  # Returns MEF data contained within [UTC_start,UTC_stop].
  # If there is one or more discontinuities, data are returned up to the first discontinuity.
  #
  # Use "windowContainsDiscontinuities(...)" first to determine whether continuous data exists
  # for the entire window. If not, use "MEFcont" to return a list of iterators to provide
  # all available within the requested window, skipping discontinuities.
  #
  #' @export

  # Defaults
  if ( is.null(info) ) {
    vault <- topsecret::get_secret_vault()
    info <- meftools::mef_info( filename, secret::get_secret(name='NSME_halo_password', key=secret::local_key(), vault=vault ) )
  }
  
  # Check the bounds
  tmp0 <- which( info$ToC[1,] < UTC_start )
  tmp1 <- which( info$ToC[1,] < UTC_stop )
  if ( length(tmp0) == 0 & length(tmp1) > 0 ) {
    print( "getMEFwindow :: requested start time is out of data window." )
    return( NULL )
  } else if ( length(tmp0) > 0 & length(tmp1) == 0 ) {
    print( "getMEFwindow :: requested stop time is out of data window." )
    return( NULL )
  } else if ( length(tmp0) == 0 & length(tmp1) == 0 ) {
    print( "getMEFwindow :: requested start and stop times are out of data window." )
    return( NULL )
  }

  # Find start and stop blocks and samples
  block_start <- length(tmp0)
  sample_start <- info$ToC[3,block_start]
  
  block_stop <- length(tmp1) + 1
  discontinuity_idx <- meftools::windowContainsDiscontinuties( info, UTC_start, UTC_stop )
  if ( length(discontinuity_idx) > 0 ) {
    block_stop <- discontinuity_idx[1]
  }
  if ( block_stop > info$header$number_of_index_entries ) {
    sample_stop <- info$header$number_of_samples
  } else {
    sample_stop <- info$ToC[3,block_stop] - 1
  }

  # Prepare for password
  vault <- topsecret::get_secret_vault()
  data <- meftools::decomp_mef( c(filename, sample_start, sample_stop,
                    password=secret::get_secret(name="NSME_halo_password",key=secret::local_key(),vault=vault) ) )

  return( data )
}
