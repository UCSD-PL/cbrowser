/*
  Functions for checking/propagating ownership of data
*/
#include<stdlib.h>
#include<stdio.h>

typedef struct reader_list {
  struct reader_list *f;
  struct reader_list *b;
  int r;
} reader_list_t;

/*----------------------------*/
/*Wrappers                    */
/*----------------------------*/
/* Copies n bytes from src to dst like memcpy(3) */
void r_memcpy(void *dst, void *src, size_t n);
void r_free(void *p);
FILE *r_popen(char *command, char *mode);
FILE *r_pclose(FILE *stream);

/*----------------------------*/
/*Ownership manipulation      */
/*----------------------------*/
/* iff p points to data that 'owner' can read */
void is_reader(int owner, void * REF(TAG([V]) = owner) p) OKEXTERN;
void is_reader_int(int owner, int REF(TAG([V]) = owner) p) OKEXTERN;
/* after add_reader(o, p), is_reader(o, p) != 0 */
void add_reader(int owner, void * REF(TAG([V]) = TAG([owner])) p) OKEXTERN;

/* stop tracking p */
void rm_data(void *p);

/* same as add_reader for each reader in os */
void add_readers(reader_list_t *os, void *p);

/* gets the readers of p */
reader_list_t *get_readers(void *p);

/* convenient */
void r_xfer(void * REF(TAG([V]) = TAG([src])) dst,
            void * REF(TAG([V]) = TAG([dst])) src) OKEXTERN;

