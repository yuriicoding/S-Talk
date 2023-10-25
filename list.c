#include "list.h"
#include "stdio.h"

typedef struct List_m List_manager;
struct List_m {
    List * head;
    List * end;
    List * current;
    int free_heads;
};


static Node nodeArray[LIST_MAX_NUM_NODES];
static List headsArray[LIST_MAX_NUM_HEADS];

static List vacantNodes;
static List_manager vacantHeads;

static bool init_vacant = false;


// Debugging tool used to print the whole list

// static void printList(List * pList){
//     Node * start = pList->head;

//     while (start!=0){
//         printf("%p\n", start);
//         start = start->pointerNext;
//     }
// }


static void initEmptyHeads(){

    vacantHeads.free_heads = 0;
    vacantHeads.head = &(headsArray[0]);
    List * firstAdded = vacantHeads.head;
    firstAdded->prevList = 0;
    firstAdded->nextList = &(headsArray[1]);
    vacantHeads.free_heads = vacantHeads.free_heads + 1;

    for (int i = 1; i <= LIST_MAX_NUM_HEADS - 2; i++){

        List * list_to_add = &(headsArray[i]);
        list_to_add->nextList = &(headsArray[i+1]);
        list_to_add->prevList = &(headsArray[i-1]);
        vacantHeads.free_heads = vacantHeads.free_heads + 1;    

    }

    vacantHeads.end = &(headsArray[LIST_MAX_NUM_HEADS-1]);
    List * lastAdded = vacantHeads.end;
    lastAdded->nextList = 0;
    lastAdded->prevList = &(headsArray[LIST_MAX_NUM_HEADS-2]);
    vacantHeads.free_heads = vacantHeads.free_heads + 1;

}


static void initEmptyNodes(){

    vacantNodes.nodes_num = 0;
    vacantNodes.head = &(nodeArray[0]);
    Node * firstAdded = vacantNodes.head;
    firstAdded->pointerBack = 0;
    firstAdded->pointerNext = &(nodeArray[1]);
    vacantNodes.nodes_num = vacantNodes.nodes_num + 1;

    for (int i = 1; i <= LIST_MAX_NUM_NODES - 2; i++){

        Node * node_to_add = &(nodeArray[i]);
        node_to_add->pointerBack = &(nodeArray[i-1]);
        node_to_add->pointerNext = &(nodeArray[i+1]);
        vacantNodes.nodes_num = vacantNodes.nodes_num + 1;

    }

    vacantNodes.end = &(nodeArray[LIST_MAX_NUM_NODES-1]);
    Node * lastAdded = vacantNodes.end;
    lastAdded->pointerNext = 0;
    lastAdded->pointerBack = &(nodeArray[LIST_MAX_NUM_NODES-2]);
    vacantNodes.nodes_num = vacantNodes.nodes_num + 1;

}


static Node * getVacantNode(){
    if (vacantNodes.nodes_num != 0){

        Node * nodeToAdd = vacantNodes.end;
        vacantNodes.nodes_num = vacantNodes.nodes_num - 1;

        //if it was last vacant node
        if (vacantNodes.nodes_num == 0){
            vacantNodes.head = 0;
            vacantNodes.end = 0;
        }

        //if it wasn't last vacant node
        else{
            Node * prev_empty_node = nodeToAdd->pointerBack;
            vacantNodes.end = prev_empty_node;
            prev_empty_node->pointerNext = 0;
        }

        return nodeToAdd;

    }
    else{
        return 0;
    }
}



static void returnNodeToVacant(Node * pNode){

    Node * nodeToReturn = pNode;
    
    if (vacantNodes.nodes_num == 0){
        vacantNodes.head = nodeToReturn;
        nodeToReturn->pointerBack = 0;
    }
    else{

        Node * lastVacantNode = vacantNodes.end;
        nodeToReturn->pointerBack = lastVacantNode;
        lastVacantNode->pointerNext = nodeToReturn;
    }

    vacantNodes.nodes_num = vacantNodes.nodes_num + 1;
    nodeToReturn->pointerNext = 0;
    vacantNodes.end = nodeToReturn;

};


List* List_create(){

    if (init_vacant == false){
        initEmptyHeads();
        initEmptyNodes();
        init_vacant = true;
    }

    if (vacantHeads.head != 0){

        List * new_list = vacantHeads.end;
        new_list->nodes_num = 0;
        new_list->current = 0;
        new_list->head = 0;
        new_list->end = 0;
        new_list->currentOut = LIST_OOB_START;

        if (vacantHeads.free_heads == 1){
            vacantHeads.head = 0;
            vacantHeads.end = 0;
            vacantHeads.free_heads = vacantHeads.free_heads - 1;
        }
        else {
            List * prev_empty_list = new_list->prevList;
            vacantHeads.end = prev_empty_list;
            prev_empty_list->nextList = 0;
            vacantHeads.free_heads = vacantHeads.free_heads - 1;
        }

        return new_list;
        
    }

    else{
        return 0;
    }
}


int List_count(List* pList){
    return pList->nodes_num;
}


void* List_first(List* pList){
    if (pList->head == 0){
        pList->current = 0;
        pList->currentOut = LIST_OOB_START;
        return 0;
    }
    else{
        pList->current = pList->head;
        pList->currentOut = NO;
        void * item = pList->current->value;
        return item;
    }
}


void* List_last(List* pList){
    if (pList->end == 0){
        pList->current = 0;
        pList->currentOut = LIST_OOB_START;
        return 0;
    }
    else{
        pList->current = pList->end;
        pList->currentOut = NO;  
        void * item = pList->current->value;
        return item;
    }
}


void* List_next(List* pList){
    if (pList->nodes_num != 0){
    if (pList->currentOut == LIST_OOB_START){
        pList->current = pList->head;
        pList->currentOut = NO;
        void * item = pList->current->value;
        return item;
    }
    else if (pList->currentOut == LIST_OOB_END || pList->current == pList->end){
        pList->current = 0;
        pList->currentOut = LIST_OOB_END;
        return 0;
    }
    else{
        pList->current = pList->current->pointerNext;
        void * item = pList->current->value;
        return item;
    }
    }
    else{
        return 0;
    }
};


void* List_prev(List* pList){
    if (pList->nodes_num != 0){
    if (pList->currentOut == LIST_OOB_END){
        pList->current = pList->end;
        pList->currentOut = NO;
        void * item = pList->current->value;
        return item;
    }
    else if (pList->currentOut == LIST_OOB_START || pList->current == pList->head){
        pList->current = 0;
        pList->currentOut = LIST_OOB_START;
        return 0;
    }
    else{
        pList->current = pList->current->pointerBack;
        void * item = pList->current->value;
        return item;
    }
    }
    else{
        return 0;
    }
};


void* List_curr(List* pList){
    if (pList->currentOut == NO){
        void * item = pList->current->value;
        return item;
    }
    else{
        return 0;
    }
};



int List_insert_after(List* pList, void* pItem){
    
    if (vacantNodes.nodes_num != 0){
        if (pList->currentOut == LIST_OOB_END){
            List_append(pList, pItem);
        }
        else if (pList->currentOut == LIST_OOB_START){
            List_prepend(pList, pItem);
        }
        else if (pList->current == pList->end){
            List_append(pList, pItem);
        }
        else{

            Node * nodeToAdd = getVacantNode();
            pList->nodes_num = pList->nodes_num + 1;

            Node * currentNode = pList->current;
            Node * afterCurrent = currentNode->pointerNext;

            currentNode->pointerNext = nodeToAdd;
            nodeToAdd->pointerBack = currentNode;
            afterCurrent->pointerBack = nodeToAdd;
            nodeToAdd->pointerNext = afterCurrent;

            pList->current = nodeToAdd;
            nodeToAdd->value = pItem;

        }

        return 0;

    }
    else{
        return -1;
    }
}



int List_insert_before(List* pList, void* pItem){
    
    if (vacantNodes.nodes_num != 0){
        if (pList->currentOut == LIST_OOB_END){
            List_append(pList, pItem);
        }
        else if (pList->currentOut == LIST_OOB_START){
            List_prepend(pList, pItem);
        }
        else if (pList->current == pList->head){
            List_prepend(pList, pItem);
        }
        else{

            Node * nodeToAdd = getVacantNode();
            pList->nodes_num = pList->nodes_num + 1;

            Node * currentNode = pList->current;
            Node * beforeCurrent = currentNode->pointerBack;

            currentNode->pointerBack = nodeToAdd;
            nodeToAdd->pointerNext = currentNode;
            beforeCurrent->pointerNext = nodeToAdd;
            nodeToAdd->pointerBack = beforeCurrent;

            pList->current = nodeToAdd;
            nodeToAdd->value = pItem;

        }

        return 0;

    }
    else{
        return -1;
    }
}



int List_append(List* pList, void* pItem){
    
    Node * nodeToAdd = getVacantNode();

    if (nodeToAdd != 0){
        
        //if it will be first node in the list
        if (pList->nodes_num == 0){
            pList->head = nodeToAdd;
            nodeToAdd->pointerBack = 0;
            nodeToAdd->pointerNext = 0; 
        }

        //if it will be 2+ node in the list
        else{
            Node * prev_node = pList->end;
            prev_node->pointerNext = nodeToAdd;
            nodeToAdd->pointerBack = prev_node;
            nodeToAdd->pointerNext = 0;
        }

        nodeToAdd->value = pItem;
        pList->nodes_num = pList->nodes_num + 1;
        pList->end = nodeToAdd;
        pList->current = nodeToAdd;
        pList->currentOut = NO; 

        return 0;
    }
    else{
        return -1;
    }  
}



int List_prepend(List* pList, void* pItem){

        Node * nodeToAdd = getVacantNode();

        if (nodeToAdd != 0){
        
        //if it will be first node in the list
        if (pList->nodes_num == 0){
            pList->end = nodeToAdd;
            nodeToAdd->pointerBack = 0;
            nodeToAdd->pointerNext = 0; 
        }

        //if it will be 2+ node in the list
        else{
            Node * next_node = pList->head;
            next_node->pointerBack = nodeToAdd;
            nodeToAdd->pointerNext = next_node;
            nodeToAdd->pointerBack = 0;
        }

        nodeToAdd->value = pItem;
        pList->nodes_num = pList->nodes_num + 1;
        pList->head = nodeToAdd;
        pList->current = nodeToAdd;
        pList->currentOut = NO;   

        return 0;
    }
    else{
        return -1;
    }

}


void* List_remove(List* pList){

    if (pList->nodes_num != 0 && pList->currentOut == NO){
        
        Node * nodeToRemove = pList->current;
        void * item = pList->current->value;

        if (pList->nodes_num == 1){
            pList->current = 0;
            pList->currentOut = LIST_OOB_END;
            pList->head = 0;
            pList->end = 0;
        }

        else if (nodeToRemove == pList->end){
            Node * secondLast = nodeToRemove->pointerBack;
            pList->current = 0;
            pList->currentOut = LIST_OOB_END;
            pList->end = secondLast;
            secondLast->pointerNext = 0;
        }

        else if (nodeToRemove == pList->head){
            Node * second = nodeToRemove->pointerNext;
            second->pointerBack = 0;
            pList->head = second;
            pList->current = second;
        }

        else{
            Node * beforeCurrent = nodeToRemove->pointerBack;
            Node * afterCurrent = nodeToRemove->pointerNext;
            beforeCurrent->pointerNext = afterCurrent;
            afterCurrent->pointerBack = beforeCurrent;

            pList->current = afterCurrent;
        }

        returnNodeToVacant(nodeToRemove);
        pList->nodes_num = pList->nodes_num - 1;
        return item;


    }
    else{
        return 0;
    }

};



void* List_trim(List* pList){

    if (pList->nodes_num != 0){
        
        Node * nodeToDelete = pList->end;
        void * item = nodeToDelete->value;

        if (pList->nodes_num == 1){
            pList->head = 0;
            pList->end = 0;
            pList->current = 0;
            pList->currentOut = LIST_OOB_START;
        }
        else{
            Node * secondLast = nodeToDelete->pointerBack;
            secondLast->pointerNext = 0;
            pList->end = secondLast;
            pList->current = secondLast;
            pList->currentOut = NO;
            
        }

        pList->nodes_num = pList->nodes_num - 1;
        returnNodeToVacant(nodeToDelete);

        return item;

    }
    else{
        return 0;
    }
}

void List_concat(List* pList1, List* pList2){
    if (pList2->nodes_num != 0){

        if (pList1->nodes_num == 0){
            pList1->head = pList2->head;
            pList1->end = pList2->end;
            pList1->nodes_num = pList1->nodes_num + pList2->nodes_num;
        }
        else{
            Node * end_list1 = pList1->end;
            Node * start_list2 = pList2->head;

            end_list1->pointerNext = start_list2;
            start_list2->pointerBack = end_list1;
            pList1->end = pList2->end;
            pList1->nodes_num = pList1->nodes_num + pList2->nodes_num;
        }

        

    }
        
        List * listToAdd = pList2;
        

        if (vacantHeads.free_heads != 0){
        List * lastList = vacantHeads.end;
        listToAdd->prevList = lastList;
        lastList->nextList = listToAdd;
        listToAdd->nextList = 0;
        vacantHeads.end = listToAdd;
        }

        else{
            listToAdd->nextList = 0;
            listToAdd->prevList = 0;
            vacantHeads.head = listToAdd;
            vacantHeads.end = listToAdd;
        }

        vacantHeads.free_heads = vacantHeads.free_heads + 1;

        //Making sure user won't be able to access anything after freeing this list
        listToAdd->head = 0;
        listToAdd->end = 0;
        listToAdd->current = 0;
        listToAdd->nodes_num = 0;
        listToAdd->currentOut = LIST_OOB_START;

};

void List_free(List* pList, FREE_FN pItemFreeFn){
    if (pList->nodes_num != 0){

        Node * start = pList->head;

        while (start != 0){
            void * item = start->value;
            (*pItemFreeFn)(item);
            start = start->pointerNext;
        }

        Node * firstNode = pList->head;
        Node * lastNode = pList->end;

        if (vacantNodes.nodes_num != 0){
        Node * lastVacantNode = vacantNodes.end;

        lastVacantNode->pointerNext = firstNode;
        firstNode->pointerBack = lastVacantNode;
        lastNode->pointerNext = 0;
        vacantNodes.end = lastNode;
        }

        else{
            vacantNodes.head = firstNode;
            vacantNodes.end = lastNode;
            lastNode->pointerNext = 0;
            firstNode->pointerBack = 0;
        }

        vacantNodes.nodes_num = vacantNodes.nodes_num + pList->nodes_num;
    }

        List * listToAdd = pList;
        

        if (vacantHeads.free_heads != 0){
        List * lastList = vacantHeads.end;
        listToAdd->prevList = lastList;
        lastList->nextList = listToAdd;
        listToAdd->nextList = 0;
        vacantHeads.end = listToAdd;
        }

        else{
            listToAdd->nextList = 0;
            listToAdd->prevList = 0;
            vacantHeads.head = listToAdd;
            vacantHeads.end = listToAdd;
        }
        vacantHeads.free_heads = vacantHeads.free_heads + 1;

        //Making sure user won't be able to access anything after freeing this list
        listToAdd->head = 0;
        listToAdd->end = 0;
        listToAdd->current = 0;
        listToAdd->nodes_num = 0;
        listToAdd->currentOut = LIST_OOB_START;

};

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    if (pList->nodes_num != 0 || pList->currentOut != LIST_OOB_END){

        if (pList->currentOut == LIST_OOB_START){
            pList->currentOut = NO;
            pList->current = pList->head;
        }

        Node * node_with_match = 0;

        while (pList->current != 0){
            void * item_to_check = pList->current->value;

            bool check_result = (*pComparator)(item_to_check, pComparisonArg);

            if (check_result == 1){
                node_with_match = pList->current;
                void * found = node_with_match->value;
                return found;
            }

            pList->current = pList->current->pointerNext;
        }

        pList->currentOut = LIST_OOB_END;
        return 0;
    }
    else{

        return 0;
    }
};

