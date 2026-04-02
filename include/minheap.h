#ifndef MINHEAP_H
#define MINHEAP_H

/**
 * In order to adequately path for A* search, we will
 * need a minheap to efficiently handle pathing from the lowest cost
 * to our destination.
 *
 * But, in order to allow for re-use, this is also designed generically.
 *
 * 99.9% of the time, you are going to want to interact with the functions provided,
 * rather than the struct directly.
 *
 * If you think you know better than the algorithm, you probably don't. There's a test suite for a reason.
 */
struct MinHeap {
    /** The heap array. */
    void **arr;
    /** The in-use length of the min-heap. */
    int len;
    /** The capacity of the min-heap. */
    int capacity;
    /** Functon to get the measured quantity from the stored data.
     * The returned measure is what the min-heap organizes on. */
    int (*get_measure)(const void *);
    /** Function pointer to clean up the elements.
     * Can do nothing, which allows for us to store primitive data types,
     * stack-allocated data, or data that is still in use. */
    void (*element_cleanup)(void *);
};

MinHeap *minheap_init(int amt, int (*measure_func)(const void *), void (*cleanup_func)(void *));
void minheap_init_static(MinHeap *heap, void **arr, int amt, int (*measure_func)(const void *));
int minheap_insert(MinHeap *heap, void *ob);
void *minheap_remove(MinHeap *heap);
void minheap_free(MinHeap *to_free);

#endif
