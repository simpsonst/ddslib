/*
    DDSLib: Dynamic data structures
    Copyright (C) 2003,2005  Steven Simpson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


    Author contact: Email to ss@comp.lancs.ac.uk
*/

#include <stdio.h>
#include <assert.h>

#include "ddslib/bheap.h"

#define get_elem(R,O) ((bheap_elem *) &(R)->memb[(char *) (O)])

#define fix_holder(R,M,N) \
if ((M)->child[N]) get_elem((R),(M)->child[N])->holder = &(M)->child[N]

static void swap(bheap *r, void *p, void *q)
{
  bheap_elem *pm = get_elem(r, p);
  bheap_elem *qm = get_elem(r, q);
  bheap_elem tmp = *qm;

  assert(p != q);

  *qm = *pm;
  *pm = tmp;

  if (pm->parent == p) {
    qm->child[pm->holder - pm->child] = p;
    *qm->holder = q;
  } else if (qm->parent == q) {
    pm->child[qm->holder - qm->child] = q;
    *pm->holder = p;
  } else {
    *pm->holder = p;
    *qm->holder = q;
  }

  if (pm->child[0]) {
    bheap_elem *cm = get_elem(r, pm->child[0]);
    cm->holder = &pm->child[0];
    cm->parent = p;
  }

  if (pm->child[1]) {
    bheap_elem *cm = get_elem(r, pm->child[1]);
    cm->holder = &pm->child[1];
    cm->parent = p;
  }

  if (qm->child[0]) {
    bheap_elem *cm = get_elem(r, qm->child[0]);
    cm->holder = &qm->child[0];
    cm->parent = q;
  }

  if (qm->child[1]) {
    bheap_elem *cm = get_elem(r, qm->child[1]);
    cm->holder = &qm->child[1];
    cm->parent = q;
  }

  if (p == r->last)
    r->last = q;
  else if (q == r->last)
    r->last = p;
}

static int swap_with_parent(bheap *r, void *p)
{
  void *q = get_elem(r, p)->parent;
  if (!q || (*r->cmp)(r->ctxt, q, p) < 0) return 0;
  swap(r, p, q);
  return 1;
}

static int swap_with_child(bheap *r, void *p, int n)
{
  void *q = get_elem(r, p)->child[n];
  if (!q || (*r->cmp)(r->ctxt, q, p) > 0) return 0;
#ifdef DEBUG
  fprintf(stderr, "Swapping %p (%s)",
	  p, r->print ? r->print(r->ctxt, p) : "");
  fprintf(stderr, " with child %d (%p; %s)...\n", n, q,
	  r->print ? r->print(r->ctxt, q) : "");
#endif
  swap(r, p, q);
  return 1;
}

static int swap_with_children(bheap *r, void *p)
{
  bheap_elem *pm = get_elem(r, p);
  void *c = p;

  if (pm->child[0] && (*r->cmp)(r->ctxt, pm->child[0], c) <= 0)
    c = pm->child[0];
  if (pm->child[1] && (*r->cmp)(r->ctxt, pm->child[1], c) <= 0)
    c = pm->child[1];
  if (c == p)
    return 0;
  swap(r, c, p);
  return 1;
}

static unsigned fls(unsigned i)
{
  unsigned x = i;
  unsigned r = 0;
  while (i) {
    r++;
    i >>= 1;
  }
  return r;
}

static void **find_pos(bheap *r, unsigned i, void **parp)
{
  void **pp = &r->first;
  unsigned f;

  if (parp) *parp = NULL;

  for (f = fls(i); f > 1; f--) {
    unsigned b = (i >> (f - 2)) & 1u;
    if (parp) *parp = *pp;
    pp = &get_elem(r, *pp)->child[b];
  }

  return pp;
}

static void find_last(bheap *r)
{
#if 0
  r->last = *find_pos(r, r->size, NULL);
#else
  void *n = r->last, *p;
  bheap_elem *nm, *pm;
  ptrdiff_t i;

  for ( ; ; ) {
#ifdef DEBUG
    fprintf(stderr, "Seeking from %p (%s)...\n", n,
	    r->print ? r->print(r->ctxt, n) : "");
#endif
    nm = get_elem(r, n);
    p = nm->parent;
    if (!p) {
#ifdef DEBUG
      fprintf(stderr, "\tWe have no parent!\n");
#endif
      break;
    }
    pm = get_elem(r, p);
    i = nm->holder - pm->child;
    if (i) {
#ifdef DEBUG
      fprintf(stderr, "\tWe're the right child of %p (%s)!\n", p,
	      r->print ? r->print(r->ctxt, p) : "");
#endif
      n = pm->child[0];
      break;
    }
    n = p;
  }

  while (nm = get_elem(r, n), nm->child[0] || nm->child[1]) {
#ifdef DEBUG
    fprintf(stderr, "%p (%s) down %s to\n", n,
	    r->print ? r->print(r->ctxt, n) : "",
	    nm->child[1] ? "right" : "left");
#endif
    n = nm->child[!!nm->child[1]];
  }
#ifdef DEBUG
  fprintf(stderr, "%p (%s).\n", n, r->print ? r->print(r->ctxt, n) : "");
#endif

  r->last = n;
#endif
}

void bheap_insert(bheap *r, void *p)
{
  void *parent;
  void **pp = find_pos(r, ++r->size, &parent);
  bheap_elem *pm = get_elem(r, p);

  assert(*pp == NULL);
  *pp = p;
  pm->holder = pp;
  pm->child[0] = pm->child[1] = NULL;
  pm->parent = parent;

  r->last = p;

  while (swap_with_parent(r, p))
    ;
}

void bheap_remove(bheap *r, void *p)
{
  void *q = r->last;

  /* Swap the offending element with the last one. */
  if (p != q)
    swap(r, p, q);

#ifdef DEBUG
  fprintf(stderr, "\nHaving swapped the offender:\n");
  bheap_debug(r, 0);
#endif

  /* Find the new last element. */
  if (--r->size > 1)
    find_last(r);
  else if (r->size)
    r->last = r->first;
  else
    r->last = NULL;

#ifdef DEBUG
  fprintf(stderr, "\nHaving recomputed the last:\n");
  bheap_debug(r, 0);
#endif

  /* Detach the offending element. */
  *get_elem(r, p)->holder = NULL;

#ifdef DEBUG
  fprintf(stderr, "\nHaving eliminated the offender:\n");
  bheap_debug(r, 0);
#endif

  if (p != q) {
    /* Let the previous swapped element ascend. */
    while (swap_with_parent(r, q))
      ;

    /* Let the previous swapped element descend. */
    while (swap_with_children(r, q))
      ;
  }

#ifdef DEBUG
  fprintf(stderr, "\nHaving resorted the old last:\n");
  bheap_debug(r, 0);
#endif
}

void *bheap_pop(bheap *r)
{
  void *p = bheap_peek(r);
  if (p)
    bheap_remove(r, p);
  return p;
}

static void print_branch(FILE *out, bheap *r, void *elem)
{
  bheap_elem *em;

  if (!elem) return;

  em = get_elem(r, elem);
  fprintf(out, "%p (%s): parent=%p, c[0]=%p, c[1]=%p, holder=%p\n",
	  elem, r->print ? r->print(r->ctxt, elem) : "",
	  em->parent, em->child[0], em->child[1],
	  (void *) em->holder);

  print_branch(out, r, em->child[0]);
  print_branch(out, r, em->child[1]);
}

void bheap_debug(bheap *r, int flags)
{
  if (flags) {
    fprintf(stderr, "Address: %p\n", (void *) r);
    fprintf(stderr, "Member offset: %lu\n", (unsigned long) r->memb);
    fprintf(stderr, "Comparison context: %p\n", r->ctxt);
  }
  fprintf(stderr, "Size: %lu\n", (unsigned long) r->size);
  fprintf(stderr, "First: %p (%s)", r->first,
	  r->print ? r->print(r->ctxt, r->first) : "");
  fprintf(stderr, "; last: %p (%s)\n", r->last,
	  r->print ? r->print(r->ctxt, r->last) : "");
  print_branch(stderr, r, r->first);
}
