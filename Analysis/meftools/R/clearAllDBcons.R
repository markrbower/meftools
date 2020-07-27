clearAllDBcons <- function() {

  all_cons <- DBI::dbListConnections( RMySQL::MySQL() )
  for ( con in all_cons )
    RMySQL::dbDisconnect(con)
}
