
test_that("read_mef_header works", {
  filename <- file.path( here::here(), "../..", "Data", "CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  header = read_mef_header(c(filename,
                             secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  expect_equal( header$encryption_algorithm, "128-bit AES")
})

test_that("TableOfContents works", {
  filename <- file.path( here::here(), "../..", "Data", "CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  header = read_mef_header(c(filename,
                             secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  variables <- c(filename,header$index_data_offset,header$number_of_index_entries)
  ToC <- table_of_contents( variables )
  expect_equal( length(ToC[1,]), 10)
  expect_equal( length(ToC[,1]), 3)
})

test_that("discontinuities works", {
  filename <- file.path( here::here(), "../..", "Data", "CSC1.mef", fsep = .Platform$file.sep)
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
  filename <- file.path( here::here(), "../..", "Data", "CSC1.mef", fsep = .Platform$file.sep)
  vault <- topsecret::get_secret_vault()
  info = mef_info(c(filename,secret::get_secret(name="MEF_password",key=secret::local_key(),vault)))
  expect_equal( info$header$encryption_algorithm, "128-bit AES")
  expect_equal( length(info$ToC[1,]), 10)
  expect_equal( info$discontinuities[1], 1)
})



