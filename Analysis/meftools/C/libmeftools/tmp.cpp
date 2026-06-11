
vector<MEFconts> MEFinfo::findContinuousMefSequences( vector<long long> timeConstraints ) {
    long long time0 = timeConstraints[0];
    long long time1 = timeConstraints[1];
    vector<long long> result;
    int microsecPerSample = 1E6/header.sampling_frequency;

    int doneFlag = 0;
    vector<MEFconts> conts;
    vector<int> contiguousStarts;
    vector<int> contiguousStops;
    vector<int> dsamp;
    int N = discontinuities.size();
    for ( int i=0; i<(N-1); i++ ) { // the last block cannot contain a start
      dsamp.push_back( ToC[3,(i+1)] - ToC[3,i] );
      if ( ToC[1][i] <= time1 ) {
        if ( ToC[1][i] >= time0 ) {
          if ( discontinuities[i] == 1 ) {
            contiguousStarts.push_back( i );
	    contiguousStops.push_back( i-1 );
          } 
        }
      }
      if ( doneFlag == 0 ) {
        contiguousStops.push_back( i-1 );
        doneFlag = 1;
      }
    }
    contiguousStops.erase( contiguousStops.begin() );
    
    N = contiguousStarts.size();
    for ( int i=0; i<N; i++ ) {
      MEFconts tmp;
      tmp.startTime  = ToC[3][contiguousStarts[i]];
      tmp.timeStep   = round( 1.0 / header.sampling_frequency );
      tmp.startBlock = ToC[1][contiguousStarts[i]];
      tmp.stopBlock  = ToC[1][contiguousStops[i]] + dsamp[contiguousStops[i]]*microsecPerSample;
      conts.push_back( tmp );
    }
    return( conts );
}

