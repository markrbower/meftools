mefBuffer <- function( power ) {
  #
  # Create the buffer
  #
  bufferSizePower <- 21
  bufferSize <- 2^bufferSizePower
  buffer <- vector( mode='double', length=bufferSize )

  fill <- function( data ) {
    buffer <- data
  }
  
  filter <- function() {
    
  }
  
  get() <- function() {
    return buffer
  }
}

