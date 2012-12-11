#include <csolve.h>
#include <stdlib.h>
#include "msg.h"
#include "ownership.h"
#include "tags.h"

void
send_msg(int fd1, int fd2, int b, message *m)
{
  if (b) {
    assert_tagged(fd1, m);
  } else {
    assert_tagged(fd2, m);
  }
}


void
main(int fd1, int fd2, int b, message *m, message *m2) CHECK_TYPE
{
  int t;

  if (b) {
    m = tag(fd1, m);
    m->content = tag(fd1, m->content);
    assert_tagged(fd1, m->content);
  } else {
    m = tag(fd2, m);
    m->content = tag(fd2, m->content);
    assert_tagged(fd2, m->content);
  }

  if (b) {
    t = get_tag(m);
    m2 = xfer_tag(fd1, m2, m);
    m2 = xfer_tag(fd2, m2, m);
    /* m2 = xfer_tag(nondet(), m2, m); */
    assert_tagged(fd1, m);
    assert_tagged(fd2, m);
    /* assert_tagged(t, m); */
  }

  send_msg(fd1, fd2, b, m);
}
