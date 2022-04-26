#include "./utils/profiler.h"
#include <iostream>

using namespace std;
using namespace tstd;

// write all test code
int main() {
    {
        auto_timer timer;
        cout << "hello world" << endl;
    }
    return 0;
}