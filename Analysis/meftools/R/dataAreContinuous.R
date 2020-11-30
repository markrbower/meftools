dataAreContinuous <- function( info, UTC_start, UTC_stop ) {
  #' @export
  discontinuities <- windowContainsDiscontinuties( info, UTC_start, UTC_stop )
  if ( length(discontinuities) == 0 ) {
    return( TRUE )
  } else {
    return( FALSE )
  }
}
