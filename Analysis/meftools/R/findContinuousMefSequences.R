findContinuousMefSequences <- function( info, time0=0, time1=.Machine$double.xmax ) {
  # Divide the continuous regions. Starts and Stops are inclusive.
  microsecPerSample <- 1E6/info$header$sampling_frequency
  contiguousStarts <- which( info$discontinuities == 1 )
  contiguousStops <- c(contiguousStarts-1, ncol(info$ToC) )
  contiguousStops <- contiguousStops[-1]
  contiguousStartTimes <- info$ToC[1,contiguousStarts]
  dsamp <- diff(info$ToC[3,])
  contiguousStopTimes <- info$ToC[1,contiguousStops] + dsamp[contiguousStops]*microsecPerSample
  
  # Restrict based on time-window requests
  keepa <- union( which( contiguousStartTimes>=time0 & contiguousStartTimes<=time1), which( contiguousStopTimes>=time0 & contiguousStopTimes<=time1) )
  keepb <- which( contiguousStartTimes<=time0 & contiguousStopTimes>=time1 )
  keep <- sort( union( keepa, keepb ) )
  
  # This one line should do what the commented section does.
  conts <- data.frame( contiguousStarts=contiguousStarts[keep], contiguousStops=contiguousStops[keep] )
  return( conts )
}