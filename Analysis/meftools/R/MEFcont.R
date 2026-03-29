MEFcont <- function( filename, password, ... ) {
  #' Creates an iterator of contiguous MEF sequences.
  #' 
  #'     timeConstraints <- NPI:::checkTimeConstraints( compArgs_caseSpecific$get('info'), case )
  #.     'iterCont <- meftools::MEFcont( filename, 'erlichda', compArgs_caseSpecific$get('bufferSize'), window=timeConstraints, info=compArgs_caseSpecific$get('info') )
  #' }
  library( iterators )
  library( itertools )
  
    time0 <- 0
    time1 <- 1E50
    args <- list(...)
    info <- NULL
    for ( arg in names(args) ) {
      switch( arg,
              "time0" = {time0 = args[[arg]]},
              "time1" = {time1 = args[[arg]]},
              "window" = {tmp <-  args[[arg]]; time0 <- tmp['start']; time1 <- tmp['stop']},
              "bufferSize" = {bufferSize <- args[[arg]]},
              "info" = {info <- args[[arg]]}
      )
    }

    # Use only meftools files everywhere
    if ( is.null(info) ) {
      info <- mef_info( c(filename,password) )
    }
    
    i <- 1
    # Divide the continuous regions. Starts and Stops are inclusive.
    conts <- findContinuousMefSequences( info, time0, time1 )
    it <- itertools::ihasNext( iterators::iter( conts, by="row" ) )
    
    # Use blocks
    nextEl <- function() {
      values <- iterators::nextElem( it )
      mi <- MEFiter( filename, password, info=info, block0=values$contiguousStarts, block1=values$contiguousStops, time0=time0, time1=time1, stepSize=3600 )
      return( mi )
    }
    
    hasNx <- function() {
      value <- it$hasNext()
      print(value)
      return( value )      
    }
    
    props <- list("filename"=filename, "password"=password, "info"=info )
    obj <- list(nextElem=nextEl,hasNext=hasNx)
    attr( obj, "props" ) <- props
    class(obj) <- c('MEFcont')
    obj    
}
