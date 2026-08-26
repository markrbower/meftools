#ifndef B_H
#define B_H

class B {
private:
	int initialized=0;
	int b;
public:
	B();

	B( int x );

	~B();

	int getInitialized() { return initialized; }

	int status() { return b; }
};

#endif
