#include <stdlib.h>
#include <stdarg.h>

/////////////////////////////////////////////////////////////////////////////
// List operations
/////////////////////////////////////////////////////////////////////////////

struct list_t {
    int key;
    struct list_t *next;
    struct list_t *prev;
};

enum input_type {
    SORTED,
    UNSORTED
};

typedef enum input_type input_type;
typedef struct list_t list_t;

typedef struct {
    list_t *head;
    list_t *tail;
} mergeable_heap;

mergeable_heap *make_heap() {
    mergeable_heap *heap = malloc(sizeof(mergeable_heap));
    heap->head = NULL;
    heap->tail = NULL;
    return heap;
}

list_t *delete_key(list_t *list, int key) {
    list_t *match;
    while (list && list->key == key) {
        match = list;
        list = list->next;
        free(match);
    }

    if (!list) {
        return NULL;
    }

    list_t *node = list;
    while (node->next) {
        if (node->next->key == key) {
            match = node->next;
            node->next = match->next;
            free(match);
        } else {
            node = node->next;
        }
    }

    return list;
}

list_t *prepend(list_t *list, int key) {
    list_t *new = malloc(sizeof(list_t));

    new->key = key;
    new->next = list;

    return new;
}

int find_min(list_t *list) {
    int min = list->key;
    list = list->next;

    while (list) {
        if (list->key < min) {
            min = list->key;
        }
        list = list->next;
    }

    return min;
}

list_t *link_together(list_t *a, list_t *b) {
    list_t *result;

    if (!a) {
        result = b;
    } else {
        result = a;
        while (a->next) {
            a = a->next;
        }
        a->next = b;
    }

    return result;
}

list_t *merge_sorted(list_t *a, list_t *b) {
    list_t merged_h;
    // TODO: rename
    list_t *new = &merged_h;

    while (a && b) {
        if (a->key < b->key) {
            new->next = a;
            a = a->next;
        } else {
            new->next = b;
            b = b->next;
        }
        new = new->next;
    }

    if (a) {
        new->next = a;
    } else {
        new->next = b;
    }

    return merged_h.next;
}

/////////////////////////////////////////////////////////////////////////////
// 1. Mergreable heaps with sorted list
/////////////////////////////////////////////////////////////////////////////

/**
 *
 * @param list
 * @param key
 * @return
 */
list_t *insert_sorted(list_t *list, int key) {
    list_t *new_node = malloc(sizeof(list_t));
    new_node->key = key;
    new_node->next = NULL;
    new_node->prev = NULL;

    // In case the key should get inserted to the root of the heap
    if (!list || key < list->key) {
        new_node->next = list;
        return new_node;
    }

    // Scanning the list until finding an instance of an element which is strictly larger than key
    list_t *current = list;
    while (current->next && current->next->key < key) {
        current = current->next;
    }

    // We take all elements from current that are greater than key, and put them in new_node after its root - key
    // Then we take new_node and put it after all elements of current that are smaller than key
    new_node->next = current->next;
    if  (current->next) {
        current->next->prev = new_node;
    }
    new_node->prev = current;
    current->next = new_node;

    // After modified the addresses correctly, return the original list's pointer
    return list;
}

void insert1(mergeable_heap *heap, int key) {
    heap->head = insert_sorted(heap->head, key);
}

int minimum(mergeable_heap *heap, input_type input_t) {
    if (input_t == SORTED) {
        return heap->head->key;
    } else if (input_t == UNSORTED) {
        return find_min(heap->head);
    }
}

/**
 * To extract the minimum element we'll do the following:
 * 1. CalL minimum()
 * 2. # TODO:
 * @param heap
 * @return
 */
int extract_min1(mergeable_heap *heap) {
    list_t *old_head = heap->head;
    int min = minimum(heap, SORTED);
    heap->head = heap->head->next;
    free(old_head); // We can now free head after its deletion
    return min;
}

//int extract_min2(mergeable_heap *heap) {
//    int min = minimum(heap, UNSORTED);
//    heap->head = delete_key(heap->head, min);
//    return min;
//}


mergeable_heap *union1(mergeable_heap *heap_a, mergeable_heap *heap_b) {
    mergeable_heap *merged_heap = malloc(sizeof(mergeable_heap));
    merged_heap->head = merge_sorted(heap_a->head, heap_b->head);

    free(heap_a);
    free(heap_b);

    return merged_heap;
}

/////////////////////////////////////////////////////////////////////////////
// 2. Mergreable heaps with unsorted lists
/////////////////////////////////////////////////////////////////////////////


void insert2(mergeable_heap *heap, int key) {
    heap->head = prepend(heap->head, key);
}

int extract_min2(mergeable_heap *heap) {
    int min = minimum(heap, UNSORTED);
    heap->head = delete_key(heap->head, min);
    return min;
}

mergeable_heap *union2(mergeable_heap *ha, mergeable_heap *hb) {
    mergeable_heap *result = make_heap();
    result->head = link_together(ha->head, hb->head);

    free(ha);
    free(hb);

    return result;
}

void destroy_heap(mergeable_heap *heap) {
    free(heap);
}

////


void test_sorted_heap(mergeable_heap *heap) {
    insert1(heap, 3);
    insert1(heap, 1);
    insert1(heap, 4);
    printf("Minimum: %d\n", minimum(heap, SORTED));
    printf("Extract Min: %d\n", extract_min1(heap));
    printf("New Minimum after extraction: %d\n", minimum(heap, SORTED));

    mergeable_heap *heapA = make_heap();
    insert1(heapA, 2);
    insert1(heapA, 5);
    mergeable_heap *mergedHeapA = union1(heap, heapA);
    printf("Minimum of merged heap: %d\n", minimum(mergedHeapA, SORTED));
    printf("Extract Min of merged heap: %d\n", extract_min1(mergedHeapA));
//    Should result the text of: Testing Sorted List Heap
//    Minimum: 1
//    Extract Min: 1
//    New Minimum after extraction: 3
//    Minimum of merged heap: 2
//    Extract Min of merged heap: 2
}

void test_unsorted_heap(mergeable_heap *heap) {
    printf("Testing Unsorted List Heap\n");
    mergeable_heap *heapB = make_heap();
    insert2(heapB, 3);
    insert2(heapB, 1);
    insert2(heapB, 4);
    printf("Minimum: %d\n", minimum(heapB, UNSORTED));
    printf("Extract Min: %d\n", extract_min2(heapB));
    printf("New Minimum after extraction: %d\n", minimum(heapB, UNSORTED));

    mergeable_heap *heapC = make_heap();
    insert2(heapC, 2);
    insert2(heapC, 5);
    mergeable_heap *mergedHeapB = union2(heapB, heapC);
    printf("Minimum of merged heap: %d\n", minimum(mergedHeapB, UNSORTED));
    printf("Extract Min of merged heap: %d\n", extract_min2(mergedHeapB));
}

int main() {
    printf("Testing Sorted List Heap\n");
    mergeable_heap *heap = make_heap();
    mergeable_heap *unsorted_heap = make_heap();

    test_sorted_heap(heap);
    test_unsorted_heap(unsorted_heap);
    // Once done, make sure to free the allocated heap
    destroy_heap(heap);
    return 0;
}