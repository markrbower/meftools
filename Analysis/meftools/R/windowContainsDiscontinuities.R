windowContainsDiscontinuties <-  function( info, UTC_start, UTC_stop ) {
  # Find any contained discontinuities
  #
  #' @export
  discontinuity_block_times <- info$ToC[ 1, which(info$discontinuities==1) ]
  discontinuity_idx <- which( discontinuity_block_times >= UTC_start & discontinuity_block_times <= UTC_stop )
  return( discontinuity_idx )
}
