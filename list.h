#ifndef __RUBYEX_LIST_H__
#define __RUBYEX_LIST_H__

#include <linux/vmalloc.h>

#define list_define(x) \
  struct x##_list { \
    struct x *x; \
    struct x##_list *next; \
  }; \
  \
  static inline int x##_list_length(struct x##_list *list) \
  { \
    int length = 1; \
    if (list == NULL) \
      return 0; \
    while ((list = list->next)) \
      length++; \
    return length; \
  } \
  \
  static inline struct x##_list *x##_list_allocate(struct x *x, struct x##_list *next) \
  { \
    struct x##_list *node = vmalloc(sizeof(struct x##_list)); \
    node->x = x; \
    node->next = next; \
    return node; \
  } \
  \
  static inline void x##_list_append(struct x##_list **list, struct x *item) \
  { \
    struct x##_list *node = x##_list_allocate(item, NULL); \
    while (*list) \
      list = &(*list)->next; \
    *list = node; \
  }

#endif

