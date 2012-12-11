#include <csolve.h>
#include "tags.h"
#include "msg.h"

struct message {
  int type;
  char NULLTERMSTR * NNSTART NNSTRINGPTR LOC(L) FINAL content;
};

#define Field(_v, _off) DEREF([BLOCK_BEGIN([_v]) + _off])
#define FieldTags(_v, _off)  TAGSET([Field(V, _off)])

struct message * TAGGED(Field(V, 8), tag) TAGGED(Field(V, 4), tag) TAGGED(V, tag) START VALIDPTR ROOM_FOR(struct message)
create_message(int tag) OKEXTERN;

void main(void)
{
  int fd = nondet();
  struct message *m;
  m = create_message(fd);

  assert_tagged(fd, m);
  assert_tagged(fd, &(m->type));
  assert_tagged(fd, m->content);
}
          
