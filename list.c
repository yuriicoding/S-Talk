#include "list.h"
#include <stdio.h>
#include <assert.h> 

static List heads[LIST_MAX_NUM_HEADS];
static Node nodes[LIST_MAX_NUM_NODES];
static int headsStack[LIST_MAX_NUM_HEADS];
static int nodesStack[LIST_MAX_NUM_NODES];
static int nodeIndex = -1;
static int headIndex = -1;
static bool areStacksPopulated = false;

void populateStacks()
{
    for (int i = LIST_MAX_NUM_HEADS-1; i >= 0; i--)
    {
        headIndex++;
        headsStack[headIndex] = i;
    }
    for (int i = LIST_MAX_NUM_NODES-1; i >= 0; i--)
    {
        nodeIndex++;
        nodesStack[nodeIndex] = i;
    }
    areStacksPopulated = true;
}



List* List_create()
{
    if(!areStacksPopulated)
        populateStacks();
    if (headIndex == - 1)
        return NULL;
    //head
    int index = headsStack[headIndex];
    headIndex--;
    List* newList = &heads[index];
    newList->head = NULL;
    newList->current = NULL;
    newList->tail = NULL;
    newList->length = 0;

    return newList;
}
// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList)
{
    assert(pList != NULL);
    if(pList->length == 0)
        return NULL;
    void* item = pList->tail->item;
    int indexToPush = pList->tail - nodes;
    nodeIndex++;
    nodesStack[nodeIndex] = indexToPush;
    
    pList->tail = pList->tail->prev;
    if(pList->tail != pList->head)
    {
        pList->tail->next = NULL;
        pList->current = pList->tail;
    }
    else
    {
        pList->head = NULL;
        pList->head = NULL;
        pList->current = NULL;
    }

    pList->length--;
    // pList->tail->next = NULL;
    // pList->current->item = pList->tail->item;

    return item;
}


// Returns the number of items in pList.
int List_count(List* pList)
{
    assert(pList != NULL);
    return pList->length;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList)
{
    assert(pList != NULL);
    if(pList->length == 0)
        pList->current = NULL;
    else 
        pList->current = pList->head;
    return pList->head->item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList)
{
    assert(pList != NULL);
    if(pList->length == 0)
    {
        pList->current = NULL;
        return NULL;
    }   
    else 
        pList->current = pList->tail;
    return pList->current->item;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList)
{
    assert(pList != NULL);
    if(pList->length == 0)
        return NULL;
    if((pList->current == NULL && pList->OOB == LIST_OOB_END) || pList->current==pList->tail)
    {
        pList->OOB = LIST_OOB_END;
        pList->current = NULL;
        return NULL;
    }
    if(pList->current == NULL && pList->OOB == LIST_OOB_START)
    {
        pList->current = pList->head;
        return pList->current->item;
    }
    pList->current = pList->current->next;
    return pList->current->item;
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList)
{    
    assert(pList != NULL);
    if(pList->length == 0)
        return NULL;
    if((pList->current == NULL && pList->OOB == LIST_OOB_START) || pList->current==pList->head)
    {
        pList->OOB = LIST_OOB_START;
        pList->current = NULL;
        return NULL;
    }
    if(pList->current == NULL && pList->OOB == LIST_OOB_END)
    {
        pList->current = pList->tail;
        return pList->current->item;
    }
    pList->current = pList->current->prev;
    return pList->current->item;
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList)
{
    assert(pList != NULL);
    if(pList->current == NULL)
        return NULL;
    return pList->current->item;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_after(List* pList, void* pItem)
{
    assert(pList != NULL);
    if(nodeIndex == -1)
        return LIST_FAIL;
    Node* nodeToInsert = &nodes[nodesStack[nodeIndex]];
    nodeToInsert->next = NULL;
    nodeToInsert->prev = NULL;
    nodeToInsert->item = pItem;
    nodeIndex--;
    if(pList->length == 0)
    {
        pList->head = nodeToInsert;
        pList->tail = nodeToInsert;
        nodeToInsert->next = NULL;
        nodeToInsert->prev = NULL;
    }
    else if(pList->current == NULL)
    {
        if(pList->OOB == LIST_OOB_START)
        {
            nodeToInsert->next = pList->head;
            nodeToInsert->prev = NULL;
            pList->head->prev = nodeToInsert;
            pList->head = nodeToInsert;
        }
        else if(pList->OOB == LIST_OOB_END)
        {
            nodeToInsert->next = NULL;
            nodeToInsert->prev = pList->tail;
            pList->tail->next = nodeToInsert;
            pList->tail = nodeToInsert;
        }
    }
    else 
    {
        if(pList->current == pList->tail)
        {
            pList->tail = nodeToInsert;
        }
        nodeToInsert->next = pList->current->next;
        nodeToInsert->prev = pList->current;
        pList->current->next = nodeToInsert;
        if(nodeToInsert->next != NULL)
        {
            nodeToInsert->next->prev = nodeToInsert;
        }
    }

    pList->length++;
    pList->current = nodeToInsert;
    return LIST_SUCCESS;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert_before(List* pList, void* pItem)
{
    assert(pList!=NULL);
    List_prev(pList);
    return List_insert_after(pList,pItem);
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem)
{
    assert(pList!=NULL);
    pList->current = pList->tail;
    return List_insert_after(pList, pItem);
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_prepend(List* pList, void* pItem)
{
    assert(pList!=NULL);
    pList->current = pList->head;
    return List_insert_before(pList, pItem);
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList)
{
    assert(pList!=NULL);
    if(pList->length == 0 || pList->current == NULL)
        return NULL;

    void* item = pList->current->item;

    Node* currentNode = pList->current;

    if(currentNode == pList->tail)
    {
        pList->tail = pList->tail->prev;
        pList->OOB = LIST_OOB_END;
    }
    if(currentNode == pList->head)
    {
        pList->head = pList->head->next;
    }
    if(currentNode->prev != NULL)
    {
        currentNode->prev->next = currentNode->next;
    }
    if(currentNode->next != NULL)
    {
        currentNode->next->prev = currentNode->prev;
    }

    pList->current = currentNode->next;
    int indexToPush = currentNode - nodes;
    nodeIndex++;
    nodesStack[nodeIndex] = indexToPush;
    
    pList->length--;
    return item;
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2)
{
    assert(pList1!=NULL && pList2!=NULL);
    if(pList1->length == 0 && pList2->length != 0)
    {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList2->length = pList2->length;
    }   
    else if(pList1->length != 0 && pList2->length != 0)
    {
        pList1->tail->next = pList2->head;
        pList2->head->prev = pList1->tail;
        pList1->tail = pList2->tail;
        pList1->length += pList2->length;
    }

    int indexToPushList = pList2 - heads;
    headIndex++;
    headsStack[headIndex] = indexToPushList;
    pList2->OOB = LIST_OOB_START;
    pList2->current = NULL;
    pList2->head = NULL;
    pList2->tail = NULL;
    pList2->length = 0;
}



// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn)
{
    assert(pList != NULL);
    Node* currentNode = pList->head;
    while (currentNode != NULL)
    {
        (*pItemFreeFn)(currentNode->item);
        
        int indexToPush = currentNode - nodes;
        nodeIndex++;
        nodesStack[nodeIndex] = indexToPush;

        currentNode = currentNode->next;
    }
    int indexToPushList = pList - heads;
    headIndex++;
    headsStack[headIndex] = indexToPushList;
    
    pList->OOB = LIST_OOB_START;
    pList->current = NULL;
    pList->head = NULL;
    pList->tail = NULL;
    pList->length = 0;
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg)
{    
    assert(pList != NULL);
    if (pList->length == 0)
    {
        return NULL;
    }
    if (pList->current == NULL && pList->OOB == LIST_OOB_END)
        return NULL;
    if (pList->current == NULL && pList->OOB == LIST_OOB_START)
        List_next(pList);
    while (pList->current != NULL)
    {
        if ( (*pComparator)(pList->current->item, pComparisonArg) )
        {
            return pList->current->item;
        }
    

        pList->current = pList->current->next;
    }
    pList->OOB = LIST_OOB_END;
    return NULL;
}
