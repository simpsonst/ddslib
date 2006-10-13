#include <stdio.h>
#include <assert.h>

#include "ddslib/vstr.h"

int main()
{
  char temp[10];
  snprintf(temp, sizeof temp, "Madam, I'm Adam.");
  printf("Last character is %d.\n", temp[9]);

  int rc;
  vstr s1 = vstr_NULL, s2 = vstr_NULL;

  rc = vstr_append(&s1, "Happy banana!"), assert(rc == 0);
  rc = vstr_append(&s2, "Slappy bunny."), assert(rc == 0);
  rc = vstr_insertn(&s1, 6, vstr_get(&s2), vstr_len(&s2)), assert(rc == 0);

  printf("s1 = %.*s\n", (int) vstr_len(&s1), vstr_get(&s1));

  rc = vstr_elide(&s1, 4, 5), assert(rc == 0);

  printf("s1 = %.*s\n", (int) vstr_len(&s1), vstr_get(&s1));

  rc = vstr_finsert(&s1, 8, "conk %ls here ", L"Degenerate"), assert(rc == 0);

  printf("s1 = %.*s\n", (int) vstr_len(&s1), vstr_get(&s1));

  rc = vstr_finsert(&s1, 100, "flangey"), assert(rc == 0);

  printf("s1 = %.*s\n", (int) vstr_len(&s1), vstr_get(&s1));
  assert(vstr_get(&s1)[vstr_len(&s1) - 1] != '\0');

  return 0;
}
