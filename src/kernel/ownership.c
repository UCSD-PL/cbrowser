#include <stdlib.h>
#include <search.h>
#include <assert.h>
#include "ownership.h"

typedef struct data_reader_list {
  struct data_reader_list *f;
  struct data_reader_list *b;
  void *v;
  reader_list_t *readers;
} data_list_t;

data_list_t *data;

int
is_reader(int reader, void *p)
{
  reader_list_t *readers = get_readers(p);
  while (readers) {
    if (readers->r == reader) return 1;
    readers = readers->f;
  }
  return 0;
}

reader_list_t *
dup_readers(reader_list_t *readers)
{
  reader_list_t *new_list, *new_list_head, *new_node;

  if (readers == NULL) return;

  //Special case the first one
  new_list_head = malloc(sizeof(*new_list_head));
  new_list_head->r = readers->r;
  insque(new_list_head, NULL);

  new_list = new_list_head;
  readers = readers->f;
  while (readers) {
    new_node = malloc(sizeof(*new_node));
    new_node->r = readers->r;

    //add to list
    insque(new_node, new_list);

    //advance
    new_list = new_node;
    readers = readers->f;
  }

  return new_list_head;
}

void
add_readers(reader_list_t *readers, void *p)
{
  data_list_t *cur_data, *new_data;
  reader_list_t *cur_reader, *new_readers;

  if (readers == NULL) return;

  new_readers = dup_readers(readers);

  if (data == NULL) {
    data = malloc(sizeof(*data));
    data->v = p;
    data->readers = readers;
    return;
  }

  cur_data = data;
  while (cur_data) {
    if (p == cur_data->v) {
      if (cur_data->readers) {
        insque(new_readers, cur_data->readers);
      } else {
        cur_data->readers = readers;
      }
      return;
    }
    else if (cur_data->f == NULL) {
      new_data = malloc(sizeof(*new_data));
      new_data->v = p;
      /* new_reader = malloc(sizeof(*new_reader)); */
      /* new_reader->r = readers; */
      new_data->readers = readers;
      insque(new_data, cur_data);
      return;
    }
    cur_data = cur_data->f;
  }
}

void
add_reader(int reader, void *p)
{
  data_list_t *cur_data, *new_data;
  reader_list_t *cur_reader, *new_reader;

  if (data == NULL) {
    data = malloc(sizeof(*data));
    data->v = p;
    data->readers = malloc(sizeof(*(data->readers)));
    data->readers->r = reader;
    insque(data->readers, NULL);
    insque(data, NULL);
    return;
  }

  cur_data = data;
  while (cur_data) {
    if (cur_data->v == p) {
      //Found the data. Add the new reader if necessary.
      cur_reader = cur_data->readers;
      while (cur_reader) {
        if (cur_reader->r == reader) {
          return;
        } else if (cur_reader->f == NULL) {
          new_reader = malloc(sizeof(*new_reader));
          new_reader->r = reader;
          insque(new_reader, cur_reader);
          return;
        }
        cur_reader = cur_reader->f;
      }
    } else if (cur_data->f == NULL) {
      new_data = malloc(sizeof(*new_data));
      new_data->v = p;

      new_reader = malloc(sizeof(*new_reader));
      new_reader->r = reader;

      new_data->readers = new_reader;
      insque(new_data->readers, NULL);
      insque(new_data, cur_data);
    }
    cur_data = cur_data->f;
  }
}

reader_list_t *
get_reader_list(void *p)
{
  data_list_t *cur_data;

  if (data == NULL) {
    return NULL;
  }

  cur_data = data;
  while (cur_data) {
    if (cur_data->v == p) {
      return cur_data->readers;
    }
    cur_data = cur_data->f;
  }

  return NULL;
}

void
del_reader(int reader, void *p)
{
  reader_list_t *readers;

  readers = get_reader_list(p);

  while (readers) {
    if (readers->r == reader) {
      remque(readers);
    }
  }
}

reader_list_t *
get_readers(void *p)
{
  return get_reader_list(p);
}


