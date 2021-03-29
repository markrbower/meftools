filename <- file.path( here::here(), "meftools/Data/CSC1.ncs", fsep = .Platform$file.sep)
meftools::ncs2mef( c(filename) )
