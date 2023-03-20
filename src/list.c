#include "napi/list.h"

#include <string.h>
#include <stdlib.h>

NP_List *np_list_create()
{
    NP_List *list = malloc(sizeof(struct NP_List));
    list->__items_size = 0;

    return list;
}

NP_List_Item *np_list_get_item(NP_List *list, size_t id)
{
    // todo
    np_list_foreach(list, item)
    {
        if ((*item)->id == id)
        {
            return *item;
        }
    }

    return NULL;
}

int64_t np_list_length(NP_List *list)
{
    return list->__items_size;
}

int64_t np_list_append(NP_List *list, NP_List_Item item)
{int64_t i; struct NP_List_Item **old_pool = list->__items_pool;
    // todo: rewrite it to use reallocarray function

    list->__items_size++;
    list->__items_pool = calloc(list->__items_size, sizeof(struct NP_List_Item));
    list->__items_pool[list->__items_size-1] = malloc(sizeof(struct NP_List_Item));
    memcpy(list->__items_pool[list->__items_size-1], &item, sizeof(struct NP_List_Item));
    
    if (list->__items_size > 1)
    {
        for (i = 0; i < list->__items_size-1; ++i)
        {
            list->__items_pool[i] = old_pool[i];
        }

        free(old_pool);
    }

    list->__items_pool[list->__items_size-1]->id = list->__items_index++;
    return list->__items_index-1;
}

void np_list_remove(NP_List *list, int64_t id)
{int64_t mod = 0; int64_t i; struct NP_List_Item **old_pool;
    old_pool = list->__items_pool;
    list->__items_pool = calloc(list->__items_size-1, sizeof(struct NP_List_Item));

    for (i = 0; i < list->__items_size; ++i)
    {
        if (old_pool[i]->id == id)
        {
            free(old_pool[i]);
            ++mod;
            continue;
        }
        list->__items_pool[i - mod] = old_pool[i];
    }

    list->__items_size--;
    free(old_pool);
}

NP_List_Item np_list_item(void *value)
{
    NP_List_Item item = { value };
    return item;
}

void np_list_destroy(NP_List *list)
{size_t i;
    for (i = 0; i < list->__items_size; ++i)
    {
        free(list->__items_pool[i]);
    }

    free(list->__items_pool);
    free(list);
}

