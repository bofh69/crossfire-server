/*
 * This is the unit tests file for common/minheap.c
 */

#include <stdint.h>

#include "minheap.h"
#include "compat.h"
#include "define.h"

#include <check.h>
#include <stdlib.h>
#include <string.h>

static MinHeap *test_heap;

static void setup(void) {
    cctk_setlog(LOGDIR "/unit/common/minheap.out");
}

static void teardown(void) {
    // Clean up the minheap after every test.
    if (test_heap != NULL)
        minheap_free(test_heap);
    test_heap = NULL;
}

// Define functions that will be needed in order to retrieve values from the heap.
int int_measure(void *ob) {
    return *((int *)ob);
}

// We will also need this in order to qsort the results to ensure it works.
// This will tell qsort to sort in ascending order.
int qsort_comp_int(const void *a, const void *b) {
    return *((int *)a) - *((int *)b);
}

// Ensure that allocation works.
START_TEST(test_minheap_allocate) {
    test_heap = minheap_init(1, int_measure, NULL);
    fail_unless(test_heap != NULL, "Failed to allocate minheap of size 1.");
    fail_unless(test_heap->len == 0, "Initialized empty heap has nonzero length.");
    fail_unless(test_heap->capacity == 1, "Initialized heap has capacity of %d, not 1.", test_heap->capacity);
}
END_TEST

START_TEST(test_minheap_overfill) {
    test_heap = minheap_init(1, int_measure, NULL);
    int vals[2] = {4, 3};
    int result;
    result = minheap_insert(test_heap, &vals[0]);
    fail_unless(result == 0, "Minheap should have allowed 1 element, but allowed 0.");
    result = minheap_insert(test_heap, &vals[1]);
    fail_unless(result == -1, "Minheap of size 1 allowed 2 elements to be added.");
}
END_TEST

START_TEST(test_minheap_empty_remove) {
    test_heap = minheap_init(1, int_measure, NULL);
    int *result = minheap_remove(test_heap);
    fail_unless(result == NULL, "Empty heap should not return a non-null value on remove.");
    int vals[3] = {1, 2, 3};
    minheap_insert(test_heap, &vals[0]);
    result = minheap_remove(test_heap);
    fail_unless(result == &vals[0], "Heap should have returned the only value it was given (%d)", vals[0]);
    result = minheap_remove(test_heap);
    fail_unless(result == NULL, "Heap should be empty, yet we returned a value on removal (len=%d)", test_heap->len);
    minheap_insert(test_heap, &vals[1]);
    int insresult = minheap_insert(test_heap, &vals[2]);
    fail_unless(insresult == -1, "Heap should have been full, but insert was allowed (len=%d)", test_heap->len);
}
END_TEST

START_TEST(test_minheap_insert_remove) {
    int vals[30] = {4, 6, 23, 7, 343, 12, 1, 1, 33, 76, 4, 34, 8, 7, 90, 123, 2, 3, 55, 65, 4, 6, 77, 65, 5, 7, 6, 5, 4, 1, 2};
    int expected[30], actual[30];
    // Copy the values to the expected array and then qsort them.
    memcpy(expected, vals, sizeof(int)*30);
    qsort(expected, 30, sizeof(int), qsort_comp_int);

    // Initialize the minheap.
    test_heap = minheap_init(30, int_measure, NULL);
    for (int i = 0; i < 30; ++i) {
        int result = minheap_insert(test_heap, &vals[i]);
        fail_unless(result == 0, "Failed to insert to the heap for index %d (%d).", i, vals[i]);
    }
    // Now we pull off the heap, which incidentally sorts the values.
    for (int i = 0; i < 30; ++i) {
        int *result = minheap_remove(test_heap);
        fail_unless(result != NULL, "Failed to remove from the heap after %d elements.", i);
        actual[i] = *result;
        fail_unless(expected[i] == actual[i], "Heap did not gather data correctly. (%d != %d)", expected[i], actual[i]);
    }
}
END_TEST

// Now let's do a test that does a struct.
typedef struct {
    int x;
    int y;
} Pos;

int measure_pos_distance(void *ob) {
    const Pos *p = ob;
    const int diag = MIN(FABS(p->x), FABS(p->y));
    const int rem = MAX(FABS(p->x), FABS(p->y)) - diag;
    return diag * 3 + rem * 2;
}

int qsort_pos_check(const void *a, const void *b) {
    return measure_pos_distance(a) - measure_pos_distance(b);
}

START_TEST(test_minheap_with_struct) {
    test_heap = minheap_init(400, measure_pos_distance, NULL);
    Pos vals[400], expected[400], actual[400];
    srand(0);
    // Initialize
    for (int i = 0; i < 400; ++i) {
        vals[i].x = rand() & 16535;
        vals[i].y = rand() & 16535;
    }
    // Copy these over to expected, since we will qsort it there.
    memcpy(expected, vals, 400 * sizeof(Pos));
    qsort(expected, 400, sizeof(Pos), qsort_pos_check);

    // And then we will use the heap for sorting into actual
    for (int i = 0; i < 400; ++i) {
        int res = minheap_insert(test_heap, &vals[i]);
        fail_unless(res == 0, "Could not insert into minheap at size %d.", test_heap->len);
    }
    // Then remove them from the heap.
    for (int i = 0; i < 400; ++i) {
        Pos *res = minheap_remove(test_heap);
        fail_unless(res != NULL, "Minheap emptied before it should have: %d items should be left.", 400-i);
        actual[i] = *res;
        // Since we sorted on the distance from (0,0), any ties could be in different orders.
        fail_unless(measure_pos_distance(&expected[i]) == measure_pos_distance(&actual[i]),
            "Minheap retrieved wrong value. Expected (%d, %d): %d, got (%d, %d): %d", expected[i].x, expected[i].y,
            measure_pos_distance(&expected[i]), actual[i].x, actual[i].y, measure_pos_distance(&actual[i]));
    }
}
END_TEST

START_TEST(test_minheap_static_alloc) {
#define HEAP_SIZE 50
    MinHeap heap;
    int *heaparr[HEAP_SIZE];
    minheap_init_static(&heap, heaparr, HEAP_SIZE, int_measure);

    // Now we get the data ready.
    int vals[HEAP_SIZE], expected[HEAP_SIZE];
    srand(5);
    for (int i = 0; i < HEAP_SIZE; ++i) {
        vals[i] = rand() & 32767;
    }
    // Copy to expected so we can qsort it.
    memcpy(expected, vals, HEAP_SIZE * sizeof(int));
    qsort(expected, HEAP_SIZE, sizeof(int), qsort_comp_int);

    // Push items onto the heap.
    for (int i = 0; i < HEAP_SIZE; ++i) {
        int res = minheap_insert(&heap, &vals[i]);
        fail_unless(res == 0, "Could not insert into minheap at size %d.", heap.len);
    }

    // Pull back off the heap
    for (int i = 0; i < HEAP_SIZE; ++i) {
        int *ref = minheap_remove(&heap);
        fail_unless(ref != NULL, "Minheap emptied before it should have: %d items should be left.", HEAP_SIZE - i);
        fail_unless(*ref == expected[i], "Minheap retrieved wrong value. Expected %d, got %d.", expected[i], *ref);
    }
}
END_TEST

static Suite *minheap_suite(void) {
    Suite *s = suite_create("minheap");
    TCase *tc_core = tcase_create("Core");
    tcase_set_timeout(tc_core, 60);

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_minheap_allocate);
    tcase_add_test(tc_core, test_minheap_overfill);
    tcase_add_test(tc_core, test_minheap_empty_remove);
    tcase_add_test(tc_core, test_minheap_insert_remove);
    tcase_add_test(tc_core, test_minheap_with_struct);
    tcase_add_test(tc_core, test_minheap_static_alloc);

    return s;
}

int main(void) {
    int nf;
    Suite *s = minheap_suite();
    SRunner *sr = srunner_create(s);

    /* to debug, uncomment this line */
    /*srunner_set_fork_status(sr, CK_NOFORK);*/

    srunner_set_xml(sr, LOGDIR "/unit/common/minheap.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/minheap.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
