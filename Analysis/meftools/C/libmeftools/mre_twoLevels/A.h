#ifndef A_H
#define A_H

#include "B.h"

class A {
private:
	B b;
public:
        A( B b );

        ~A();

	int status() {
		return b.status();
	}
};

#endif
