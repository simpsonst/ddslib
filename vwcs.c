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

void vwcs_reset(vwcs *p)
{
  free(p->base);
  p->base = NULL;
  p->cap = p->len = 0;
}

static int setcap(vwcs *p, size_t nc)
{
  void *np = realloc(p->base, nc * sizeof(wchar_t));
  if (!np) return -1;
  p->base = np;
  p->cap = nc;
  return 0;
}

void vwcs_compact(vwcs *p)
{
  if (p->cap > p->len)
    setcap(p, p->len);
}

wchar_t *vwcs_splice(vwcs *p, size_t index, size_t n)
{
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    if (setcap(p, nc) < 0) return NULL;
  }
  if (index > p->len) index = p->len;
  size_t end = index + n;
  size_t rem = p->len - index;
  wmemmove(p->base + end, p->base + index, rem);
  p->len += n;
  return p->base + index;
}

void vwcs_elide(vwcs *p, size_t index, size_t n)
{
  if (index >= p->len) return;
  if (index + n > p->len) n = p->len - index;
  size_t end = index + n;
  size_t rem = p->len - end;
  wmemmove(p->base + index, p->base + end, rem);
  p->len -= n;
  if (p->len < p->cap / 4u) {
    size_t nc = (p->len + 3u) / 4u * 5u;
    assert(nc >= p->len);
    assert(setcap(p, nc));
  }
}

int vwcs_setcap(vwcs *p, size_t nc)
{
  if (nc < p->len) return -1;
  return setcap(p, nc);
}

int vwcs_ensure(vwcs *p, size_t cap)
{
  if (cap <= p->cap) return 0;
  return vwcs_setcap(p, cap);
}

void vwcs_cancel(vwcs *p)
     vwcs_INLINEBODY
(
{
  p->len = 0;
}
);

int vwcs_vinsertf(vwcs *p, size_t index, const wchar_t *fmt, va_list ap)
{
  if (index > p->len) index = p->len;

  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vswprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  if (req >= 0) {
    // Do we need to allocate an extra byte at the end for the L'\0'
    // which wprintf will add?
    int gap = index >= p->len;

    // Try to make that space available.
    wchar_t *pos = vwcs_splice(p, index, req + gap);
    if (pos) {
      wchar_t old = '\0';
      if (!gap)
	old = p->base[index + req];

      // Now write the characters in.
      int rc = vswprintf(pos, req + 1, fmt, ap);
      assert(rc >= 0);

      if (gap)
	// Remove the trailing L'\0'.
	vwcs_elide(p, p->len - 1, 1);
      else
	// Restore the original character overwritten by the L'\0'.
	p->base[index + req] = old;
      req = 0;
    } else
      req = -1;
  }
  va_end(ap);
  return req;
}

int vwcs_insertf(vwcs *p, size_t index, const wchar_t *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vwcs_vinsertf(p, index, fmt, ap);
  va_end(ap);
  return rc;
}

int vwcs_appendf(vwcs *p, const wchar_t *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vwcs_vappendf(p, fmt, ap);
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

int vwcs_insertvin(vwcs *p, size_t index,
		   const vwcs *q, size_t qi, size_t qn)
{
  if (qi >= vwcs_len(q)) return 0;
  if (qi + qn > vwcs_len(q)) qn = vwcs_len(q) - qi;
  return vwcs_insertn(p, index, vwcs_get(q) + qi, qn);
}

int vwcs_insertvrn(vwcs *p, size_t index,
		   const vwcs *q, size_t qi, size_t qn)
{
  if (qi >= vwcs_len(q)) return 0;
  if (qi + qn > vwcs_len(q)) qn = vwcs_len(q) - qi;
  qi = vwcs_len(q) - qi - qn;
  return vwcs_insertn(p, index, vwcs_get(q) + qi, qn);
}

int vwcs_insertvi(vwcs *p, size_t index, const vwcs *q, size_t qi)
{
  if (qi >= vwcs_len(q)) return 0;
  return vwcs_insertvin(p, index, q, qi, vwcs_len(q) - qi);
}

int vwcs_insertvn(vwcs *p, size_t index, const vwcs *q, size_t qn)
{
  if (qn > vwcs_len(q)) qn = vwcs_len(q);
  return vwcs_insertvin(p, index, q, 0, qn);
}

int vwcs_insertvr(vwcs *p, size_t index, const vwcs *q, size_t qi)
{
  if (qi > vwcs_len(q)) return 0;
  qi = vwcs_len(q) - qi;
  return vwcs_insertn(p, index, vwcs_get(q), qi);
}

int vwcs_insertv(vwcs *p, size_t index, const vwcs *q)
{
  return vwcs_insertn(p, index, vwcs_get(q), vwcs_len(q));
}

int vwcs_term(vwcs *p)
{
  if (!p->base) return 0;
  if (p->len == 0 || p->base[p->len - 1] != L'\0')
    if (vwcs_appendn(p, L"", 1) < 0) return -1;
  return 0;
}

int vwcs_unterm(vwcs *p)
{
  if (!p->base || p->len == 0 || p->base[p->len - 1] != L'\0')
    return 0;
  vwcs_elide(p, p->len - 1, 1);
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

extern int vwcs_vappendf(vwcs *p, const wchar_t *fmt, va_list ap)
     vwcs_INLINEBODY
(
{ return vwcs_vinsertf(p, vwcs_len(p), fmt, ap); }
);

int vwcs_appendvin(vwcs *p, const vwcs *q, size_t qi, size_t qn)
{
  return vwcs_insertvin(p, vwcs_len(p), q, qi, qn);
}

int vwcs_appendvrn(vwcs *p, const vwcs *q, size_t qi, size_t qn)
{
  return vwcs_insertvrn(p, vwcs_len(p), q, qi, qn);
}

int vwcs_appendvi(vwcs *p, const vwcs *q, size_t qi)
{
  return vwcs_insertvi(p, vwcs_len(p), q, qi);
}

int vwcs_appendvn(vwcs *p, const vwcs *q, size_t qn)
{
  return vwcs_insertvn(p, vwcs_len(p), q, qn);
}

int vwcs_appendvr(vwcs *p, const vwcs *q, size_t qi)
{
  return vwcs_insertvr(p, vwcs_len(p), q, qi);
}

int vwcs_appendv(vwcs *p, const vwcs *q)
{
  return vwcs_insertv(p, vwcs_len(p), q);
}
