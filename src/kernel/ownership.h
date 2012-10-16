/*
  Functions for checking ownership of data
*/

typedef struct reader_list {
  struct reader_list *f;
  struct reader_list *b;
  int r;
} reader_list_t;


//iff p points to data that 'owner'
// can read
int is_reader(int owner, void *p);

//after add_reader(o, p), is_reader(o, p) != 0
void add_reader(int owner, void *p);

//stop tracking p
void rm_data(void *p);

//same as add_reader for each reader in os
void add_readers(reader_list_t *os, void *p);

//gets the readers of p
reader_list_t *get_readers(void *p);
