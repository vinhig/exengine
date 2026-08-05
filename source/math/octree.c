#include <exengine/math/octree.h>

#include <stdio.h>
#include <stdlib.h>

int ex_octree_min_size = EX_OCTREE_DEFAULT_MIN_SIZE;

ex_octree_t *ex_octree_new(void) {
  ex_octree_t *o = calloc(1, sizeof(ex_octree_t));

  for (int i = 0; i < 8; i++) {
    o->children[i] = NULL;
  }
  memset(o->region.min, 0.0f, sizeof(vec3));
  memset(o->region.max, 1.0f, sizeof(vec3));

  o->built = 0;
  o->first = 1;
  o->obj_list = ex_list_new();

  o->data_len = 0;
  o->data = NULL;

  return o;
}

void ex_octree_init(ex_octree_t *o, rect_t region, ex_list_t *objects) {
  memcpy(&o->region, &region, sizeof(rect_t));
  for (int i = 0; i < 8; i++) {
    o->children[i] = NULL;
  }
  o->obj_list = objects;
  o->built = 0;
  o->first = 0;
  o->data_len = 0;
  o->data = NULL;
}

void ex_octree_build(ex_octree_t *o) {
  if (o->obj_list->data == NULL) {
    return;
  }

  if (o->obj_list->next == NULL) {
    ex_octree_finalize(o);
    return;
  }

  // our size
  vec3 region;
  vec3_sub(region, o->region.max, o->region.min);

  if (region[0] <= ex_octree_min_size || region[1] <= ex_octree_min_size || region[2] <= ex_octree_min_size) {
    if (!o->first) {
      ex_octree_finalize(o);
      return;
    }
  }

  vec3 half, center;
  vec3_scale(half, region, 0.5f);
  vec3_add(center, o->region.min, half);

  // octant regions
  rect_t octants[8];
  octants[0] = ex_rect_new(o->region.min, center);
  octants[1] = ex_rect_new((vec3){center[0], o->region.min[1], o->region.min[2]}, (vec3){o->region.max[0], center[1], center[2]});
  octants[2] = ex_rect_new((vec3){center[0], o->region.min[1], center[2]}, (vec3){o->region.max[0], center[1], o->region.max[2]});
  octants[3] = ex_rect_new((vec3){o->region.min[0], o->region.min[1], center[2]}, (vec3){center[0], center[1], o->region.max[2]});
  octants[4] = ex_rect_new((vec3){o->region.min[0], center[1], o->region.min[2]}, (vec3){center[0], o->region.max[1], center[2]});
  octants[5] = ex_rect_new((vec3){center[0], center[1], o->region.min[2]}, (vec3){o->region.max[0], o->region.max[1], center[2]});
  octants[6] = ex_rect_new(center, o->region.max);
  octants[7] = ex_rect_new((vec3){o->region.min[0], center[1], center[2]}, (vec3){center[0], o->region.max[1], o->region.max[2]});

  // object lists
  ex_list_t *obj_lists[8];
  size_t obj_lenghts[8];
  for (int i = 0; i < 8; i++) {
    obj_lists[i] = ex_list_new();
    obj_lenghts[i] = 0;
  }

  // add objects to appropriate octant
  size_t obj_count = 0;
  ex_list_node_t *n = o->obj_list;
  while (n->data != NULL) {
    int found = 0;

    for (int j = 0; j < 8; j++) {
      ex_octree_obj_t *obj = (ex_octree_obj_t *)n->data;
      if (ex_aabb_inside(octants[j], obj->box)) {
        ex_list_add(obj_lists[j], (void *)n->data);
        obj_lenghts[j]++;
        found = 1;
        break;
      }
    }

    // remove obj from this list
    if (found) {
      ex_list_t *next = n->next;
      o->obj_list = ex_list_remove(o->obj_list, (void *)n->data);
      if (next != NULL) {
        n = next;
        continue;
      } else {
        break;
      }
    } else {
      obj_count++;
    }

    if (n->next != NULL) {
      n = n->next;
    } else {
      break;
    }
  }

  // create children
  for (int i = 0; i < 8; i++) {
    if (obj_lists[i]->data != NULL) {
      o->children[i] = calloc(1, sizeof(ex_octree_t));
      ex_octree_init(o->children[i], octants[i], obj_lists[i]);
      o->children[i]->data_len = obj_lenghts[i];
      ex_octree_build(o->children[i]);
    } else {
      o->children[i] = NULL;
    }
  }

  o->data_len = obj_count;
  ex_octree_finalize(o);
}

void ex_octree_finalize(ex_octree_t *o) {
  // move object data into a flat array
  int i = 0;
  ex_list_node_t *n = o->obj_list;
  while (n->data != NULL) {
    ex_octree_obj_t *data = n->data;

    if (i == 0) {
      o->data = calloc(1, o->data_len * sizeof(uint32_t));
    }
    memcpy(&o->data[i], &data->index, sizeof(uint32_t));

    free(n->data);
    n->data = NULL;
    i++;
    if (n->next != NULL) {
      n = n->next;
    } else {
      break;
    }
  }

  // destroy our temp list
  if (o->obj_list != NULL) {
    ex_list_destroy(o->obj_list);
    o->obj_list = NULL;
  }

  o->built = 1;
}

ex_octree_t *ex_octree_reset(ex_octree_t *o) {
  if (o == NULL) {
    return NULL;
  }

  for (int i = 0; i < 8; i++) {
    if (o->children[i] != NULL) {
      ex_octree_reset(o->children[i]);
    }
  }

  if (o->obj_list != NULL) {
    ex_list_destroy(o->obj_list);
    o->obj_list = NULL;
  }

  if (o->data_len > 0 && o->data != NULL) {
    free(o->data);
  }

  if (!o->first) {
    free(o);
  } else {
    free(o);
    return ex_octree_new();
  }

  return NULL;
}

void ex_octree_destroy(ex_octree_t *o) {
  if (o == NULL) {
    return;
  }

  for (int i = 0; i < 8; i++) {
    if (o->children[i] != NULL) {
      ex_octree_destroy(o->children[i]);
    }
  }

  if (o->obj_list != NULL) {
    ex_list_destroy(o->obj_list);
    o->obj_list = NULL;
  }

  if (o->data_len > 0 && o->data != NULL) {
    free(o->data);
  }

  free(o);
}

void ex_octree_get_colliding_count(ex_octree_t *o, rect_t *bounds, int *count) {
  if (o == NULL) {
    return;
  }

  // add our data to the list
  if (o->data != NULL) {
    if (!ex_aabb_aabb(o->region, *bounds)) {
      return;
    }

    (*count)++;
  }

  // recurse adding data to the list
  for (int i = 0; i < 8; i++) {
    if (o->children[i] != NULL) {
      ex_octree_get_colliding_count(o->children[i], bounds, count);
    }
  }
}

void ex_octree_get_colliding(ex_octree_t *o, rect_t *bounds, ex_octree_data_t *data_list, int *index) {
  if (o == NULL) {
    return;
  }

  // add our data to the list
  if (o->data != NULL) {
    if (!ex_aabb_aabb(o->region, *bounds)) {
      return;
    }

    data_list[*index].len = o->data_len;
    data_list[*index].data = o->data;
    (*index)++;
  }

  // recurse adding data to the list
  for (int i = 0; i < 8; i++) {
    if (o->children[i] != NULL) {
      ex_octree_get_colliding(o->children[i], bounds, data_list, index);
    }
  }
}
