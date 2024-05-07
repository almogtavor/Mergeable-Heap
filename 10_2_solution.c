#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////////////
// List operations
/////////////////////////////////////////////////////////////////////////////

struct list_t {
    int key;
    struct list_t *next;
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

/**
 * Searching for the minimum element of an unsorted heap. Complexity time O(n).
 */
int find_min_in_unsorted_heap(list_t *current) {
    int min = current->key;
    current = current->next;

    while (current) {
        if (current->key < min) {
            min = current->key;
        }
        current = current->next;
    }

    return min;
}

list_t *merge_sorted_lists(list_t *a, list_t *b) {
    list_t merged_heap;
    list_t *current = &merged_heap;

    while (a && b) {
        if (a->key < b->key) {
            current->next = a;
            a = a->next;
        } else {
            current->next = b;
            b = b->next;
        }
        current = current->next;
    }

    if (a) {
        current->next = a;
    } else {
        current->next = b;
    }

    return merged_heap.next;
}

/////////////////////////////////////////////////////////////////////////////
// 1. Mergreable heaps with sorted list
/////////////////////////////////////////////////////////////////////////////

int minimum(mergeable_heap *heap, input_type input_t) {
    if (input_t == SORTED) {
        return heap->head->key;
    } else { // For unsorted cases
        return find_min_in_unsorted_heap(heap->head);
    }
}


mergeable_heap *union1(mergeable_heap *heap_a, mergeable_heap *heap_b) {
    if (!heap_a) return heap_b;
    if (!heap_b) return heap_a;
    mergeable_heap *merged_heap = malloc(sizeof(mergeable_heap));
    merged_heap->head = merge_sorted_lists(heap_a->head, heap_b->head);

    free(heap_a);
    free(heap_b);

    return merged_heap;
}

/////////////////////////////////////////////////////////////////////////////
// 2. Mergreable heaps with unsorted lists
/////////////////////////////////////////////////////////////////////////////


/**
 * We receive the address of tail since we want to be able to update it.
 *
 * @param list
 * @param key
 * @return
 */
list_t *insert_sorted(list_t *list, int key, list_t **tail) {
    list_t *new_node = malloc(sizeof(list_t));
    new_node->key = key;
    new_node->next = NULL;

    // In case the key should get inserted to the root of the heap
    if (!list || key < list->key) {
        new_node->next = list;
        if (!list) {  // If the list was empty, update the tail to the new node
            *tail = new_node;
        }
        return new_node;
    }

    // Scanning the list until finding an instance of an element which is strictly larger than key
    list_t *current = list;
    while (current->next && current->next->key < key) {
        current = current->next;
    }

    if (current->next == NULL) {
        // Insert at the end of the list
        current->next = new_node;
        *tail = new_node;
    } else {
        // We take all elements from current that are greater than key, and put them in new_node after its root - key
        // Then we take new_node and put it after all elements of current that are smaller than key
        new_node->next = current->next;
        current->next = new_node;
    }
    return list;
}

/**
 * Inserts a new node with the specified key into the heap. This operation prepends the node
 * to the linked list that represents the heap, updating the heap's head, and its tail (in case of a new list).
 * Relevant for unsorted heaps. Complexity time O(1).
 *
 * @param heap The heap to insert into.
 * @param key The key of the new node to insert.
 */
void prepend(mergeable_heap *heap, int key) {
    list_t *new_node = malloc(sizeof(list_t));

    new_node->key = key;
    new_node->next = heap->head;

    if (heap->head == NULL) {
        heap->tail = new_node;
    }
    heap->head = new_node;
}

void insert(mergeable_heap *heap, int key, input_type inputType) {
    if (inputType == SORTED) {
        heap->head = insert_sorted(heap->head, key, &heap->tail);
    } else {  // For UNSORTED inputType
        prepend(heap, key);
    }
}

/**
 * Deletes all nodes with the specified key from the list and updates the tail if necessary.
 * Relevant for unsorted heaps. Complexity time O(n).
 *
 * @param list The head of the list from which nodes will be deleted.
 * @param key The key of nodes to delete.
 * @param tail Pointer to the tail of the list.
 * @return The possibly new head of the list.
 */
list_t *delete_key(list_t *list, int key, list_t **tail) {
    list_t *current = list, *prev = NULL;

    while (current) {
        if (current->key == key) {
            list_t *toDelete = current;
            if (*tail == current) {  // Update tail if tail is being deleted
                *tail = prev;
            }
            if (prev != NULL) {
                prev->next = current->next;  // Skip the node being deleted
            } else {
                list = current->next;  // Update head if the head node is being deleted
            }
            current = current->next;
            free(toDelete);
        } else {
            prev = current;
            current = current->next;
        }
    }

    // If all nodes were deleted, set tail to NULL
    if (prev == NULL) *tail = NULL;

    return list;
}

/**
 * To extract the minimum element we'll do the following:
 * To extract the minimum element from the heap, the function:
 * 1. Calls minimum() to find the minimum value.
 * 2. Removes the head of the heap if it's sorted, as the minimum is at the head.
 * 3. For unsorted heaps, finds and deletes the minimum element from anywhere in the heap.
 * 4. Frees the memory allocated for the node that contained the minimum element.
 */
int extract_min(mergeable_heap *heap, input_type inputType) {
    if (!heap || !heap->head) {
        fprintf(stderr, "Heap is empty or does not exist.\n");
        return -1;
    }
    int min = minimum(heap, inputType);
    if (inputType == SORTED) {
        list_t *old_head = heap->head; // Temporarily store the node to be freed
        heap->head = heap->head->next;
        if (!heap->head) heap->tail = NULL;
        free(old_head); // We can now free the old head after its deletion
    } else {
        heap->head = delete_key(heap->head, min, &heap->tail);;
    }
    return min;
}

/**
 * Merges two heaps into one in constant time, for unsorted heaps cases.
 */
mergeable_heap *unsorted_union(mergeable_heap *heap_a, mergeable_heap *heap_b) {
    mergeable_heap *merged_heap = make_heap();
    if (!heap_a->head) {
        free(heap_a);
        return heap_b;
    }
    if (!heap_b->head) {
        free(heap_b);
        return heap_a;
    }
    // Connect the tail of heap_a to the head of heap_b
    heap_a->tail->next = heap_b->head;

    // Set the head of the new merged heap to the head of heap_a & tail to tail of heap_b
    merged_heap->head = heap_a->head;
    merged_heap->tail = heap_b->tail;

    free(heap_a);
    free(heap_b);
    return merged_heap;
}


/** Function to split the nodes of the given list into half
 * `lead` advances twice as fast as `trail`, so when 'lead' will get to the
 * end of the list, `trail` would be at the half of it.
 */
list_t *split(list_t *head) {
    list_t *lead = head, *trail = head;
    list_t *trailPrev = NULL;  // Keeps track of the node before `trail`
    while (lead && lead->next) {
        trailPrev = trail;
        trail = trail->next;
        lead = lead->next->next;
    }
    if (trailPrev) trailPrev->next = NULL;  // Split the list into two halves
    return trail;  // `trail` is the start of the second half
}

// Function to merge two sorted lists and update the tail
list_t *merge(list_t *a, list_t *b, list_t **tail) {
    list_t mergedResult;
    list_t *current = &mergedResult;  // Temporary head node to make operations easier
    mergedResult.next = NULL;
    while (a && b) {
        if (a->key < b->key) {
            current->next = a;
            a = a->next;
        } else {
            current->next = b;
            b = b->next;
        }
        current = current->next;
    }
    current->next = a ? a : b;  // Attach the remaining part
    // Move to the actual last node to update the tail
    while (current->next) {
        current = current->next;
    }
    *tail = current;  // Update the tail
    return mergedResult.next;
}

/** Recursive merge sort function that also updates the tail after sorting is complete.
 * The complexity time of the algorithm is O(nlog(n)) at the worst case.
 */
list_t *merge_sort(list_t *head, list_t **tail) {
    if (!head || !head->next) {
        *tail = head;  // In case the list is empty or has one element
        return head;
    }
    list_t *secondHalf = split(head);
    head = merge_sort(head, tail);  // Sort the first half
    secondHalf = merge_sort(secondHalf, tail);  // Sort the second half
    return merge(head, secondHalf, tail);  // Merge and update tail
}

void sort_mergeable_heap(mergeable_heap *heap) {
    if (!heap) return;
    heap->head = merge_sort(heap->head, &heap->tail);
}

void destroy_heap(mergeable_heap *heap) {
    list_t *current = heap->head;
    while (current != NULL) {
        list_t *next = current->next;
        free(current);
        current = next;
    }
    free(heap);
}

////

// Helper function to print list
void printList(list_t *node) {
    while (node != NULL) {
        printf("%d -> ", node->key);
        node = node->next;
    }
    printf("NULL\n");
}

void handle_sigsegv(int sig) {
    printf("----------------Caught segmentation fault!---------------\n");
    printf("--------------------------ERROR!-------------------------\n");
    exit(EXIT_FAILURE);
}

int main() {
    signal(SIGSEGV, handle_sigsegv);
    mergeable_heap *a = make_heap();
    mergeable_heap *b = make_heap();
    char selectedChar;


    printf("\nHow would you like to implement the mergeable heap? Enter 'E' to exit\n");
    printf("  1) Using sorted linked lists.\n");
    printf("  2) Using unsorted linked lists.\n");
    scanf(" %c", &selectedChar);

    // TODO: decide how to let them retry invalid input
    if (selectedChar == 'E' || (selectedChar != '1' && selectedChar != '2')) exit(0);

    input_type inputType = (selectedChar == '1') ? SORTED : UNSORTED;
    while (true) {
        printf("Please choose an operation from the menu: \n");
        printf("  1) Make Heap \n");
        printf("  2) Insert to A \n");
        printf("  3) Insert to B \n");
        printf("  4) Extract Min from A \n");
        printf("  5) Extract Min from B \n");
        printf("  6) Union A and B \n");
        printf("  7) Sort A and B \n");
        printf("  8) Print Lists \n");
        printf("Press E to EXIT \n");
        scanf(" %c", &selectedChar);

        if (selectedChar == 'E') break;

        int key;
        switch (selectedChar) {
            case '1':  // Make Heap
                destroy_heap(a);
                destroy_heap(b);
                a = make_heap();
                b = make_heap();
                printf("New heaps created.\n");
                break;
            case '2':  // Insert to A
                printf("Enter key to insert to A: ");
                scanf("%d", &key);
                if (inputType == SORTED) {
                    insert1(a, key);
                } else {
                    prepend(a, key);
                }
                printf("Key inserted to A.\n");
                break;
            case '3':  // Insert to B
                printf("Enter key to insert to B: ");
                scanf("%d", &key);
                if (inputType == SORTED) {
                    insert1(b, key);
                } else {
                    prepend(b, key);
                }
                printf("Key inserted to B.\n");
                break;
            case '4':  // Print Min of A
                printf("Extracted the minimum of A: %d\n", extract_min(a, inputType));
                break;
            case '5':  // Print Min of B
                printf("Extracted the minimum of B: %d\n", extract_min(b, inputType));
                break;
            case '6':  // Union
                if (inputType == SORTED) {
                    a = union1(a, b);
                } else {
                    a = unsorted_union(a, b);
                }
                printList(a->head);
                b = make_heap();
                printf("Heaps unified. Second heap is now empty.\n");
                break;
            case '7':  // Sort A and B
                if (inputType == UNSORTED) {
                    printf("Current A: ");
                    printList(a->head);
                    printf("Current B: ");
                    printList(b->head);
                    sort_mergeable_heap(a);
                    printf("Sorted A: ");
                    printList(a->head);
                    sort_mergeable_heap(b);
                    printf("Sorted B: ");
                    printList(b->head);
                    printf("Heaps sorted.\n");
                } else {
                    printf("Heaps are already in sorted mode.\n");
                }
                break;
            case '8':  // Print Lists
                printf("Current lists A and B: \n");
                printf("A: ");
                printList(a->head);
                printf("B: ");
                printList(b->head);
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
    }

    destroy_heap(a);
    destroy_heap(b);
    return 0;
}