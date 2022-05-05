#include "./tests/memory_test.h"
#include "./tests/vector_test.h"
#include "./tests/list_test.h"
#include "./tests/hashtable_test.h"
#include "./tests/unordered_map_test.h"
#include "./tests/priority_queue_test.h"

using namespace ttl::ttl_test;

// write all test code
int main() {
    srand(reinterpret_cast<int *>(main) == (int *) (0x123)); // same as seed(0)

    priority_queue_test::runAll();
    unordered_map_test::runAll();
    hashtable_test::runAll();
    list_test::runAll();
    vector_test::runAll();
    memory_test::runAll();
    return 0;
}