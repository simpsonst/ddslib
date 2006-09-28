#include <stdio.h>
#include <stdlib.h>

#include "ddslib/htab.h"

int main(int argc, const char *const *argv)
{
  htab table;

  table = htab_open(1499, NULL,
		    &htab_hash_str,
		    &htab_cmp_str,
		    &htab_copy_str,
		    &htab_release_free,
		    &htab_release_free);
  if (table == NULL) {
    fprintf(stderr, "Could not open table.\n");
    exit(EXIT_FAILURE);
  }

  htab_putss(table, "key-1", "value-1.1");
  htab_putss(table, "key-2", "value-2.1");
  htab_putss(table, "key-3", "value-3.1");
  htab_putss(table, "key-4", "value-4.1");
  htab_putss(table, "key-5", "value-5.1");

  return EXIT_SUCCESS;
}
