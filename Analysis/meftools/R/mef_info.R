mef_info <- function( args ) {
  #' Return a structure containing MEF header information.
  #' Based on Matlab code "read_mef_discont".
  #' 
  #' @param filename String: The complete path to a .mef file
  #' @param password String: The public password for the MEF file.
  #' @export
  library( meftools )
  
  options( warn = -1 )
  
  filename <- args[1]
  password <- args[2]

  # Read the headerpwd
  print( paste0( filename, ' ', password ) )
  header = read_mef_header(c(filename,password))
#  print( header )
  
  # Read the ToC
  # Use Rcpp to pass in 'index_data_offset' and 'number_of_index_entries', then
  # return the ToC.
  print( 'Reading ToC' )
  variables <- c(filename,header$index_data_offset,header$number_of_index_entries)
  print( paste0( variables ) )
  ToC <- table_of_contents( variables )
  print( 'Read ToC' )

  # Read the discontnuities
  # Use Rcpp to pass in 'number_of_index_entries', then
  # return the discontinuity flags.
  print( 'Reading discontinuities' )
  discontinuities <- get_discontinuities( c(filename,header$number_of_index_entries), ToC );
  print( 'Read discontinuities' )
  
  # Put the returnable values in a named list.
  value <- list( header=header, ToC=ToC, discontinuities=discontinuities );  
  return( value )
}
