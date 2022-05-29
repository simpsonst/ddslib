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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ddslib/htab.h"

static void tass(htab t, const char *key, const char *val)
{
  const char *actual = htab_getss(t, key);
  int rc = strcmp(actual, val);
  if (rc != 0)
    printf("Test failed: %s yielded %s, not %s\n", key, actual, val);
}

int main(int argc, const char *const *argv)
{
  htab table;

  table = htab_open(1499, NULL,
                    &htab_hash_str,
                    &htab_cmp_str,
                    &htab_copy_str,
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

  tass(table, "key-3", "value-3.1");
  tass(table, "key-5", "value-5.1");

  htab_putss(table, "key-4", "value-4.2");
  htab_putss(table, "key-2", "value-2.2");
  htab_putss(table, "key-3", "value-3.2");

  tass(table, "key-2", "value-2.2");
  tass(table, "key-3", "value-3.2");
  tass(table, "key-4", "value-4.2");
  tass(table, "key-5", "value-5.1");

  printf("All tests complete.\n");
  htab_close(table);
  return EXIT_SUCCESS;
}
