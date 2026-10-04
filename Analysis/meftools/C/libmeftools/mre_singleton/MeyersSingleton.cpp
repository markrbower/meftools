#include <iostream>
using namespace std;

class Singleton {

private:

    Singleton() {}

public:

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance()
    {
        static Singleton instance;
        return instance;
    }

    void display()
    {
        cout << "Singleton Instance\n";
    }
};

int main() {
    Singleton& s1 = Singleton::getInstance();
    Singleton& s2 = Singleton::getInstance();

    s1.display();

    cout << "Address of s1: " << &s1 << endl;
    cout << "Address of s2: " << &s2 << endl;

    return 0;
}
