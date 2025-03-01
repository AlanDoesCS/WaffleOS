#include "datastructures.h"
#include <stddef.h>
#include "../core/memory.h"

SinglyLinkedListNode* SinglyLinkedListNode_Create(size_t size) {
    SinglyLinkedListNode* node = (SinglyLinkedListNode*) malloc(sizeof(SinglyLinkedListNode) + size);
    if (node) {
        node->next = NULL;
    }
    return node;
}

SinglyLinkedList* SinglyLinkedList_Create(size_t type_size) {
    SinglyLinkedList* list = (SinglyLinkedList*) malloc(sizeof(SinglyLinkedList));
    if (list) {
        list->type_size = type_size;
        list->head = NULL;
    }
    return list;
}

void SinglyLinkedList_Append(SinglyLinkedList* list, const void* data) {
    if (!list || !data) return;

    SinglyLinkedListNode* new_node = SinglyLinkedListNode_Create(list->type_size);
    if (!new_node) return;

    memcpy(new_node->data, data, list->type_size);

    if (!list->head) {
        list->head = new_node;
    } else {
        SinglyLinkedListNode* current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// WARNING: If you store pointers, remember to cast+dereference
void* SinglyLinkedList_Get(SinglyLinkedList* list, size_t index) {
    if (!list) return NULL;

    SinglyLinkedListNode* current = list->head;
    size_t i = 0;

    while (current && i < index) {
        current = current->next;
        i++;
    }

    if (!current) {
        // out of range
        return NULL;
    }

    return current->data;
}


void SinglyLinkedList_Free(SinglyLinkedList* list) {
    if (!list) return;

    SinglyLinkedListNode* current = list->head;
    while (current) {
        SinglyLinkedListNode* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}