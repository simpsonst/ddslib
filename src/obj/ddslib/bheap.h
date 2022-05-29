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

#ifndef bheap_INCLUDED
#define bheap_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

  typedef struct {
    void *child[2], *parent;
    void **holder;
  } bheap_elem;

  typedef struct {
    void *first, *last;
    void *ctxt;
    int (*cmp)(void *, const void *, const void *);
    char *(*print)(void *, const void *);
    size_t memb, size;
  } bheap;

#define bheap_init(R, T, MEMB, OBJ, CMP)        \
  ((void) ((R)->first = (R)->last = 0,          \
           (R)->memb = offsetof(T, MEMB),       \
           (R)->ctxt = (OBJ),                   \
           (R)->cmp = (CMP),                    \
           (R)->size = 0u,                      \
           (R)->print = 0))

  void bheap_insert(bheap *, void *);
  void bheap_remove(bheap *, void *);
  void *bheap_pop(bheap *);
#define bheap_peek(R) ((R)->first)

  void bheap_debug(bheap *, int);

#ifdef __cplusplus
}
#endif

#endif
