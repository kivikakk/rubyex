#ifndef __RUBYEX_LIST_H__
#define __RUBYEX_LIST_H__

#include <linux/vmalloc.h>

#define list_define(x) \
  struct x##_list_node { \
    struct x *x; \
    struct x##_list_node *next; \
  }; \
  \
  struct x##_list { \
    struct x##_list_node *head; \
  }; \
  \
  static inline int x##_list_length(struct x##_list *list) { \
    int length = 0; \
    struct x##_list_node *node; \
    if (!list) \
      return 0; \
    node = list->head; \
    while (node) { \
      node = node->next; \
      length++; \
    } \
    return length; \
  } \
  \
  static inline struct x##_list *x##_list_allocate(struct x##_list_node *head) { \
    struct x##_list *list = vmalloc(sizeof(struct x##_list)); \
    list->head = head; \
    return list; \
  } \
  \
  static inline struct x##_list_node *x##_list_node_allocate(struct x *x, struct x##_list_node *next) { \
    struct x##_list_node *node = vmalloc(sizeof(struct x##_list_node)); \
    node->x = x; node->next = next; \
    return node; \
  } \
  \
  static inline void x##_list_append(struct x##_list *list, struct x *item) \
  { \
    struct x##_list_node *node = x##_list_node_allocate(item, NULL); \
    struct x##_list_node **search = &list->head; \
    while (*search) \
      search = &(*search)->next; \
    *search = node; \
  } \


#endif

