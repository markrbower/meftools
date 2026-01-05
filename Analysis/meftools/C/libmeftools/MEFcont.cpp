// MEFcont.cpp
// Mark R. Bower
// Yale University
//
  #' Creates an iterator of contiguous MEF sequences.
  #' 
  #' @param filename String: The complete path to a .mef file
  #' @param password String: The public password for the MEF file.
  #' @param block0 Integer: Start block number.
  #' @param block1 Integer: Stop block number.
  #' @param time0 Integer: Start time value (in microseconds)
  #' @param time1 Integer: Stop time value (in microseconds)
  #' @param step Integer: Time step (in seconds) for each iteration.
  #' @return A data iterator for MEF files.˙0
  #' @export
  #' @examples
  #' \dontrun{
  #'   data_iter <- MEFiter( filename, password, time0=1.00E15, time1=1.01E15, step=10 )
  #'   data <- nextElem( data_iter )
  #' }
#include <string>
#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"

using namespace std;

class MEFcont( char* filename, char* password, int bufferSize, timeConstraints, info ) {
private:
  int time0, time1, window, bufferSize;
  MEFinfo info;
  MEFiter conts;

public:
  MEFcont( string filename, string password, int block0, int block1, int time0, int time1, int step ) {
    if ( info == NULL ) {
      info = MEFinfo( filename, password );
    }
    # Divide the continuous regions. Starts and Stops are inclusive.
    conts = info.findContinuousMefSequences( time0, time1 );
    it <- itertools::ihasNext( iterators::iter( conts, by="row" ) );
    


  }

}

