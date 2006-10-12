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
#include <stdio.h>

#include "ddslib/vstr.h"

void vstr_cancel(vstr *p)
{
  free(p->base);
  p->base = NULL;
  p->cap = p->len = 0;
}

void vstr_compact(vstr *p)
{
  if (p->cap > p->len) {
    void *np = realloc(p->base, p->len);
    if (!np) return;
    p->base = np;
    p->cap = p->len;
  }
}

char *vstr_splice(vstr *p, size_t index, size_t n)
{
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    void *np = realloc(p->base, nc);
    if (!np) return NULL;
    p->base = np;
    p->cap = nc;
  }
  if (index > p->len) index = p->len;
  size_t end = index + n;
  assert(p->len >= index);
  size_t rem = p->len - index;
  memmove(p->base + end, p->base + index, rem);
  p->len += n;
  return p->base + index;
}

int vstr_vfinsert(vstr *p, size_t index, const char *fmt, va_list ap)
{
  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vsnprintf(NULL, 0, fmt, ap2);
  va_end(ap2);

  // Try to make that space available.
  char *pos = vstr_splice(p, index, req);
  if (!pos) return -1;

  // Now write the characters in.
  vsnprintf(pos, req, fmt, ap);
  return 0;
}

int vstr_finsert(vstr *p, size_t index, const char *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vstr_vfinsert(p, index, fmt, ap);
  va_end(ap);
  return rc;
}

int vstr_fappend(vstr *p, const char *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vstr_vfappend(p, fmt, ap);
  va_end(ap);
  return rc;
}

int vstr_insertn(vstr *p, size_t index, const char *s, size_t n)
{
  printf("%s %d\n", __FILE__, __LINE__), fflush(stdout);
  char *pos = vstr_splice(p, index, n);
  printf("%s %d\n", __FILE__, __LINE__), fflush(stdout);
  if (!pos) return -1;
  memcpy(pos, s, n);
  return 0;
}

int vstr_elide(vstr *p, size_t index, size_t n)
{
  if (index >= p->len) return 0;
  if (index + n > p->len) n = p->len - index;
  size_t end = index + n;
  size_t rem = p->len - end;
  memmove(p->base + index, p->base + end, rem);
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

int vstr_term(vstr *p)
{
  if (!p->base) return 0;
  if (p->len == 0 || p->base[p->len - 1] != '\0')
    if (vstr_appendn(p, "", 1) < 0) return -1;
  return 0;
}

extern char *vstr_get(const vstr *p)
     vstr_INLINEBODY
(
{ return p->base; }
);

extern size_t vstr_len(const vstr *p)
     vstr_INLINEBODY
(
{ return p->len; }
);

extern int vstr_insert(vstr *p, size_t index, const char *s)
     vstr_INLINEBODY
(
{ return vstr_insertn(p, index, s, strlen(s)); }
);

extern int vstr_appendn(vstr *p, const char *s, size_t n)
     vstr_INLINEBODY
(
{ return vstr_insertn(p, vstr_len(p), s, n); }
);

extern int vstr_append(vstr *p, const char *s)
     vstr_INLINEBODY
(
{ return vstr_appendn(p, s, strlen(s)); }
);

extern int vstr_vfappend(vstr *p, const char *fmt, va_list ap)
     vstr_INLINEBODY
(
{ return vstr_vfinsert(p, vstr_len(p), fmt, ap); }
);
