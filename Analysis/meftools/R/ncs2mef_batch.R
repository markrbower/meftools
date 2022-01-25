ncs2mef_batch <- function( directoryName ) {
  # usage:   ncs2mef_batch( directoryNameContainingNCSfiles )
  # example: ncs2mef_batch( '/Users/markrbower/Dropbox/Documents/Concepts/2018_07_29_meftools/meftools/Analysis/meftools/tests/Data' )
  #
  # output: .mef files placed in "<directoryName>/mef2".
  library( meftools )
  
  setwd( directoryName )
  ncs_filenames <- list.files( path='.', pattern='*.ncs', full.names=TRUE, recursive=TRUE )
  for ( ncs_filename in ncs_filenames ) {
    print( paste0( "Converting: ", ncs_filename ) )
    meftools::ncs2mef( c(ncs_filename) )
  }
  print( "Conversions complete.")
}
