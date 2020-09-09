
test_that("read_mef_header works", {
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  header = read_mef_header(c(filename,
                             secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  expect_equal( header$encryption_algorithm, "128-bit AES")
})

test_that("TableOfContents works", {
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  header = read_mef_header(c(filename,
                secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  variables <- c(filename,header$index_data_offset,header$number_of_index_entries)
  ToC <- table_of_contents( variables )
  expect_equal( length(ToC[1,]), 10)
  expect_equal( length(ToC[,1]), 3)
})

test_that("discontinuities works", {
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  header = read_mef_header(c(filename,
                secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  variables <- c(filename,header$index_data_offset,header$number_of_index_entries)
  ToC <- table_of_contents( variables )
  discontinuities <- get_discontinuities( c(filename,header$number_of_index_entries), ToC );
  expect_equal( length(discontinuities), 10)
  expect_equal( discontinuities[1], 1)
})

test_that("mef_info works", {
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  info = mef_info(c(filename,secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  expect_equal( info$header$encryption_algorithm, "128-bit AES")
  expect_equal( length(info$ToC[1,]), 10)
  expect_equal( info$discontinuities[1], 1)
})

test_that("ncs2mef works", {
  filename <- file.path( testthat::test_path(), "../Data/CSC1.ncs", fsep = .Platform$file.sep)
#  print( paste0( "ncs2mef: filename: ", filename ) )
  newFilename <- file.path( testthat::test_path(), "mef2/CSC1.mef", fsep = .Platform$file.sep)
#  print( paste0( "ncs2mef: newFilename: ", newFilename ) )
  #  # Convert  
  meftools::ncs2mef( c(filename) )
#  # Check the output
  info <- meftools::mef_info( c(newFilename, "blah"))
  expect_equal( info$header$encryption_algorithm, "128-bit AES")
  file.remove( newFilename )
})

test_that("decomp_mef works", {
  vault = topsecret::get_secret_vault()
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  data <- meftools::decomp_mef( c(filename,1,32000,topsecret::get("MEF_password") ) )
  expect_equal( data[100], 239 )
})

test_that("MEFcont works", {
  vault = topsecret::get_secret_vault()
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  mc <- meftools::MEFcont( filename, topsecret::get("MEF_password"))
  expect_true( "MEFcont" %in% class(mc) )
  mc$hasNext()
  mi <- mc$nextElem()
  expect_true( "MEFiter" %in% class(mi) )
})

test_that("MEFiter works", {
  vault = topsecret::get_secret_vault()
  filename <- file.path( testthat::test_path(), "../Data/CSC1.mef", fsep = .Platform$file.sep)
  mc <- meftools::MEFcont( filename, topsecret::get("MEF_password"))
  mc$hasNext()
  mi <- mc$nextElem()
  mi$hasNext()
  data <- mi$nextElem()
  expect_true( !is.null(data) )
  expect_equal( length(data), 128000 )
  expect_equal( data[100], 355 )
})


