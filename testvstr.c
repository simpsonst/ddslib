#include <stdio.h>

#include "ddslib/vstr.h"

int main()
{
  vstr s1 = vstr_NULL, s2 = vstr_NULL;

  vstr_append(&s1, "Happy banana!");
  vstr_append(&s2, "Slappy bunny.");
  vstr_insert(&s1, 6, vstr_get(&s2));

  printf("s1 = %.*s\n", (int) vstr_len(&s1), vstr_get(&s1));
  return 0;
}
