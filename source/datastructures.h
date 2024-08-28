#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include "types.h"

typedef struct SinglyLinkedListNode {
    struct SinglyLinkedListNode *next;
    uint8_t data[];
} SinglyLinkedListNode;

typedef struct SinglyLinkedList {
    size_t type_size;
    SinglyLinkedListNode *head;
} SinglyLinkedList;

SinglyLinkedListNode* SinglyLinkedListNode_Create(size_t size);
SinglyLinkedList* SinglyLinkedList_Create(size_t type_size);
void SinglyLinkedList_Append(SinglyLinkedList* list, const void* data);
void SinglyLinkedList_Free(SinglyLinkedList* list);

#endif //DATASTRUCTURES_H
