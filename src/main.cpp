#include "./tests/memory_test.h"
#include "./tests/vector_test.h"
#include "./tests/list_test.h"

using namespace ttl::ttl_test;

// write all test code
int main() {
    list_test::runAll();
    vector_test::runAll();
    memory_test::runAll();
    return 0;
}