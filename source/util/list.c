#include <exengine/util/list.h>

#include <stdlib.h>

ex_list_node_t *ex_list_new() {
  ex_list_node_t *n = calloc(1, sizeof(ex_list_node_t));
  n->prev = nullptr;
  n->next = nullptr;
  n->data = nullptr;

  return n;
}

void ex_list_add(ex_list_node_t *n, void *data) {
  // get next empty node
  while (n->data != nullptr) {
    if (n->next != nullptr) {
      n = n->next;
    } else {
      return;
    }
  }

  // set data and add new node
  n->data = data;
  n->next = calloc(1, sizeof(ex_list_node_t));
  n->next->prev = n;
  n->next->next = nullptr;
  n->next->data = nullptr;
}

ex_list_t *ex_list_remove(ex_list_node_t *n, void *data) {
  ex_list_t *root = n;

  // find node with matching data addr
  while (n->data != data) {
    if (n->next != nullptr) {
      n = n->next;
    } else {
      return root;
    }
  }

  // remove node and adjust root
  if (n->prev != nullptr) {
    if (n->next != nullptr) {
      n->prev->next = n->next;
      n->next->prev = n->prev;
    } else {
      n->prev->next = nullptr;
    }
  } else if (n->next != nullptr) {
    // reset root node
    root = n->next;
    root->prev = nullptr;
  }

  // free node
  n->data = nullptr;
  free(n);

  return root;
}

void ex_list_destroy(ex_list_node_t *n) {
  if (n == NULL) {
    return;
  }

  // remove first node
  ex_list_node_t *next = n->next;
  free(n);

  // remove the remaining nodes
  while (next != NULL) {
    n = next;
    next = next->next;
    free(n);
  }
}