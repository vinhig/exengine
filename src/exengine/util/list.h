/* list
 * A simple arbitrary linked-list implementation.
 */

#ifndef EX_LIST_H
#define EX_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct ex_list_node_t ex_list_node_t;
typedef ex_list_node_t ex_list_t;

struct ex_list_node_t {
  void *data;
  struct ex_list_node_t *next, *prev;
};

/**
 * [ex_list_new initialize a new list]
 * @return [ex_list_t pointer]
 */
ex_list_node_t *ex_list_new();

/**
 * [ex_list_add add a new element to the list]
 * @param n    [ex_list_t to add element]
 * @param data [void pointer of data to add]
 */
void ex_list_add(ex_list_node_t *n, void *data);

/**
 * [ex_list_remove remove an element from the list]
 * @param  n    [ex_list_t to remove element]
 * @param  data [void pointer of data to remove]
 * @return      [restructured ex_list_t pointer]
 */
ex_list_t *ex_list_remove(ex_list_node_t *n, void *data);

/**
 * [ex_list_destroy cleanup mallocs etc]
 * @param n [ex_list_t pointer to delete]
 */
void ex_list_destroy(ex_list_node_t *n);

#endif // EX_LIST_H