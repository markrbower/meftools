MEFcont <- function( filename, password, ... ) {
  #' Creates an iterator of contiguous MEF sequences.
  #' 
  #' @param filename String: The complete path to a .mef file
  #' @param password String: The public password for the MEF file.
  #' @param block0 Integer: Start block number.
  #' @param block1 Integer: Stop block number.
  #' @param time0 Integer: Start time value (in microseconds)
  #' @param time1 Integer: Stop time value (in microseconds)
  #' @param step Integer: Time step (in seconds) for each iteration.
  #' @return A data iterator for MEF files.
  #' @export
  #' @examples
  #' \dontrun{
  #'   data_iter <- MEFiter( filename, password, time0=1.00E15, time1=1.01E15, step=10 )
  #'   data <- nextElem( data_iter )
  #' }
  
  # Parse arguments
  args <- list(...)
    for ( arg in names(args) ) {
      switch( arg,
              "time0" = {time0 = args[[arg]]},
              "time1" = {time1 = args[[arg]]},
              "window" = {tmp <-  args[[arg]]; time0 <- tmp['start']; time1 <- tmp['stop']},
              "bufferSize" = {bufferSize <- args[[arg]]},
              "info" = {info <- args[[arg]]}
      )
    }
  
  if ( !exists('info') ) {
    info <- mef_info( c(filename,password) )
  }

  # Set defaults
  if ( !exists( "time0" ) ) {time0 <- 0}
  if ( !exists( "time1" ) ) {time1 <- 1E20}
  if ( !exists( "block0" ) ) {block0 <- 1}
  if ( !exists( "block1" ) ) {block1 <- length( info$discontinuities )}

    i <- 1
    # Divide the continuous regions. Starts and Stops are inclusive.
    conts <- findContinuousMefSequences( info )
    # Keep only those continuous regions within the time/block bounds.
    contStartTimes <- info$ToC[1,conts$contiguousStarts]
    idx0 <- which( contStartTimes < time0 )
    contStopTimes <- info$ToC[1,conts$contiguousStops]
    idx1 <- which( contStopTimes > time1 )
    if ( length(idx0) == 0 ) {
      print( "MEFcont: No valid starts found." )
      return()
    }
    if ( length(idx1) == 0 ) {
      print( "MEFcont: No valid stops found." )
      return()
    }
    if ( idx0 > idx1 ) {
      print( "MEFcont: starts and stops do not overlap." )
      return()
    }
    newIdx <- idx0[length(idx0)]:idx1[1]
    newStarts <- conts$contiguousStarts[newIdx]
    newStops <- conts$contiguousStops[newIdx]
    conts <- data.frame( contiguousStarts=newStarts, contiguousStops=newStops )
    
    it <- iterators::iter( conts, by="row" )
    
    # Use blocks
    nextEl <- function() {
      values <- iterators::nextElem( it )
      mi <- MEFiter( filename, password, info=info, block0=values$contiguousStarts, block1=values$contiguousStops, time0=time0, time1=time1 )
      return( mi )
    }
    
    ihasNext <- function(it) {
      if (!is.null(it$hasNext)) return(it)
      cache <- NULL
      has_next <- NA
      
      nextEl <- function() {
        if (!hasNx())
          stop('StopIteration', call.=FALSE)
        has_next <<- NA
        meftools::MEFiter( filename, password, info=info, block0=cache$contiguousStarts, block1=cache$contiguousStops, time0=time0, time1=time1 )
      }
      
      hasNx <- function() {
        if (!is.na(has_next)) return(has_next)
        tryCatch({
          cache <<- iterators::nextElem(it)
          has_next <<- TRUE
        },
        error=function(e) {
          if (identical(conditionMessage(e), 'StopIteration')) {
            has_next <<- FALSE
            } else {
              stop(e)
            }
          })
        has_next
      }
        
      obj <- list(nextElem=nextEl, hasNext=hasNx)
      class(obj) <- c('ihasNext', 'abstractiter', 'iter')
      obj
    }
    
    props <- list("filename"=filename, "password"=password, "info"=info )
    
    obj <- list(nextElem=nextEl,hasNext=ihasNext)
    attr( obj, "props" ) <- props
    obj <- ihasNext(it)
    class(obj) <- c('ihasNext', 'abstractiter', 'iter', 'MEFcont')
    obj    
#    class(obj) <- c( 'MEFcont', 'abstractiter', 'iter' )
#    ihn_obj <- itertools::ihasNext( obj )
#    class(ihn_obj) <- c( 'MEFcont', 'ihasNext', 'abstractiter', 'iter' )
#    ihn_obj
}
