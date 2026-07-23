/* list
 * A simple arbitrary linked-list implementation.
 */

#pragma once

typedef struct ex_list_node_t ex_list_node_t;
typedef ex_list_node_t ex_list_t;

struct ex_list_node_t {
  void *data;
  struct ex_list_node_t *next, *prev;
};

/**
 * @brief Initialize a new linked list.
 * @return pointer to the new list node
 */
ex_list_node_t *ex_list_new();

/**
 * @brief Add a new element to the list.
 * @param n    list to add the element to
 * @param data pointer to the data to add
 */
void ex_list_add(ex_list_node_t *n, void *data);

/**
 * @brief Remove an element from the list.
 * @param n    list to remove the element from
 * @param data pointer to the data to remove
 * @return restructured list pointer
 */
ex_list_t *ex_list_remove(ex_list_node_t *n, void *data);

/**
 * @brief Clean up list allocations.
 * @param n pointer to the list to delete
 */
void ex_list_destroy(ex_list_node_t *n);
