SQLiter <- function( db, query ) {
  #' Return an iterator of database records.
  #' 
  #' @param db MySQL database connection:
  #' @param query String: "select ..." - it is assumed that this will always be a "select" query.
  #
  i <- 1
    
  resultset <- DBI::dbGetQuery( db, query )

  it <- iterators::iter( resultset, by="row" )

  nextEl <- function() {
    n <- iterators::nextElem(it)
  }
  
  obj <- list(nextElem=nextEl)
  class(obj) <- c('SQLiter', 'abstractiter', 'iter')
  returnable <- itertools::ihasNext(obj)
  attr( returnable, "size" ) <- nrow( resultset )
  return( returnable )
}
