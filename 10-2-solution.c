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

typedef struct list_t list_t;

typedef struct {
    list_t *head;
} mergeable_heap;

mergeable_heap *make_heap() {
    mergeable_heap *result = malloc(sizeof(mergeable_heap));
    result->head = NULL;
    return result;
}

list_t *insert_sorted(list_t *list, int key) {
    list_t *new = malloc(sizeof(list_t));
    new->key = key;

    if (!list || key < list->key) {
        new->next = list;
        return new;
    }

    list_t *l = list;
    while (l->next && l->next->key < key) {
        l = l->next;
    }

    new->next = l->next;
    l->next = new;

    return list;
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
    list_t dummy;
    list_t *new = &dummy;

    while (a && b) {
        if (a->key < b->key) {
            new->next = a;
            a = a->next;
            new = new->next;
        } else {
            new->next = b;
            b = b->next;
            new = new->next;
        }
    }

    if (a) {
        new->next = a;
    } else {
        new->next = b;
    }

    return dummy.next;
}

/////////////////////////////////////////////////////////////////////////////
// 1. Mergreable heaps with sorted list
/////////////////////////////////////////////////////////////////////////////

void insert1(mergeable_heap *heap, int key) {
    heap->head = insert_sorted(heap->head, key);
}

int sorted_minimum(mergeable_heap *heap) {
    return heap->head->key;
}

int extract_min1(mergeable_heap *heap) {
    list_t *head = heap->head;
    int result = head->key;
    heap->head = head->next;
    free(head);
    return result;
}

mergeable_heap *union1(mergeable_heap *ha, mergeable_heap *hb) {
    mergeable_heap *result = malloc(sizeof(mergeable_heap));
    result->head = merge_sorted(ha->head, hb->head);

    free(ha);
    free(hb);

    return result;
}

/////////////////////////////////////////////////////////////////////////////
// 2. Mergreable heaps with unsorted lists
/////////////////////////////////////////////////////////////////////////////


void insert2(mergeable_heap *heap, int key) {
    heap->head = prepend(heap->head, key);
}

int unsorted_minimum(mergeable_heap *heap) {
    return find_min(heap->head);
}

int extract_min2(mergeable_heap *heap) {
    int min = unsorted_minimum(heap);
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
    printf("Minimum: %d\n", sorted_minimum(heap));
    printf("Extract Min: %d\n", extract_min1(heap));
    printf("New Minimum after extraction: %d\n", sorted_minimum(heap));

    mergeable_heap *heapA = make_heap();
    insert1(heapA, 2);
    insert1(heapA, 5);
    mergeable_heap *mergedHeapA = union1(heap, heapA);
    printf("Minimum of merged heap: %d\n", sorted_minimum(mergedHeapA));
    printf("Extract Min of merged heap: %d\n", extract_min1(mergedHeapA));
}

//void test_unsorted_heap() {
//    printf("Testing Unsorted List Heap\n");
//    mergeable_heap *heapB = make_heap();
//    insert2(heapB, 3);
//    insert2(heapB, 1);
//    insert2(heapB, 4);
//    printf("Minimum: %d\n", unsorted_minimum(heapB));
//    printf("Extract Min: %d\n", extract_min2(heapB));
//    printf("New Minimum after extraction: %d\n", unsorted_minimum(heapB));
//
//    mergeable_heap *heapC = make_heap();
//    insert2(heapC, 2);
//    insert2(heapC, 5);
//    mergeable_heap *mergedHeapB = union2(heapB, heapC);
//    printf("Minimum of merged heap: %d\n", unsorted_minimum(mergedHeapB));
//    printf("Extract Min of merged heap: %d\n", extract_min2(mergedHeapB));
//}

int main() {
    printf("Testing Sorted List Heap\n");
    mergeable_heap *heap = make_heap();

    test_sorted_heap(heap);
//    test_unsorted_heap();
    // Once done, make sure to free the allocated heap
    destroy_heap(heap);
    return 0;
}