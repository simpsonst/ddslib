#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "bheap.h"

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
