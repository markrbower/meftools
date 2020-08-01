MEFiter <- function(filename, password, ... ) {
  #' Create a data iterator for MEF files
  #' 
  #' @param filename String: The complete path to a .mef file
  #' @param password String: The public password for the MEF file.
  #' @param block0 Integer: Start block number.
  #' @param block1 Integer: Stop block number.
  #' @param time0 Integer: Start time value (in microseconds)
  #' @param time1 Integer: Stop time value (in microseconds)
  #' @param stepSize Integer: Timestep size (in seconds) for each iteration.
  #' @return A data iterator for MEF files.
  #' @export
  #' @examples
  #' \dontrun{
  #'   data_iter <- MEFiter( filename, password, time0=1.00E15, time1=1.01E15, stepSize=10 )
  #'   data <- nextElem( data_iter )
  #' }

  args <- list(...)
  for ( arg in names(args) ) {
    switch( arg,
            "block0" = {block0 = args[[arg]];
            #            print( paste0( "block0 is ", block0 ) )
            },
            "block1" = {block1 = args[[arg]];
            #            print( paste0( "block1 is ", block1 ) )
            },
            "time0" = {time0 = args[[arg]];
            #            print( paste0( "time0 is ", time0 ) )
            },
            "time1" = {time1 = args[[arg]];
            #            print( paste0( "time0 is ", time0 ) )
            },
            "info" = {info = args[[arg]]#;
            print( paste0( info$ToC[1,1] ) )
            },
            "stepSize" = {stepSize = args[[arg]];}
    )
  }
  
  if ( !exists( "info" ) ) {
    print( 'MEFiter was not initialized with an info variable' )
    stop(0)
    #    info <- mef_info( c(filename,password) )
  }
  if ( !exists( "block0" ) | is.null(block0) ) {
    block0 <- 1
  }
  if ( !exists( "block1" ) | is.null(block1) ) {
    block1 <- length( info$discontinuities )
  }
  if ( !exists( "time0" ) | is.null( time0 ) ) {
    time0 <- 0
  }
  if ( !exists( "time1" ) | is.null( time1 ) ) {
    time1 <- 1E20
  }
  if ( exists( "stepSize" ) ) {
    sampleSize <- round( info$header$block_interval * stepSize / 1E6 )
  } else {
    sampleSize <- ceiling( 1E5/info$header$maximum_block_length )
  }
  microsecondsPerSample <- 1E6 / info$header$sampling_frequency
  
  # This seems messy and confusing with the addition of time ....
  i <- 1
  #  print( paste0( block0, ':', block1 ) )
  #  print( paste0( block0, ' ', block1, ' ', sampleSize ) )
  
  # If sampleSize exists, use it. If not, use sampleSize.
  if ( (block1-block0) > sampleSize ) {
    #    print( paste0( "multi-block read" ) )
    S <- seq( block0, block1 )
    it <- iterators::idiv( length(S), chunkSize=sampleSize );
    newstops <- cumsum( unlist(as.list(it)) )
    stop <- block0 + newstops - 1
    NN <- length(newstops)
    start <- c( block0, block0+newstops )
    start <- start[-length(start)]
  } else {
    #    print( paste0( "single-block read" ) )
    start <- block0
    stop <- block1
  }
  
  # start/stop can be more than one-dimensional.
  if ( length(start) != length(stop) ) {
    print("ERROR: start and stop must be the same length")
    return(NULL)
  }
  df <- data.frame( start, stop )
  
  it <- iterators::iter( df, by="row" )
  
  nextEl <- function() {
    # This is where decomp_mef returns data.
    #    print( paste0( time0, ' ', time1 ) )
    n <- iterators::nextElem(it)
    #    print( paste0( n$start, ' ', n$stop, ' ', ncol(info$ToC) ) )
    block0 <- n$start
    if ( n$stop > length(info$ToC[3,]) ) {
      block1 <- ncol( info$ToC[1,] )
    } else {
      block1 <- n$stop
    }
    s0 <- info$ToC[3,block0]
    if ( block1==info$header$number_of_index_entries ) {
      s1 <- info$header$number_of_samples
    } else {
      s1 <- info$ToC[3,(block1+1)]-1
    }
    dlast <- s1 - info$ToC[3,block1] + 1
    #    print( paste0( s0, ' ', s1 ) )
    data <- decomp_mef(c(filename, s0, s1, password) )
    # Check the time window.
    blockTime <- c( info$ToC[1,block0],  info$ToC[1,block1] + round(dlast*1E6/info$header$sampling_frequency) )
    #    print( paste0( dlast, ' ', blockTime[1], ' ', blockTime[2] ) )
    if ( blockTime[1]<=time0 & time0<=blockTime[2] ) { # requested start is within decoded data
      bad <- ceiling( (time0 - blockTime[1]) / microsecondsPerSample )
      data <- data[-1:-bad]
    }
    if ( blockTime[1]<=time1 & time1<=blockTime[2] ) { # requested stop is within decoded data
      bad <- ceiling( (blockTime[2] - time1) / microsecondsPerSample )
      LL <- length(data)
      #      print( paste0( bad, ' ', LL, ' ', -(LL-bad+1) ) )
      data <- data[-(LL-bad+1):-LL]
    }
    # Add the timestamp of the first value as an attribute.
    attr( data, 's0' ) <- info$ToC[3,n$start]
    attr( data, 't0' ) <- info$ToC[1,n$start]
    # Add the timestamp of the last value as an attribute.
    attr( data, 's1' ) <- info$ToC[3,n$stop] + dlast
    attr( data, 't1' ) <- info$ToC[1,n$stop] + dlast * microsecondsPerSample;
    # Add metadata as attributes.
    attr( data, 'info' ) <- info
    attr( data, 'dt' ) <- microsecondsPerSample
    return(data)
  }
  
  ihasNext <- function(it) {
    if (!is.null(it$hasNext)) return(it)
    cache <- NULL
    has_next <- NA
    
    nextEl <- function() {
      if (!hasNx())
        stop('StopIteration', call.=FALSE)
      has_next <<- NA
      
      # This is where decomp_mef returns data.
      #    print( paste0( n$start, ' ', n$stop, ' ', ncol(info$ToC) ) )
      block0 <- cache$start
      if ( cache$stop > length(info$ToC[3,]) ) {
        block1 <- ncol( info$ToC[1,] )
      } else {
        block1 <- cache$stop
      }
      s0 <- info$ToC[3,block0]
      if ( block1==info$header$number_of_index_entries ) {
        s1 <- info$header$number_of_samples
      } else {
        s1 <- info$ToC[3,(block1+1)]-1
      }
      dlast <- s1 - info$ToC[3,block1] + 1
      #    print( paste0( s0, ' ', s1 ) )
      data <- decomp_mef(c(filename, s0, s1, password) )
      # Check the time window.
      blockTime <- c( info$ToC[1,block0],  info$ToC[1,block1] + round(dlast*1E6/info$header$sampling_frequency) )
      #    print( paste0( dlast, ' ', blockTime[1], ' ', blockTime[2] ) )
      if ( blockTime[1]<=time0 & time0<=blockTime[2] ) { # requested start is within decoded data
        bad <- ceiling( (time0 - blockTime[1]) / microsecondsPerSample )
        data <- data[-1:-bad]
      }
      if ( blockTime[1]<=time1 & time1<=blockTime[2] ) { # requested stop is within decoded data
        bad <- ceiling( (blockTime[2] - time1) / microsecondsPerSample )
        LL <- length(data)
        #      print( paste0( bad, ' ', LL, ' ', -(LL-bad+1) ) )
        data <- data[-(LL-bad+1):-LL]
      }
      # Add the timestamp of the first value as an attribute.
      attr( data, 's0' ) <- info$ToC[3,cache$start]
      attr( data, 't0' ) <- info$ToC[1,cache$start]
      # Add the timestamp of the last value as an attribute.
      attr( data, 's1' ) <- info$ToC[3,cache$stop] + dlast
      attr( data, 't1' ) <- info$ToC[1,cache$stop] + dlast * microsecondsPerSample;
      # Add metadata as attributes.
      attr( data, 'info' ) <- info
      attr( data, 'dt' ) <- microsecondsPerSample
      return(data)
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

  props <- list("filename"=filename, "password"=password, "info"=info )
  
  obj <- list(nextElem=nextEl,hasNext=ihasNext)
  attr( obj, "props" ) <- props
  obj <- ihasNext(it)
  class(obj) <- c('ihasNext', 'abstractiter', 'iter', 'MEFiter')
  obj    
}
