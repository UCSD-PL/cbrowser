/*
  Functions for checking ownership of data
*/

typedef struct reader_list {
  struct reader_list *f;
  struct reader_list *b;
  int r;
} reader_list_t;


//!= 0 iff p points to data that 'owner'
//can read, or data that is 'low' security
int is_reader(int owner, void *p);

//sets the reader for p, elevating the
//security level
void set_reader(int owner, void *p);

//sets the reader for p, elevating the
//security level
void set_readers(reader_list_t *os, void *p);

//gets the reader for p, elevating the
//security level
reader_list_t *get_readers(void *p);
