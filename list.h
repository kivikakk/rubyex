#ifndef __RUBYEX_LIST_H__
#define __RUBYEX_LIST_H__

#define list_define(x) \
  struct x##_list { \
    struct x *payload; \
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
  }

#endif

