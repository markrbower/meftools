setwd("/Users/markrbower/Dropbox/Documents/Concepts/2020_06_01_IntroductionToComputation")
ncs_filename <- here( "Data/CSC33.ncs" )
mef_filename <- here( "mef2/CSC33.mef" )
if ( file.exists(mef_filename) ) {
  file.remove( mef_filename )
}
ncs2mef( c(ncs_filename) )

library( topsecret )
vault <- topsecret::get_secret_vault()
info <- mef_info( c( mef_filename,secret::get_secret("MEF_password",key = secret::local_key(), vault = vault) ) )
iter_cont <- MEFcont( filename=mef_filename, password=secret::get_secret("MEF_password", key = secret::local_key(), vault = vault) ,time0=5377964401,time1=6735527300)
iter_cont$hasNext()
iter_data <- iter_cont$nextElem()
iter_data$hasNext()
tetrode_data <- iter_data$nextElem()
iter_data$hasNext()
tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_1 <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_2 <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_3 <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_4 <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_5 <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_6 <- iter_data$nextElem()
iter_data$hasNext();tetrode_data_7 <- iter_data$nextElem()

ttd <- c(tetrode_data_1,tetrode_data_2,tetrode_data_3,tetrode_data_4,tetrode_data_5,tetrode_data_6,tetrode_data_7)

bf_low <- butter(2, c(1/16000,1000/16000), type="pass")
ttd_low <- filtfilt( bf_low, ttd )
# plot( ttd_low, type='l') # RPubs "stimulated_seizure"

Tsample <- 1/info$header$sampling_frequency
TstimON  <- attr(tetrode_data_1,'t0') + 55160*Tsample*1E6     # 45 sec into rec
TstimOFF <- attr(tetrode_data_1,'t0') + 118750*Tsample*1E6    # ~ 2 sec durxn
TseizureON  <- attr(tetrode_data_1,'t0') + 149000*Tsample*1E6  # 46.56 sec
TseizureOFF <- attr(tetrode_data_1,'t0') + 700000*Tsample*1E6  # 17.2 sec durxn
# roughly 21.5 min of data are recorded after the seizure




