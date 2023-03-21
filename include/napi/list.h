#pragma once

#include <napi/core.h>

#define np_list_foreach(list, var) if(list != NULL) if (list->__items_size > 0) for (NP_List_Item **var = list->__items_pool; var < list->__items_pool+list->__items_size && list->__items_size > 0; ++var)

struct NP_List_Item
{
    void *value;
    int64_t id;
};

struct NP_List
{
    struct NP_List_Item **__items_pool;
    int64_t __items_size;
    int64_t __items_index;
};

typedef struct NP_List NP_List;
typedef struct NP_List_Item NP_List_Item;

extern NP_List *np_list_create();
extern NP_List_Item *np_list_get_item(NP_List *list, size_t id);
extern NP_List_Item np_list_item(void *value);
extern int64_t np_list_append(NP_List *list, NP_List_Item item);
extern int64_t np_list_length(NP_List *list);
extern void np_list_remove(NP_List *list, int64_t id);
extern void np_list_destroy(NP_List *list);

