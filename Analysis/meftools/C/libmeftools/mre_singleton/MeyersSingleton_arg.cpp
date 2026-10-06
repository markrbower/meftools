#include <iostream>
using namespace std;

class Singleton {

private:

    Singleton( int v ) {
	value = v;
    }

    int value;
    static int i_;
    static bool initialized_;

public:
    static void Init(int i) {
        i_ = i;
        initialized_ = true;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
	if ( !initialized_ ) {
		throw invalid_argument( "Error" );
	}
        static Singleton instance(i_);
        return instance;
    }

    void display()
    {
        cout << "Singleton Instance: " << value << endl;
    }
};

int Singleton::i_ = 0;
bool Singleton::initialized_ = false;

int main() {
    Singleton::Init( 2 );
    Singleton& s1 = Singleton::getInstance();

    Singleton::Init( 5 );
    Singleton& s2 = Singleton::getInstance();

    cout << "These should be the same, showing the second assignment was ignored." << endl;
    s1.display();
    s2.display();

    cout << "These should be the same, showing that a second request returned the first instance." << endl;
    cout << "Address of s1: " << &s1 << endl;
    cout << "Address of s2: " << &s2 << endl;

    return 0;
}

