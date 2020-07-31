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
  
    time0 <- 0
    time1 <- 1E50
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
    print( time0 )
    print( time1 )

    # Use only meftools files everywhere
    info <- mef_info( c(filename,password) )
    
    i <- 1
    # Divide the continuous regions. Starts and Stops are inclusive.
    conts <- findContinuousMefSequences( info )
    it <- iterators::iter( conts, by="row" )
    
    # Use blocks
    nextEl <- function() {
      values <- nextElem( it )
      print(values)
#      print( paste0( colnames(values) ) )
      #print( paste0( filename ) )
#      print( values )
#      print( time0 )
#      print( time1 )
      
      mi <- MEFiter( filename, password, info=info, block0=values$contiguousStarts, block1=values$contiguousStops, time0=time0, time1=time1 )
#      MEFiter( filename=filename, password=password, info=info, time0=time0, time1=time1 )
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
        eval( cache() )
      }
      
      hasNx <- function() {
        if (!is.na(has_next)) return(has_next)
        tryCatch({
          cache <<- nextElem(it)
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
    
    cache <- list("filename"=filename, "password"=password, "info"=info )
    
    obj <- list(nextElem=nextEl)
    attr( obj, "cache" ) <- cache
#    class(obj) <- c( 'MEFcont', 'abstractiter', 'iter' )
    ihn_obj <- ihasNext( iterators::iter(obj) )
    class(ihn_obj) <- c( 'MEFcont', 'ihasNext', 'abstractiter', 'iter' )
    ihn_obj
}
