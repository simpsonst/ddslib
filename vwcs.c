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
    void *np = realloc(p->base, p->len * sizeof(wchar_t));
    if (!np) return;
    p->base = np;
    p->cap = p->len;
  }
}

wchar_t *vwcs_splice(vwcs *p, size_t index, size_t n)
{
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    void *np = realloc(p->base, nc * sizeof(wchar_t));
    if (!np) return NULL;
    p->base = np;
    p->cap = nc;
  }
  if (index > p->len) index = p->len;
  size_t end = index + n;
  size_t rem = p->len - end;
  wmemmove(p->base + end, p->base + index, rem);
  p->len += n;
  return p->base + index;
}

int vwcs_vfinsert(vwcs *p, size_t index, const wchar_t *fmt, va_list ap)
{
  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vswprintf(NULL, 0, fmt, ap2);
  va_end(ap2);

  // Try to make that space available.
  wchar_t *pos = vwcs_splice(p, index, req);
  if (!pos) return -1;

  // Now write the characters in.
  vswprintf(pos, req, fmt, ap);
  return 0;
}

int vwcs_finsert(vwcs *p, size_t index, const wchar_t *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vwcs_vfinsert(p, index, fmt, ap);
  va_end(ap);
  return rc;
}

int vwcs_fappend(vwcs *p, const wchar_t *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vwcs_vfappend(p, fmt, ap);
  va_end(ap);
  return rc;
}

int vwcs_insertn(vwcs *p, size_t index, const wchar_t *s, size_t n)
{
  wchar_t *pos = vwcs_splice(p, index, n);
  if (!pos) return -1;
  wmemcpy(pos, s, n);
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
    void *np = realloc(p->base, nc * sizeof(wchar_t));
    if (np) {
      p->base = np;
      p->cap = nc;
    }
  }
  return 0;
}

int vwcs_term(vwcs *p)
{
  if (!p->base) return 0;
  if (p->len == 0 || p->base[p->len - 1] != L'\0')
    if (vwcs_appendn(p, L"", 1) < 0) return -1;
  return 0;
}

extern wchar_t *vwcs_get(const vwcs *p)
     vwcs_INLINEBODY
(
{ return p->base; }
);

extern size_t vwcs_len(const vwcs *p)
     vwcs_INLINEBODY
(
{ return p->len; }
);

extern int vwcs_insert(vwcs *p, size_t index, const wchar_t *s)
     vwcs_INLINEBODY
(
{ return vwcs_insertn(p, index, s, wcslen(s)); }
);

extern int vwcs_appendn(vwcs *p, const wchar_t *s, size_t n)
     vwcs_INLINEBODY
(
{ return vwcs_insertn(p, vwcs_len(p), s, n); }
);

extern int vwcs_append(vwcs *p, const wchar_t *s)
     vwcs_INLINEBODY
(
{ return vwcs_appendn(p, s, wcslen(s)); }
);

extern int vwcs_vfappend(vwcs *p, const wchar_t *fmt, va_list ap)
     vwcs_INLINEBODY
(
{ return vwcs_vfinsert(p, vwcs_len(p), fmt, ap); }
);
