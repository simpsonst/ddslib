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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "ddslib/bheap.h"

struct mystr {
  bheap_elem links;
  int val;
};

int mycmp(void *n, const void *v1, const void *v2)
{
  const struct mystr *m1 = v1, *m2 = v2;
  return m1->val - m2->val;
}

char *myprint(void *n, const void *v)
{
  static char string[20];
  const struct mystr *m = v;
  if (m)
    sprintf(string, "%d", m->val);
  else
    return "nil";
  return string;
}

int main()
{
  int lastval = 0;
  bheap heap;
  int i;
  struct mystr *p;

  srand(time(NULL));

  bheap_init(&heap, struct mystr, links, NULL, &mycmp);
  heap.print = &myprint;

  bheap_debug(&heap, 1);

  for (i = 0; i < 20; i++) {
    p = malloc(sizeof *p);
    p->val = rand() / (double) RAND_MAX * 20;
    printf("Inserting %p (%d)...\n", (void *) p, p->val);
    bheap_insert(&heap, p);
  }

  fprintf(stderr, "Insertion complete...\n");

  while ((p = bheap_pop(&heap))) {
    int toad;

    // Remove the top one.
    assert(p->val >= lastval);
    lastval = p->val;
    printf("\t\t\tRemoving %p (%d)\n", (void *) p, p->val);
    free(p);

    // Add some new ones no later than this.
    toad = rand() / (RAND_MAX + 1.0) * 2.95;
    for (i = 0; i < toad; i++) {
      p = malloc(sizeof *p);
      p->val = lastval + rand() / (double) RAND_MAX * 20;
      printf("Inserting %p (%d)...\n", (void *) p, p->val);
      bheap_insert(&heap, p);
    }
  }

  return 0;
}
