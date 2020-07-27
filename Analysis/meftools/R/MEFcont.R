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

    source('~/Dropbox/Documents/Concepts/2018_07_27_meftools/Analysis/meftools/R/findContinuousMefSequences.R')

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
    print( bufferSize )
    
    # Use only meftools files everywhere

    source('~/Dropbox/Documents/Concepts/2018_07_27_meftools/Analysis/meftools/R/MEFiter.R')
    source('~/Dropbox/Documents/Concepts/2018_07_27_meftools/Analysis/meftools/R/mef_info.R')
#    info <- mef_info( c(filename,password) )
    
    i <- 1
    # Divide the continuous regions. Starts and Stops are inclusive.
    conts <- findContinuousMefSequences( info )
    it <- iter( conts, by="row" )
    
    # Use blocks
    nextEl <- function() {
      values <- nextElem( it )
#      print( paste0( colnames(values) ) )
      #print( paste0( filename ) )
#      print( values )
#      print( time0 )
#      print( time1 )
      
#      MEFiter( filename, password, info=info, block0=values$contiguousStarts, block1=values$contiguousStops, time0=time0, time1=time1 )
      MEFiter( filename=filename, password=password, info=info, time0=time0, time1=time1 )
    }
    
    cache <- list("filename"=filename, "password"=password, "info"=info )
    
    obj <- list(nextElem=nextEl)
    attr( obj, "cache" ) <- cache
    class(obj) <- c('MEFcont', 'abstractiter', 'iter')
    ihn_obj <- ihasNext( obj )
}
