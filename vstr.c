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
#include <wchar.h>

#include "ddslib/vstr.h"

int vstr_wcsmblen(const wchar_t *s, size_t len)
{
  if (!s) return 0;
  static const mbstate_t null;
  mbstate_t state = null;
  char buf[MB_CUR_MAX];
  int res = 0;
  while (len > 0) {
    size_t rc = wcrtomb(buf, *s, &state);
    if (rc == (size_t) -1)
      return -1;
    res += rc;
    len--;
    s++;
  }
  return res;
}

void vstr_reset(vstr *p)
{
  free(p->base);
  p->base = NULL;
  p->cap = p->len = 0;
}

static int setcap(vstr *p, size_t nc)
{
  if (!nc) nc = 1;
  void *np = realloc(p->base, nc);
  if (!np) return -1;
  p->base = np;
  p->cap = nc;
  return 0;
}

void vstr_compact(vstr *p)
{
  if (p->cap > p->len)
    setcap(p, p->len);
}

char *vstr_splice(vstr *p, size_t index, size_t n)
{
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    if (setcap(p, nc) < 0) return NULL;
  }
  if (index > p->len) index = p->len;
  size_t end = index + n;
  assert(p->len >= index);
  size_t rem = p->len - index;
  memmove(p->base + end, p->base + index, rem);
  p->len += n;
  return p->base + index;
}

void vstr_elide(vstr *p, size_t index, size_t n)
{
  if (index >= p->len) return;
  if (index + n > p->len) n = p->len - index;
  size_t end = index + n;
  size_t rem = p->len - end;
  memmove(p->base + index, p->base + end, rem);
  p->len -= n;
  if (p->len < p->cap / 4u) {
    size_t nc = (p->len + 3u) / 4u * 5u;
    assert(nc >= p->len);
    assert(setcap(p, nc));
  }
}

int vstr_setcap(vstr *p, size_t nc)
{
  if (nc < p->len) return -1;
  return setcap(p, nc);
}

int vstr_ensure(vstr *p, size_t cap)
{
  if (cap <= p->cap) return 0;
  return vstr_setcap(p, cap);
}

void vstr_clear(vstr *p)
     vstr_INLINEBODY
(
{
  p->len = 0;
}
);

int vstr_vinsertf(vstr *p, size_t index, const char *fmt, va_list ap)
{
  if (index > p->len) index = p->len;

  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vsnprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  if (req < 0) return -1;

  // Do we need to allocate an extra byte at the end for the '\0'
  // which printf will add?
  int gap = index >= p->len;

  // Try to make that space available.
  char *pos = vstr_splice(p, index, req + gap);
  if (!pos) return -1;

  char old = '\0';
  if (!gap)
    old = p->base[index + req];

  // Now write the characters in.
  int rc = vsnprintf(pos, req + 1, fmt, ap);
  assert(rc >= 0);

  if (gap)
    // Remove the trailing '\0'.
    vstr_elide(p, p->len - 1, 1);
  else
    // Restore the original character overwritten by the '\0'.
    p->base[index + req] = old;
  return 0;
}

int vstr_insertf(vstr *p, size_t index, const char *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vstr_vinsertf(p, index, fmt, ap);
  va_end(ap);
  return rc;
}

int vstr_appendf(vstr *p, const char *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vstr_vappendf(p, fmt, ap);
  va_end(ap);
  return rc;
}

int vstr_empty(vstr *p)
{
  vstr_clear(p);
  return p->base ? setcap(p, 1) : 0;
}

int vstr_insertn(vstr *p, size_t index, const char *s, size_t n)
{
  if (!s || !n) return 0;
  char *pos = vstr_splice(p, index, n);
  if (!pos) return -1;
  assert(pos - p->base >= 0);
  assert((size_t) (pos - p->base) <= p->cap);
  assert((size_t) (pos - p->base) + n <= p->cap);
  memcpy(pos, s, n);
  return 0;
}

int vstr_insertvin(vstr *p, size_t index,
		   const vstr *q, size_t qi, size_t qn)
{
  if (qi >= vstr_len(q)) return 0;
  if (qi + qn > vstr_len(q)) qn = vstr_len(q) - qi;
  return vstr_insertn(p, index, vstr_get(q) + qi, qn);
}

int vstr_insertvrn(vstr *p, size_t index,
		   const vstr *q, size_t qi, size_t qn)
{
  if (qi >= vstr_len(q)) return 0;
  if (qi + qn > vstr_len(q)) qn = vstr_len(q) - qi;
  qi = vstr_len(q) - qi - qn;
  return vstr_insertn(p, index, vstr_get(q) + qi, qn);
}

int vstr_insertvi(vstr *p, size_t index, const vstr *q, size_t qi)
{
  if (qi >= vstr_len(q)) return 0;
  return vstr_insertvin(p, index, q, qi, vstr_len(q) - qi);
}

int vstr_insertvn(vstr *p, size_t index, const vstr *q, size_t qn)
{
  if (qn > vstr_len(q)) qn = vstr_len(q);
  return vstr_insertvin(p, index, q, 0, qn);
}

int vstr_insertvr(vstr *p, size_t index, const vstr *q, size_t qi)
{
  if (qi > vstr_len(q)) return 0;
  qi = vstr_len(q) - qi;
  return vstr_insertn(p, index, vstr_get(q), qi);
}

int vstr_insertv(vstr *p, size_t index, const vstr *q)
{
  return vstr_insertn(p, index, vstr_get(q), vstr_len(q));
}

int vstr_term(vstr *p)
{
  if (!p->base) return 0;
  if (p->len == 0 || p->base[p->len - 1] != '\0')
    if (vstr_appendn(p, "", 1) < 0) return -1;
  return 0;
}

int vstr_unterm(vstr *p)
{
  if (p->len == 0 || p->base[p->len - 1] != '\0')
    return 0;
  vstr_elide(p, p->len - 1, 1);
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

extern int vstr_vappendf(vstr *p, const char *fmt, va_list ap)
     vstr_INLINEBODY
(
{ return vstr_vinsertf(p, vstr_len(p), fmt, ap); }
);

int vstr_appendvin(vstr *p, const vstr *q, size_t qi, size_t qn)
{
  return vstr_insertvin(p, vstr_len(p), q, qi, qn);
}

int vstr_appendvrn(vstr *p, const vstr *q, size_t qi, size_t qn)
{
  return vstr_insertvrn(p, vstr_len(p), q, qi, qn);
}

int vstr_appendvi(vstr *p, const vstr *q, size_t qi)
{
  return vstr_insertvi(p, vstr_len(p), q, qi);
}

int vstr_appendvn(vstr *p, const vstr *q, size_t qn)
{
  return vstr_insertvn(p, vstr_len(p), q, qn);
}

int vstr_appendvr(vstr *p, const vstr *q, size_t qi)
{
  return vstr_insertvr(p, vstr_len(p), q, qi);
}

int vstr_appendv(vstr *p, const vstr *q)
{
  return vstr_insertv(p, vstr_len(p), q);
}
