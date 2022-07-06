#include "./tests/memory_test.h"
#include "./tests/vector_test.h"
#include "./tests/list_test.h"
#include "./tests/hashtable_test.h"
#include "./tests/unordered_map_test.h"
#include "./tests/priority_queue_test.h"
#include "./tests/bitset_test.h"
#include "./tests/deque_test.h"
#include "./tests/bs_tree_test.h"
#include "./tests/segment_tree_test.h"

using namespace ttl::ttl_test;

// write all test code
int main() {
    segment_tree_test::runAll();
    // if (time(nullptr)) return 0;
    bs_tree_test::runAll();
    deque_test::runAll();
    bitset_test::runAll();
    priority_queue_test::runAll();
    unordered_map_test::runAll();
    hashtable_test::runAll();
    list_test::runAll();
    vector_test::runAll();
    memory_test::runAll();
    return 0;
}