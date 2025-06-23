#ifndef TOC_LIST_H
#define TOC_LIST_H

#include <stddef.h> /* for offsetof */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* List api (abreviated) */
#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)
  
#define container_of(ptr, type, member)                 \
  (type*)(((char*)(ptr)) - offsetof(type,member))

  struct list_head {
    struct list_head *next, *prev;
  };

#define LIST_HEAD_INIT(name) { &(name), &(name) }

  static inline void INIT_LIST_HEAD(struct list_head *list)
  {
    list->next = list;
    list->prev = list;
  }

#define list_entry(ptr, type, member)           \
  container_of(ptr, type, member)
  
#define list_for_each_entry(pos, pos_type, head, member, lhp)		\
  for ((lhp)= ((head)->next),                                           \
         pos = (pos_type*)((char*)(lhp)-offsetof(pos_type,member));	\
       &pos->member != (head);                                          \
       (lhp) = pos->member.next,					\
         pos = (pos_type*)((char*)(lhp)-offsetof(pos_type,member)))

#define list_for_each_entry_safe(pos, pos_type, n, head, member, lhp)	\
  for ((lhp) = ((head)->next),                                          \
         pos = (pos_type*)((char*)(lhp)-offsetof(pos_type,member)),	\
         (lhp) = pos->member.next,					\
         n = (pos_type*)((char*)(lhp)-offsetof(pos_type,member));	\
       &pos->member != (head);                                          \
       pos = n,                                                         \
         (lhp) = pos->member.next,					\
         n = (pos_type*)((char*)(lhp)-offsetof(pos_type,member)) )

  static inline void __list_del(struct list_head * prev, struct list_head * next)
  {
    next->prev = prev;
    prev->next = next;
  }

  /**
   * list_del - deletes entry from list.
   * @entry: the element to delete from the list.
   * Note: list_empty on entry does not return true after this, the entry is
   * in an undefined state.
   */
  static inline void list_del(struct list_head *entry)
  {
    __list_del(entry->prev, entry->next);
    entry->next = (struct list_head*)LIST_POISON1;
    entry->prev = (struct list_head*)LIST_POISON2;
  }
  
  static inline void __list_add(struct list_head *new_,
                         struct list_head *prev,
                         struct list_head *next)
  {
    next->prev = new_;
    new_->next = next;
    new_->prev = prev;
    prev->next = new_;
  }

  /**
   * list_add - add a new entry
   * @new: new entry to be added
   * @head: list head to add it after
   *
   * Insert a new entry after the specified head.
   * This is good for implementing stacks.
   */
  static inline void list_add(struct list_head *new_, struct list_head *head)
  {
    __list_add(new_, head, head->next);
  }

  /**
   * list_add_tail - add a new entry
   * @new: new entry to be added
   * @head: list head to add it before
   *
   * Insert a new entry before the specified head.
   * This is useful for implementing queues.
   */
  static inline void list_add_tail(struct list_head *new_, struct list_head *head)
  {
    __list_add(new_, head->prev, head);
  }
  
  static inline int list_empty(const struct list_head *head)
  {
    return head->next == head;
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

