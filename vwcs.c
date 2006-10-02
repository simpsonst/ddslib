/*
    DDSLib: Dynamic data structures
    Copyright (C) 2006  Steven Simpson

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

#include <assert.h>
#include <stdlib.h>

#include "ddslib/vwcs.h"

void vwcs_cancel(vwcs *p)
{
  free(p->base);
  p->base = NULL;
  p->cap = p->len = 0;
}

void vwcs_compact(vwcs *p)
{
  if (p->cap > p->len) {
    void *np = realloc(p->base, p->len);
    if (!np) return;
    p->base = np;
    p->cap = p->len;
  }
}

int vwcs_insertn(vwcs *p, size_t index, const wchar_t *s, size_t n)
{
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    void *np = realloc(p->base, nc);
    if (!np) return -1;
    p->base = np;
    p->cap = nc;
  }
  if (index > p->len) index = p->len;
  size_t end = index + n;
  size_t rem = p->len - end;
  wmemmove(p->base + end, p->base + index, rem);
  wmemcpy(p->base + p->len, s, n);
  p->len += n;
  return 0;
}

int vwcs_elide(vwcs *p, size_t index, size_t n)
{
  if (index >= p->len) return 0;
  if (index + n > p->len) n = p->len - index;
  size_t end = index + n;
  size_t rem = p->len - end;
  wmemmove(p->base + index, p->base + end, rem);
  p->len -= n;
  if (p->len < p->cap / 4u) {
    size_t nc = (p->len + 3u) / 4u * 5u;
    assert(nc >= p->len);
    void *np = realloc(p->base, nc);
    if (np) {
      p->base = np;
      p->cap = nc;
    }
  }
  return 0;
}

extern wchar_t *vwcs_get(const vwcs *);
extern size_t vwcs_len(const vwcs *);
extern int vwcs_insert(vwcs *p, size_t index, const wchar_t *s);
extern int vwcs_appendn(vwcs *p, const wchar_t *s, size_t n);
extern int vwcs_append0(vwcs *p, const wchar_t *s);
