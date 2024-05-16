#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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
 * Finds the minimum element of a mergeable heap. For a sorted heap, it returns the head's key. Complexity time O(1).
 * For an unsorted heap, it scans through the list to find the minimum. Complexity time O(n).
 */
int minimum(mergeable_heap *heap, input_type input_t) {
    if (input_t == SORTED) {
        // For sorted heaps, the minimum element is the head of the list.
        return heap->head->key;
    } else {
        // For unsorted heaps, iterate through the list to find the minimum element.
        list_t *current = heap->head;
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
}

mergeable_heap *union_heaps(mergeable_heap *heapA, mergeable_heap *heapB, input_type type) {
    if (!heapA) return heapB;
    if (!heapB) return heapA;

    mergeable_heap *mergedHeap = malloc(sizeof(mergeable_heap));

    if (type == SORTED) {
        // Merging two sorted lists into one sorted list in linear complexity time
        list_t tempNode;  // Temporary node to simplify merging. The merged list will start after this node
        list_t *current = &tempNode;
        tempNode.next = NULL;

        list_t *a = heapA->head;
        list_t *b = heapB->head;

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

        current->next = a ? a : b;  // attach the remainder
        mergedHeap->head = tempNode.next;
        // If there's any remainder, the tail must be updated to the tail of the remaining part
        if (current->next) {
            mergedHeap->tail = (a) ? heapA->tail : heapB->tail;
        } else {
            mergedHeap->tail = current;
        }
    } else {
        // Concatenating two unsorted lists in constant complexity time
        if (heapA->tail) {
            heapA->tail->next = heapB->head;
        } else {
            heapA->head = heapB->head;  // If heapA is empty
        }
        mergedHeap->head = heapA->head;
        mergedHeap->tail = (heapB->tail) ? heapB->tail : heapA->tail;
    }

    free(heapA); // since we replace `heapA` with `mergedHeap`

    return mergedHeap;
}


/**
 * Inserts a node with the specified key into the sorted linked list of a mergeable_heap.
 * Updates both the head and the tail of the heap, ensuring proper order is maintained.
 */
void insert_to_sorted_heap(mergeable_heap *heap, int key) {
    if (!heap) return;
    list_t *newNode = malloc(sizeof(list_t));
    newNode->key = key;
    newNode->next = NULL;

    // In case the key should get inserted to the root of the heap
    if (!heap->head || key < heap->head->key) {
        newNode->next = heap->head;
        heap->head = newNode;
        if (!heap->head->next) {  // If the list was empty, update the tail to the new node
            heap->tail = newNode;
        }
        return;
    }

    // Scanning the list until finding an element which is larger than key
    list_t *current = heap->head;
    while (current->next && current->next->key < key) {
        current = current->next;
    }

    // We take all elements that are greater than key, and put them in newNode after its root, which is key
    // Then we take newNode and put it after all elements of current that are smaller than key
    newNode->next = current->next;
    current->next = newNode;
    if (!newNode->next) {  // If new node is now the last node
        heap->tail = newNode;
    }
}

/**
 * Inserts a new node with the specified key into the heap. This operation prepends the node
 * to the linked list that represents the heap, updating the heap's head, and its tail (in case of a new list).
 * Relevant for unsorted heaps. Complexity time O(1).
 */
void prepend(mergeable_heap *heap, int key) {
    list_t *newNode = malloc(sizeof(list_t));

    newNode->key = key;
    newNode->next = heap->head;

    if (heap->head == NULL) {
        heap->tail = newNode;
    }
    heap->head = newNode;
}

void insert(mergeable_heap *heap, int key, input_type inputType) {
    if (inputType == SORTED) {
        insert_to_sorted_heap(heap, key);
    } else {  // For UNSORTED inputType
        prepend(heap, key);
    }
}

/**
 * Deletes all nodes with the specified key from the list and updates the tail if necessary.
 * Relevant for unsorted heaps. Complexity time O(n).
 * The method returns the possibly new head of the list.
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
        list_t *oldHead = heap->head; // Temporarily store the node to be freed
        heap->head = heap->head->next;
        if (!heap->head) heap->tail = NULL;
        free(oldHead); // We can now free the old head after its deletion
    } else {
        heap->head = delete_key(heap->head, min, &heap->tail);;
    }
    return min;
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

void sort(mergeable_heap *heap) {
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

// Function to read numbers from a file and populate a heap
void populate_heap_from_file(const char *filename, mergeable_heap *heap, input_type inputType) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    } else {
        printf("Successfully loaded the file %s.\n", filename);
    }

    int value;
    // Assuming the file contains numbers separated by spaces or newlines
    while (fscanf(file, "%d", &value) == 1) {
        insert(heap, value, inputType);
    }

    fclose(file);
}

// Helper function to print list
void print_list(list_t *node) {
    while (node != NULL) {
        printf("%d -> ", node->key);
        node = node->next;
    }
    printf("NULL\n");
}

int main() {
    mergeable_heap *a = make_heap();
    mergeable_heap *b = make_heap();
    char selectedChar;

    printf("\nHow would you like to implement the mergeable heap? Enter 'E' to exit\n");
    printf("  1) Using sorted linked lists.\n");
    printf("  2) Using unsorted linked lists.\n");
    scanf(" %c", &selectedChar);

    if (selectedChar == 'E') exit(0);
    while (selectedChar != '1' && selectedChar != '2') {
        printf("Invalid input. Try again.\n");
        scanf(" %c", &selectedChar);
        if (selectedChar == 'E') exit(0);
    }

    input_type inputType = (selectedChar == '1') ? SORTED : UNSORTED;
    populate_heap_from_file("../heap_a.txt", a, inputType);
    populate_heap_from_file("../heap_b.txt", b, inputType);
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
                    insert(a, key, inputType);
                } else {
                    prepend(a, key);
                }
                printf("Key inserted to A.\n");
                break;
            case '3':  // Insert to B
                printf("Enter key to insert to B: ");
                scanf("%d", &key);
                if (inputType == SORTED) {
                    insert(b, key, inputType);
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
                a = union_heaps(a, b, inputType);
                print_list(a->head);
                b = make_heap();
                printf("Heaps unified. Second heap is now empty.\n");
                break;
            case '7':  // Sort A and B
                printf("Current A: ");
                print_list(a->head);
                printf("Current B: ");
                print_list(b->head);
                if (inputType == UNSORTED) {
                    sort(a);
                    printf("Sorted A: ");
                    print_list(a->head);
                    sort(b);
                    printf("Sorted B: ");
                    print_list(b->head);
                    printf("Heaps sorted.\n");
                } else {
                    printf("Heaps are already in sorted mode.\n");
                }
                break;
            case '8':  // Print Lists
                printf("Current lists A and B: \n");
                printf("A: ");
                print_list(a->head);
                printf("B: ");
                print_list(b->head);
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