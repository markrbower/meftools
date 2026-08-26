#include "A.h"
#include "B.h"

A::A( B tmp_b ) : b(tmp_b) {
	if ( tmp_b.getInitialized()==0 ) {
		b = B( 1 );
	} else {
		b = tmp_b;
	}
};


A::~A() {};

