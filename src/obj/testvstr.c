// -*- c-basic-offset: 2; indent-tabs-mode: nil -*-

/*
 * DDSLib: Dynamic data structures
 * Copyright (C) 2002-3,2005-6,2012,2016  Steven Simpson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 *
 * Author contact: Email to s.simpson at lancaster.ac.uk
 */

#undef NDEBUG

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <wchar.h>

#include "ddslib/vstr.h"
#include "ddslib/vwcs.h"

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

static void wcheck(int line, const vwcs *p, const wchar_t *s, size_t len)
{
  if (vwcs_len(p) != len - 1 || wmemcmp(vwcs_get(p), s, len - 1)) {
    fprintf(stderr, "%4d EXPECTED: %zu [%.*ls]\n",
            line, len - 1, (int) (len - 1), s);
    fprintf(stderr, "          GOT: %zu [%.*ls]\n",
            vwcs_len(p), (int) vwcs_len(p), vwcs_get(p));
    fflush(stderr);
    abort();
  }
}

#define WCHECK(P, S) wcheck(__LINE__, (P), (S), sizeof (S) / sizeof(wchar_t))

int main(void)
{ 
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
      int rc = vstr_empty(&s1);
      assert(rc == 0);
      CHECK(&s1, "");

      rc = vstr_appendf(&s1, "%dx%d", 21, 34);
      assert(rc >= 0);
      CHECK(&s1, "21x34");
    }

    {
      int rc = vstr_setf(&s1, "%dx%d", 21, 34);
      assert(rc >= 0);
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
  }



  {
    vwcs s1 = vwcs_NULL;

    assert(vwcs_get(&s1) == NULL);
    assert(vwcs_len(&s1) == 0);
    assert(vwcs_extract(&s1) == NULL);

    {
      vwcs_setc(&s1, L'z', 10);
      WCHECK(&s1, L"zzzzzzzzzz");
    }

    {
      vwcs_truncate(&s1, 6);
      WCHECK(&s1, L"zzzzzz");
    }

    {
      wchar_t *p = vwcs_splice(&s1, 3, 10);
      assert(p == vwcs_get(&s1) + 3);
      assert(vwcs_len(&s1) == 16);
      assert(memcmp(vwcs_get(&s1), L"zzz", 3) == 0);
      assert(memcmp(vwcs_get(&s1) + 13, L"zzz", 3) == 0);
    }

    {
      vwcs_elide(&s1, 3, 10);
      WCHECK(&s1, L"zzzzzz");
    }

    {
      int rc = vwcs_insertc(&s1, 3, L'a', 7);
      assert(rc == 0);
      WCHECK(&s1, L"zzzaaaaaaazzz");
    }

    {
      int rc = vwcs_insert(&s1, 5, L"boo");
      assert(rc == 0);
      WCHECK(&s1, L"zzzaabooaaaaazzz");
    }

    {
      int rc = vwcs_setf(&s1, L"%dx%d", 21, 34);
      assert(rc >= 0);
      WCHECK(&s1, L"21x34");
    }

    {
      int rc = vwcs_set0(&s1, L"worst");
      assert(rc == 0);
      WCHECK(&s1, L"worst\0");
    }

    vwcs s2 = vwcs_NULL;
    assert(vwcs_get(&s2) == NULL);
    assert(vwcs_len(&s2) == 0);
    assert(vwcs_extract(&s2) == NULL);

    {
      int rc = vwcs_set(&s2, L"bing-bong");
      assert(rc == 0);
      WCHECK(&s2, L"bing-bong");
    }



    {
      int rc = vwcs_setv(&s1, &s2);
      assert(rc == 0);
      WCHECK(&s1, L"bing-bong");
    }

    {
      int rc = vwcs_setvi(&s1, &s2, 2);
      assert(rc == 0);
      WCHECK(&s1, L"ng-bong");
    }

    {
      int rc = vwcs_setvi(&s1, &s2, 12);
      assert(rc == 0);
      WCHECK(&s1, L"");
    }

    {
      int rc = vwcs_setvn(&s1, &s2, 5);
      assert(rc == 0);
      WCHECK(&s1, L"bing-");
    }

    {
      int rc = vwcs_setvn(&s1, &s2, 12);
      assert(rc == 0);
      WCHECK(&s1, L"bing-bong");
    }

    {
      int rc = vwcs_setvin(&s1, &s2, 1, 2);
      assert(rc == 0);
      WCHECK(&s1, L"in");
    }

    {
      int rc = vwcs_setvr(&s1, &s2, 5);
      assert(rc == 0);
      WCHECK(&s1, L"-bong");
    }

    {
      int rc = vwcs_setvr(&s1, &s2, 20);
      assert(rc == 0);
      WCHECK(&s1, L"bing-bong");
    }

    {
      int rc = vwcs_setvrn(&s1, &s2, 5, 3);
      assert(rc == 0);
      WCHECK(&s1, L"-bo");
    }

    {
      int rc = vwcs_setvrn(&s1, &s2, 3, 5);
      assert(rc == 0);
      WCHECK(&s1, L"ong");
    }

    {
      int rc = vwcs_setvrn(&s1, &s2, 12, 5);
      assert(rc == 0);
      WCHECK(&s1, L"bi");
    }

    {
      int rc = vwcs_setvrn(&s1, &s2, 12, 2);
      assert(rc == 0);
      WCHECK(&s1, L"");
    }






    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elide(&s1, 3, 3);
      WCHECK(&s1, L"hoo-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elide(&s1, 3, 10);
      WCHECK(&s1, L"hoo");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elide(&s1, 12, 10);
      WCHECK(&s1, L"hoojar-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relide(&s1, 3, 3);
      WCHECK(&s1, L"hoojalip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relide(&s1, 3, 10);
      WCHECK(&s1, L"lip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relide(&s1, 12, 10);
      WCHECK(&s1, L"hoojar-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elider(&s1, 5, 3);
      WCHECK(&s1, L"hoojarip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elider(&s1, 5, 7);
      WCHECK(&s1, L"hoojar");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elider(&s1, 12, 7);
      WCHECK(&s1, L"-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elider(&s1, 12, 14);
      WCHECK(&s1, L"");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relider(&s1, 5, 3);
      WCHECK(&s1, L"hor-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relider(&s1, 5, 7);
      WCHECK(&s1, L"r-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relider(&s1, 12, 7);
      WCHECK(&s1, L"hooja");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relider(&s1, 12, 14);
      WCHECK(&s1, L"");
    }





    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elect(&s1, 3, 3);
      WCHECK(&s1, L"jar");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elect(&s1, 3, 10);
      WCHECK(&s1, L"jar-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_elect(&s1, 12, 10);
      WCHECK(&s1, L"");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relect(&s1, 3, 3);
      WCHECK(&s1, L"r-f");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relect(&s1, 3, 10);
      WCHECK(&s1, L"hoojar-f");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relect(&s1, 12, 10);
      WCHECK(&s1, L"");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_electr(&s1, 5, 3);
      WCHECK(&s1, L"-fl");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_electr(&s1, 5, 7);
      WCHECK(&s1, L"-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_electr(&s1, 12, 7);
      WCHECK(&s1, L"hoojar");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_electr(&s1, 12, 14);
      WCHECK(&s1, L"hoojar-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relectr(&s1, 5, 3);
      WCHECK(&s1, L"oja");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relectr(&s1, 5, 7);
      WCHECK(&s1, L"hooja");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relectr(&s1, 12, 7);
      WCHECK(&s1, L"r-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_relectr(&s1, 12, 14);
      WCHECK(&s1, L"hoojar-flip");
    }



    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_truncate(&s1, 7);
      WCHECK(&s1, L"hoojar-");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_truncate(&s1, 14);
      WCHECK(&s1, L"hoojar-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_rtruncate(&s1, 7);
      WCHECK(&s1, L"hooj");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_rtruncate(&s1, 14);
      WCHECK(&s1, L"");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_neck(&s1, 7);
      WCHECK(&s1, L"flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_neck(&s1, 14);
      WCHECK(&s1, L"");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_rneck(&s1, 7);
      WCHECK(&s1, L"ar-flip");
    }

    {
      int rc = vwcs_set(&s1, L"hoojar-flip");
      assert(rc == 0);
      vwcs_rneck(&s1, 14);
      WCHECK(&s1, L"");
    }
  }

  return 0;
}
