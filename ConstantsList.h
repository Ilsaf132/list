#ifndef CONSTANTS_LIST_
#define CONSTANTS_LIST_

const int DATA_SIZE = 10;
const int POISON_CONST = -1;

enum Error_list {
    ERROR_OK = 0,
    ERROR_LIST_ADDRESS,
    ERROR_CAPACITY,
    ERROR_ANCHOR,
    ERROR_FREE,
    ERROR_PUSH,
    ERROR_POP,
    ERROR_SIZE,
};

#endif