#undef NDEBUG

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "ddslib/vstr.h"

static void check(int line, const vstr *p, const char *s, size_t len)
{
  if (vstr_len(p) != len - 1 || memcmp(vstr_get(p), s, len - 1)) {
    fprintf(stderr, "%4d EXPECTED: %zu [%.*s]\n",
	    line, len - 1, (int) (len - 1), s);
    fprintf(stderr, "          GOT: %zu [%.*s]\n",
	    vstr_len(p), (int) vstr_len(p), vstr_get(p));
    fflush(stderr);
    abort();
  }
}

#define CHECK(P, S) check(__LINE__, (P), (S), sizeof (S))

int main(void)
{
  vstr s1 = vstr_NULL;

  assert(vstr_get(&s1) == NULL);
  assert(vstr_len(&s1) == 0);
  assert(vstr_extract(&s1) == NULL);

  {
    vstr_setc(&s1, 'z', 10);
    CHECK(&s1, "zzzzzzzzzz");
  }

  {
    vstr_truncate(&s1, 6);
    CHECK(&s1, "zzzzzz");
  }

  {
    char *p = vstr_splice(&s1, 3, 10);
    assert(p == vstr_get(&s1) + 3);
    assert(vstr_len(&s1) == 16);
    assert(memcmp(vstr_get(&s1), "zzz", 3) == 0);
    assert(memcmp(vstr_get(&s1) + 13, "zzz", 3) == 0);
  }

  {
    vstr_elide(&s1, 3, 10);
    CHECK(&s1, "zzzzzz");
  }

  {
    int rc = vstr_insertc(&s1, 3, 'a', 7);
    assert(rc == 0);
    CHECK(&s1, "zzzaaaaaaazzz");
  }

  {
    int rc = vstr_insert(&s1, 5, "boo");
    assert(rc == 0);
    CHECK(&s1, "zzzaabooaaaaazzz");
  }

  {
    int rc = vstr_setf(&s1, "%dx%d", 21, 34);
    assert(rc == 0);
    CHECK(&s1, "21x34");
  }

  {
    int rc = vstr_set0(&s1, "worst");
    assert(rc == 0);
    CHECK(&s1, "worst\0");
  }

  vstr s2 = vstr_NULL;
  assert(vstr_get(&s2) == NULL);
  assert(vstr_len(&s2) == 0);
  assert(vstr_extract(&s2) == NULL);

  {
    int rc = vstr_set(&s2, "bing-bong");
    assert(rc == 0);
    CHECK(&s2, "bing-bong");
  }



  {
    int rc = vstr_setv(&s1, &s2);
    assert(rc == 0);
    CHECK(&s1, "bing-bong");
  }

  {
    int rc = vstr_setvi(&s1, &s2, 2);
    assert(rc == 0);
    CHECK(&s1, "ng-bong");
  }

  {
    int rc = vstr_setvi(&s1, &s2, 12);
    assert(rc == 0);
    CHECK(&s1, "");
  }

  {
    int rc = vstr_setvn(&s1, &s2, 5);
    assert(rc == 0);
    CHECK(&s1, "bing-");
  }

  {
    int rc = vstr_setvn(&s1, &s2, 12);
    assert(rc == 0);
    CHECK(&s1, "bing-bong");
  }

  {
    int rc = vstr_setvin(&s1, &s2, 1, 2);
    assert(rc == 0);
    CHECK(&s1, "in");
  }

  {
    int rc = vstr_setvr(&s1, &s2, 5);
    assert(rc == 0);
    CHECK(&s1, "-bong");
  }

  {
    int rc = vstr_setvr(&s1, &s2, 20);
    assert(rc == 0);
    CHECK(&s1, "bing-bong");
  }

  {
    int rc = vstr_setvrn(&s1, &s2, 5, 3);
    assert(rc == 0);
    CHECK(&s1, "-bo");
  }

  {
    int rc = vstr_setvrn(&s1, &s2, 3, 5);
    assert(rc == 0);
    CHECK(&s1, "ong");
  }

  {
    int rc = vstr_setvrn(&s1, &s2, 12, 5);
    assert(rc == 0);
    CHECK(&s1, "bi");
  }

  {
    int rc = vstr_setvrn(&s1, &s2, 12, 2);
    assert(rc == 0);
    CHECK(&s1, "");
  }






  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elide(&s1, 3, 3);
    CHECK(&s1, "hoo-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elide(&s1, 3, 10);
    CHECK(&s1, "hoo");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elide(&s1, 12, 10);
    CHECK(&s1, "hoojar-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relide(&s1, 3, 3);
    CHECK(&s1, "hoojalip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relide(&s1, 3, 10);
    CHECK(&s1, "lip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relide(&s1, 12, 10);
    CHECK(&s1, "hoojar-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elider(&s1, 5, 3);
    CHECK(&s1, "hoojarip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elider(&s1, 5, 7);
    CHECK(&s1, "hoojar");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elider(&s1, 12, 7);
    CHECK(&s1, "-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elider(&s1, 12, 14);
    CHECK(&s1, "");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relider(&s1, 5, 3);
    CHECK(&s1, "hor-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relider(&s1, 5, 7);
    CHECK(&s1, "r-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relider(&s1, 12, 7);
    CHECK(&s1, "hooja");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relider(&s1, 12, 14);
    CHECK(&s1, "");
  }





  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elect(&s1, 3, 3);
    CHECK(&s1, "jar");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elect(&s1, 3, 10);
    CHECK(&s1, "jar-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_elect(&s1, 12, 10);
    CHECK(&s1, "");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relect(&s1, 3, 3);
    CHECK(&s1, "r-f");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relect(&s1, 3, 10);
    CHECK(&s1, "hoojar-f");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relect(&s1, 12, 10);
    CHECK(&s1, "");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_electr(&s1, 5, 3);
    CHECK(&s1, "-fl");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_electr(&s1, 5, 7);
    CHECK(&s1, "-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_electr(&s1, 12, 7);
    CHECK(&s1, "hoojar");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_electr(&s1, 12, 14);
    CHECK(&s1, "hoojar-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relectr(&s1, 5, 3);
    CHECK(&s1, "oja");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relectr(&s1, 5, 7);
    CHECK(&s1, "hooja");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relectr(&s1, 12, 7);
    CHECK(&s1, "r-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_relectr(&s1, 12, 14);
    CHECK(&s1, "hoojar-flip");
  }



  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_truncate(&s1, 7);
    CHECK(&s1, "hoojar-");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_truncate(&s1, 14);
    CHECK(&s1, "hoojar-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_rtruncate(&s1, 7);
    CHECK(&s1, "hooj");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_rtruncate(&s1, 14);
    CHECK(&s1, "");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_neck(&s1, 7);
    CHECK(&s1, "flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_neck(&s1, 14);
    CHECK(&s1, "");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_rneck(&s1, 7);
    CHECK(&s1, "ar-flip");
  }

  {
    int rc = vstr_set(&s1, "hoojar-flip");
    assert(rc == 0);
    vstr_rneck(&s1, 14);
    CHECK(&s1, "");
  }

  return 0;
}
