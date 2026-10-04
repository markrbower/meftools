#include <iostream>
#include <stdexcept>

using namespace std;

class S {
public:
    static void Init(int i)
    {
        i_ = i;
        initialized_ = true;
    }

    static S& getInstance()
    {
        if (!initialized_) {
            throw invalid_argument("not initialized.");
        }
        static S instance(i_);
        return instance;
    }

    void display() {
	cout << i_ << endl;
    }

private:
    S(int) { }

    static int i_;
    static bool initialized_;
};

int S::i_ = 0;
bool S::initialized_ = false;

int main() {
	S::Init( 1 );

	S s = S::getInstance();

	s.display();

	return 0;
}

