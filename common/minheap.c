/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2021 The Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file common/minheap.c
 * Implements a minheap.
 *
 * Initial use is for the A* pathing algorithm in monster AI, but
 * is designed to be generic enough that anything that needs a minheap can use it.
 */

#include "minheap.h"
#include "global.h"

#include <stdlib.h>

/**
 * Get the index of the left child in the heap.
 * left child is index * 2 + 1.
 *
 * @return
 * the index of the left child of the provided index.
 */
static inline int minheap_get_left(int index) {
    // We can assert the last digit is 0 after a left shift,
    // so we can do a bitwise-or instead of addition to save time.
    return (index << 1) | 1;
}

/**
 * Get the index of the right child in the heap.
 * right child is index * 2 + 2.
 *
 * @return
 * the index of the right child of the provided index.
 */
static inline int minheap_get_right(int index) {
    return (index << 1) + 2;
}

/**
 * Get the index of the parent in the heap.
 * parent is (index - 1) / 2 (rounded down).
 *
 * @return
 * the index of the parent of the provided index, or -1 if provided index
 * was the root of the tree.
 */
static inline int minheap_get_parent(int index) {
    return index ? (index - 1) >> 1 : -1;
}

/**
 * Normalize the minheap.
 * It is expcted that the value at arr[0]
 * is the only element violating the minheap.
 *
 * That element will be pushed down the tree to
 * its proper place.
 */
static void minheap_normalize(MinHeap *heap) {
    int at = 0;
    // The measurements of the three items being compared.
    // [0] -- parent
    // [1] -- left child
    // [2] -- right child
    int val[3];
    // Handle some special cases for when the heap is small.
    if (heap->len == 1)
        return;
    while (at < heap->len) {
        int left = minheap_get_left(at),
            right = minheap_get_right(at),
            minchild;
        // Break out of the loop if we hit the bottom of the heap.
        if (left >= heap->len)
            break;
        val[0] = heap->get_measure(heap->arr[at]);
        val[1] = heap->get_measure(heap->arr[left]);
        // Check to see if there's the right child as well.
        if (right < heap->len) {
            // If there is the ight child, determine which child is the smaller of the two.
            val[2] = heap->get_measure(heap->arr[right]);
            if (val[1] < val[2])
                minchild = left;
            else
                minchild = right;
        }
        // If no right child, the left will be the smaller one.
        else
            minchild = left;
        // If the smaller child is smaller than the value we are percolating, swap them.
        if (val[0] <= (minchild == left ? val[1] : val[2]))
            break;
        void *tmp = heap->arr[at];
        heap->arr[at] = heap->arr[minchild];
        heap->arr[minchild] = tmp;
        // Prepare for next iteration
        at = minchild;
    }
}

/**
 * Function to intialize the minheap
 *
 * @param amt
 * The capacity of the min-heap.
 *
 * @param measure_func
 * Pointer to the function to measure the data we're storing.
 *
 * @param cleanup_func
 * Pointer to a given element's cleanup function.
 * Can be NULL if the data does not need to be cleaned up for any reason.
 *
 * @return
 * Pointer to the allocated min-heap. Errors out if allocation fails.
 */
MinHeap *minheap_init(int amt, int (*measure_func)(const void *), void (*cleanup_func)(void *)){
    MinHeap *newheap = malloc(sizeof(MinHeap));
    if (!newheap)
        fatal(OUT_OF_MEMORY);
    newheap->arr = malloc(amt * sizeof(void *));
    if (!newheap->arr)
        fatal(OUT_OF_MEMORY);
    newheap->len = 0;
    newheap->capacity = amt;
    newheap->get_measure = measure_func;
    newheap->element_cleanup = cleanup_func;
    return newheap;
}

/**
 * Initialize the minheap using statically allocated components.
 * It is expected the caller will do any necessary cleanup on data before
 * the components fall out of scope.
 *
 * It is also expected that the caller ensured arr can hold amt elements.
 *
 * @param heap
 * Pointer to the heap we are initializing.
 *
 * @param arr
 * Pointer to the array of elements we are using.
 *
 * @param amt
 * The capacity of the heap
 *
 * @param measure_func
 * Pointer to the function that gives the calculation the minheap uses to organize elements.
 */
void minheap_init_static(MinHeap *heap, void **arr, int amt, int (*measure_func)(const void *)) {
    if (heap == NULL)
        return;
    heap->arr = arr;
    heap->len = 0;
    heap->capacity = amt;
    heap->get_measure = measure_func;
    heap->element_cleanup = NULL;
}

/**
 * Inserts an element into the min-heap
 *
 * @param ob
 * The data object being inserted.
 *
 * @return
 * 0 if successful, -1 if the heap is already full.
 */
int minheap_insert(MinHeap *heap, void *ob) {
    if (heap->len == heap->capacity)
        return -1;
    // To insert, we pull down elements deeper into the heap,
    // and then put the new element at the top, and percolate it down.
    int at = heap->len++;
    while (at) {
        int parent = minheap_get_parent(at);
        heap->arr[at] = heap->arr[parent];
        at = parent;
    }
    heap->arr[0] = ob;
    minheap_normalize(heap);
    return 0;
}

/**
 * Pops the top of the minheap off.
 *
 * @return
 * The popped element, or NULL if the heap is empty.
 */
void *minheap_remove(MinHeap *heap) {
    if (heap->len == 0)
        return NULL;
    void *removed = heap->arr[0];
    // Insert the last element of the heap back into the top,
    // and percolate down. The previous spot it existed is no longer part of the heap.
    heap->arr[0] = heap->arr[--heap->len];
    minheap_normalize(heap);
    return removed;
}

/**
 * Cleans the minheap
 *
 * @param to_free
 * The minheap to free.
 */
void minheap_free(MinHeap *to_free){
    // If the data elements have a cleanup method provided,
    // then clean them up. Otherwise just deallocate our references.
    if (to_free->element_cleanup) {
        for (int i = 0; i < to_free->len; ++i)
            to_free->element_cleanup(to_free->arr[i]);
    }
    free(to_free->arr);
    free(to_free);
}
