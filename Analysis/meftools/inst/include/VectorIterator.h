#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

// Iterator interface
template <typename T>
class Iterator {
public:
    virtual T next() = 0;
    virtual bool hasNext() = 0;
};

