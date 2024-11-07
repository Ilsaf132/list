#include <stdio.h>
#include <stdlib.h>
#include "ConstantsList.h"

struct List {
    int* data;
    int* next;
    int* prev;
    int free;
    int size;
    int capacity;
};

Error_list InitList(struct List* list, const char* dump_html_name, FILE** dump_html);
Error_list DumpList(struct List* list);
void GraphDump(struct List* list, const char* file_name, FILE** dump_html);
Error_list PoisonArray(int* arr, int index_start);
Error_list CheckListPush(struct List* list, int anchor);
Error_list CheckListPop(struct List* list, int anchor);
Error_list PushList(struct List* list, int elem, int anchor);
Error_list PopList(struct List* list, int anchor);
void DtorList(struct List* list, FILE** dump_html);
void MySystem(const char* str, int number_dump);

int main() {
    List list = {};
    const char* file_name = "Dump.dot";
    const char* dump_html_name = "Dump.html";
    FILE* dump_html = NULL;

    if(InitList(&list, dump_html_name, &dump_html))         return -1;
    if(PushList(&list, 10, 0))  return -1;
    if(PushList(&list, 20, 1))  return -1;
    GraphDump(&list, file_name, &dump_html);
    if(PushList(&list, 30, 1))  return -1;
    GraphDump(&list, file_name, &dump_html);
    if(PopList(&list, 2))       return -1;
    GraphDump(&list, file_name, &dump_html);
    DtorList(&list, &dump_html);
}

Error_list InitList(struct List* list, const char* dump_html_name, FILE** dump_html) {
    if (list == NULL) {
        printf("Error in address of list!\n");
        return ERROR_LIST_ADDRESS;
    }
    
    *dump_html = fopen(dump_html_name, "w+");

    list -> capacity = DATA_SIZE;
    list -> size = 1;
    list -> data = (int*) calloc(list -> capacity, sizeof(int));
    list -> next = (int*) calloc(list -> capacity, sizeof(int));
    list -> prev = (int*) calloc(list -> capacity, sizeof(int));
    list -> free = 1;

    PoisonArray(list -> data, 0);
    PoisonArray(list -> prev, 1);
    list -> prev[0] = 0;
    list -> next[0] = 0;

    for(int i = 1; i < list -> capacity - 1; ++i) {
        list -> next[i] = i + 1;
    }
    list -> next[list -> capacity - 1] = 0;

    DumpList(list);
    return ERROR_OK;
}

Error_list DumpList(struct List* list) {
    printf("\n---------------------------DUMP----------------------------\n");
    printf("data: ");
    for(int i = 0; i < list -> capacity; ++i) {
        printf("%d ", list-> data[i]);
    }
    printf("\nnext: ");
    for(int i = 0; i < list -> capacity; ++i) {
        printf("%d ", list-> next[i]);
    }
    printf("\nprev: ");
    for(int i = 0; i < list -> capacity; ++i) {
        printf("%d ", list-> prev[i]);
    }
    printf("\nfree: %d", list -> free);
    printf("\nsize: %d", list -> size);
    printf("\ncapacity: %d", list -> capacity);
    printf("\n-----------------------------------------------------------\n");
    return ERROR_OK;
}

void GraphDump(struct List* list, const char* file_name, FILE** dump_html) {
    FILE* file_dump = fopen(file_name, "w");

    fprintf(file_dump, "digraph list {\n    rankdir = LR\n");

    fprintf(file_dump, "    node%d [shape = Mrecord, style = \"filled\", label = \"%d | data: %d | next: %d | prev: %d\", "
                                    "fillcolor = \"purple\"]\n",               0, 0, list->data[0], list->next[0], list->prev[0]);
    for(int i = 1; i < list -> capacity; ++i) {
        fprintf(file_dump, "    node%d [shape = Mrecord, style = \"filled\", label = \"%d | data: %d | next: %d | prev: %d\", "
        "fillcolor = \"yellow\"]\n",                                                i, i, list->data[i], list->next[i], list->prev[i]);
    }

    fprintf(file_dump, "    free [fillcolor = \"blue\", color = \"black\"]\n");

    int i = 0;
    do {
        fprintf(file_dump, "    node%d -> node%d[color = \"darkgreen\"]\n", i, list->next[i]);
        i = list->next[i];
    } while (i != 0);

    fprintf(file_dump, "    free -> node%d[color = \"red\"]\n", list -> free);
    i = list->free;

    while(list -> next[i] != 0) {
        fprintf(file_dump, "    node%d -> node%d[color = \"red\"]\n", i, list->next[i]);
        i = list->next[i];
    }
    fprintf(file_dump, "}");

    fclose(file_dump);

    static int number_dump = 0;
    MySystem("dot Dump.dot -Tpng -o Graphs\\Dump%02d.png", number_dump);

    if(number_dump == 0) {
        fprintf(*dump_html, "<pre>\n");
    }

    fprintf(*dump_html, "\n<img src = \"Graphs\\Dump%02d.png\"/>\n", number_dump);
    number_dump++;
}

void MySystem(const char* str, int number_dump)
{
    char command_dot[52] = "";

    sprintf(command_dot, str, number_dump);

    system(command_dot);
}

Error_list PoisonArray(int* arr, int index_start) {
    for(int i = index_start; i < DATA_SIZE; ++i) {
        arr[i] = POISON_CONST;
    }
    return ERROR_OK;
}

Error_list PushList(struct List* list, int elem, int anchor) {
    if (CheckListPush(list, anchor)) {
        return ERROR_PUSH;
    }

    list -> data[list -> free] = elem;
    int old_free = list -> free;
    list -> free = list -> next[list -> free];

    int index = 0;
    for(int i = 0; i < anchor; ++i) {
        index = list -> next[index];
    }

    list -> next[old_free] = list -> next[index];
    list -> prev[list -> next[index]] = old_free;
    list -> next[index] = old_free;
    list -> prev[old_free] = index; 

    list -> size++;

    return ERROR_OK;
}

Error_list PopList(struct List* list, int anchor) {
    if (CheckListPop(list, anchor)) {
        return ERROR_POP;
    }

    int index = 0;
    for(int i = 0; i < anchor; ++i) {
        index = list -> next[index];
    }
    list -> data[index] = POISON_CONST;

    int ind_next = list -> next[index];
    int ind_prev = list -> prev[index];

    list -> next[ind_prev] = ind_next;
    list -> prev[ind_next] = ind_prev;

    list -> prev[index] = POISON_CONST;
    list -> next[index] = list -> free;
    list -> free = index;

    list -> size--;
    return ERROR_OK;
}

Error_list CheckListPop(struct List* list, int anchor) {
    if (list == NULL) {
        printf("Error in address of list!\n");
        return ERROR_LIST_ADDRESS;
    }
    if (anchor >= list -> size || anchor < 0) {
        printf("Problem with anchor element!\n");
        return ERROR_ANCHOR;
    }
    if (list -> size > list -> capacity) {
        printf("No capacity! Realloc the memory!\n");
        return ERROR_CAPACITY;
    }
    if(list -> size <= 0) {
        printf("Problem with size of elements in list!\n");
        return ERROR_SIZE;
    }
    return ERROR_OK;

}

Error_list CheckListPush(struct List* list, int anchor) {
    if (list == NULL) {
        printf("Error in address of list!\n");
        return ERROR_LIST_ADDRESS;
    }
    if (list -> size >= list -> capacity) {
        printf("No capacity! Realloc the memory!\n");
        return ERROR_CAPACITY;
    }
    if (list -> free <= 0 || list -> free >= list -> capacity) {
        printf("Problem with free element`s address!\n");
        return ERROR_FREE;
    }
    if (anchor >= list -> size || anchor < 0) {
        printf("Anchor element is enourmous!\n");
        return ERROR_ANCHOR;
    }
    if(list -> size < 0) {
        printf("Problem with size of elements in list!\n");
        return ERROR_SIZE;
    }
    return ERROR_OK;
}

void DtorList(struct List* list, FILE** dump_html){
    fclose(*dump_html);
    free(list -> data);
    free(list -> next);
    free(list -> prev);
}