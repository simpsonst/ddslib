/*
    DDSLib: Dynamic data structures
    Copyright (C) 2002-3,2005-6,2012  Steven Simpson

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
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#include "ddslib/vstr.h"


/*** Internal functions that don't do any range checking ***/

/* Set the capacity to an exact size. */
static int setcap(vstr *p, size_t nc)
{
  assert(p->len <= nc);
  if (!nc) nc = 1;
  void *np = realloc(p->base, nc);
  if (!np) return -1;
  p->base = np;
  p->cap = nc;
  return 0;
}

/* Set the contents to an array of known length, increasing the
   capacity if necessary. */
static int set(vstr *p, const char *s, size_t n)
{
  assert(s != NULL);
  if (vstr_ensure(p, n) < 0) return -1;
  memcpy(p->base, s, n);
  p->len = n;
  return 0;
}

/* Insert space for n elements starting at x from start.  Return a pointer to
   the first new element. */
static char *splice(vstr *p, size_t x, size_t n)
{
  assert(x <= p->len);

  /* Determine whether greater capacity is required. */
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    if (setcap(p, nc) < 0) return NULL;
  }

  /* Move the tail to the new end position. */
  const size_t end = x + n;
  const size_t rem = p->len - x;
  memmove(p->base + end, p->base + x, rem);
  p->len += n;

  return p->base + x;
}

/* Reduce the capacity if the length is much smaller. */
static void reduce_cap(vstr *p)
{
  if (p->len < p->cap / 4u) {
    size_t nc = (p->len + 3u) / 4u * 5u;
    if (nc <= p->cap) {
      int rc = setcap(p, nc);
      assert(rc == 0);
    }
  }
}

/* Remove n elements starting at x from start. */
static void elide(vstr *p, size_t x, size_t n)
{
  assert(x <= p->len);
  assert(n <= p->len - x);

  /* Move the tail over the removed section. */
  size_t end = x + n;
  size_t rem = p->len - end;
  memmove(p->base + x, p->base + end, rem);
  p->len -= n;

  /* Determine whether to reduce the capacity. */
  reduce_cap(p);
}

/* Remove all elements after x from start. */
static void truncate(vstr *p, size_t x)
{
  assert(p->len >= x);

  /* Discard the tail. */
  p->len = x;

  /* Determine whether to reduce the capacity. */
  reduce_cap(p);
}

/* Remove the first x elements, and retain the next n. */
static void elect(vstr *p, size_t x, size_t n)
{
  assert(x <= p->len);
  assert(n <= p->len - x);

  memmove(p->base, p->base + x, n);
  truncate(p, n);
}




/* Normalize index and length for *_vin functions.  *xp is an index
   from the start.  *np is a length towards the end.  If *xp is beyond
   the length, return true to indicate out-of-range.  If *xp + *np is
   beyond the length, reduce it to hit the end exactly. */
static int normalize_vin(const vstr *p, size_t *xp, size_t *np)
{
  if (*xp >= p->len)
    return 1;
  if (*np > p->len - *xp)
    *np = p->len - *xp;
  return 0;
}

/* Normalize index and length for *_vrn functions.  *xp is an index
   from the end.  *np is a length towards the end.  If *xp is beyond
   the start, move it to the start, and reduce *np by a corresponding
   amount, so it effectively doesn't move.  If *np would go negative
   or zero, return true to indicate out-of-range.  Finally, if *np
   goes beyond the end, reduce it to hit the end exactly.  */
static int normalize_vrn(const vstr *p, size_t *xp, size_t *np)
{
  if (*xp >= p->len) {
    size_t diff = *xp - p->len;
    if (*np <= diff)
      return 1;
    *np -= diff;
    *xp = p->len;
  }
  if (*np > *xp)
    *np = *xp;
  return 0;
}



char *(vstr_get)(const vstr *p)
{
  return p->base;
}

size_t (vstr_len)(const vstr *p)
{
  return p->len;
}

void (vstr_clear)(vstr *p)
{
  p->len = 0;
}

void vstr_reset(vstr *p)
{
  free(p->base);
  p->base = NULL;
  p->cap = p->len = 0;
}


/* Optimize the capacity to the required size. */
void vstr_compact(vstr *p)
{
  if (p->cap > p->len) {
    int rc = setcap(p, p->len);
    assert(rc == 0);
  }
}







int vstr_setc(vstr *p, int c, size_t n)
{
  if (vstr_ensure(p, n) < 0) return -1;
  memset(p->base, c, n);
  p->len = n;
  return 0;
}

int vstr_setn(vstr *p, const char *s, size_t n)
{
  if (s) {
    return set(p, s, n);
  } else {
    vstr_reset(p);
    return 0;
  }
}

int vstr_setv(vstr *to, const vstr *from)
{
  return vstr_setn(to, from->base, from->len);
}

int vstr_setvin(vstr *p, const vstr *q, size_t qx, size_t qn)
{
  if (q->base) {
    if (normalize_vin(q, &qx, &qn))
      return set(p, "", 0);
    else
      return set(p, q->base + qx, qn);
  } else {
    vstr_reset(p);
    return 0;
  }
}

int vstr_setvi(vstr *p, const vstr *q, size_t qx)
{
  return vstr_setvin(p, q, qx, q->len - qx);
}

int vstr_setvn(vstr *p, const vstr *q, size_t qn)
{
  return vstr_setvin(p, q, 0, qn);
}

int vstr_setvr(vstr *p, const vstr *q, size_t qx)
{
  if (q->base) {
    if (qx >= q->len)
      return set(p, q->base, q->len);
    else
      return set(p, q->base + q->len - qx, qx);
  } else {
    vstr_reset(p);
    return 0;
  }
}

int vstr_setvrn(vstr *p, const vstr *q, size_t qx, size_t qn)
{
  if (q->base) {
    if (normalize_vrn(q, &qx, &qn))
      return set(p, "", 0);
    else
      return set(p, q->base + q->len - qx, qn);
  } else {
    vstr_reset(p);
    return 0;
  }
}

int vstr_vsetf(vstr *p, const char *fmt, va_list ap)
{
  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vsnprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  if (req < 0) return -1;

  // Allocate the required space, and set the length.
  if (vstr_ensure(p, req + 1) < 0) return -1;
  p->len = req;

  // Write the characters in.
  int rc = vsnprintf(p->base, req + 1, fmt, ap);
  assert(rc >= 0);

  return 0;
}







char *vstr_splice(vstr *p, size_t x, size_t n)
{
  if (x > p->len) x = p->len;
  return splice(p, x, n);
}

char *vstr_rsplice(vstr *p, size_t x, size_t n)
{
  char *r = vstr_splicer(p, x, n);
  return r ? r + n : NULL;
}

char *vstr_splicer(vstr *p, size_t x, size_t n)
{
  if (x > p->len) x = p->len;
  return splice(p, p->len - x, n);
}

char *vstr_rsplicer(vstr *p, size_t x, size_t n)
{
  char *r = vstr_splice(p, x, n);
  return r ? r + n : NULL;
}





void vstr_truncate(vstr *p, size_t x)
{
  if (p->base && x < p->len)
    truncate(p, x);
}

void vstr_rtruncate(vstr *p, size_t x)
{
  if (p->base)
    truncate(p, x < p->len ? p->len - x : 0);
}

void vstr_neck(vstr *p, size_t x)
{
  if (x < p->len)
    elide(p, 0, x);
  else
    vstr_clear(p);
}

void vstr_rneck(vstr *p, size_t x)
{
  if (x < p->len)
    elide(p, 0, p->len - x);
  else
    vstr_clear(p);
}




char *vstr_extract(vstr *p)
{
  vstr_compact(p);
  char *r = p->base;
  p->len = p->cap = 0;
  p->base = NULL;
  return r;
}

int vstr_setcap(vstr *p, size_t ncap)
{
  if (ncap < p->len) return -1;
  return setcap(p, ncap);
}

int vstr_ensure(vstr *p, size_t cap)
{
  if (cap <= p->cap) return 0;
  return vstr_setcap(p, cap);
}

int vstr_vinsertf(vstr *p, size_t x, const char *fmt, va_list ap)
{
  if (x > p->len) x = p->len;

  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vsnprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  if (req < 0) return -1;

  // Do we need to allocate an extra byte at the end for the '\0'
  // which printf will add?
  int gap = x >= p->len;

  // Try to make that space available.
  char *pos = vstr_splice(p, x, req + gap);
  if (!pos) return -1;

  char old = '\0';
  if (!gap)
    old = p->base[x + req];

  // Now write the characters in.
  int rc = vsnprintf(pos, req + 1, fmt, ap);
  assert(rc >= 0);

  if (gap)
    // Remove the trailing '\0'.
    vstr_elide(p, p->len - 1, 1);
  else
    // Restore the original character overwritten by the '\0'.
    p->base[x + req] = old;
  return 0;
}

int vstr_empty(vstr *p)
{
  vstr_clear(p);
  return p->base ? setcap(p, 1) : 0;
}

int vstr_insertc(vstr *p, size_t x, int c, size_t n)
{
  if (!n) return 0;
  char *pos = vstr_splice(p, x, n);
  if (!pos) return -1;
  assert(pos - p->base >= 0);
  assert((size_t) (pos - p->base) <= p->cap);
  assert((size_t) (pos - p->base) + n <= p->cap);
  memset(pos, c, n);
  return 0;
}

int vstr_insertn(vstr *p, size_t x, const char *s, size_t n)
{
  if (!s || !n) return 0;
  char *pos = vstr_splice(p, x, n);
  if (!pos) return -1;
  assert(pos - p->base >= 0);
  assert((size_t) (pos - p->base) <= p->cap);
  assert((size_t) (pos - p->base) + n <= p->cap);
  memcpy(pos, s, n);
  return 0;
}

int vstr_insertvin(vstr *p, size_t x,
		   const vstr *q, size_t qx, size_t qn)
{
  if (qx >= vstr_len(q)) return 0;
  if (qx + qn > vstr_len(q)) qn = vstr_len(q) - qx;
  return vstr_insertn(p, x, vstr_get(q) + qx, qn);
}

int vstr_insertvrn(vstr *p, size_t x,
		   const vstr *q, size_t qx, size_t qn)
{
  if (qx >= vstr_len(q)) return 0;
  if (qx + qn > vstr_len(q)) qn = vstr_len(q) - qx;
  qx = vstr_len(q) - qx - qn;
  return vstr_insertn(p, x, vstr_get(q) + qx, qn);
}

int vstr_insertvi(vstr *p, size_t x, const vstr *q, size_t qx)
{
  if (qx >= vstr_len(q)) return 0;
  return vstr_insertvin(p, x, q, qx, vstr_len(q) - qx);
}

int vstr_insertvn(vstr *p, size_t x, const vstr *q, size_t qn)
{
  if (qn > vstr_len(q)) qn = vstr_len(q);
  return vstr_insertvin(p, x, q, 0, qn);
}

int vstr_insertvr(vstr *p, size_t x, const vstr *q, size_t qx)
{
  if (qx > vstr_len(q)) return 0;
  qx = vstr_len(q) - qx;
  return vstr_insertn(p, x, vstr_get(q), qx);
}

int vstr_insertv(vstr *p, size_t x, const vstr *q)
{
  return vstr_insertn(p, x, vstr_get(q), vstr_len(q));
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
  truncate(p, p->len - 1);
  return 0;
}



int vstr_appendn(vstr *p, const char *s, size_t n)
{
  return vstr_insertn(p, vstr_len(p), s, n);
}

int vstr_appendc(vstr *p, int c, size_t n)
{
  return vstr_insertc(p, vstr_len(p), c, n);
}

int vstr_vappendf(vstr *p, const char *fmt, va_list ap)
{
  return vstr_vinsertf(p, vstr_len(p), fmt, ap);
}

int vstr_appendvin(vstr *p, const vstr *q, size_t qx, size_t qn)
{
  return vstr_insertvin(p, vstr_len(p), q, qx, qn);
}

int vstr_appendvrn(vstr *p, const vstr *q, size_t qx, size_t qn)
{
  return vstr_insertvrn(p, vstr_len(p), q, qx, qn);
}

int vstr_appendvi(vstr *p, const vstr *q, size_t qx)
{
  return vstr_insertvi(p, vstr_len(p), q, qx);
}

int vstr_appendvn(vstr *p, const vstr *q, size_t qn)
{
  return vstr_insertvn(p, vstr_len(p), q, qn);
}

int vstr_appendvr(vstr *p, const vstr *q, size_t qx)
{
  return vstr_insertvr(p, vstr_len(p), q, qx);
}

int vstr_appendv(vstr *p, const vstr *q)
{
  return vstr_insertv(p, vstr_len(p), q);
}




void vstr_elide(vstr *p, size_t x, size_t n)
{
  if (x >= p->len) return;
  if (n > p->len - x) n = p->len - x;
  elide(p, x, n);
}

void vstr_relide(vstr *p, size_t x, size_t n)
{
  if (x >= p->len) return;
  if (n > p->len - x) n = p->len - x;
  elide(p, p->len - x - n, n);
}

void vstr_elider(vstr *p, size_t x, size_t n)
{
  if (normalize_vrn(p, &x, &n)) return;
  elide(p, p->len - x, n);
}

void vstr_relider(vstr *p, size_t x, size_t n)
{
  if (normalize_vrn(p, &x, &n)) return;
  elide(p, x - n, n);
}


void vstr_elect(vstr *p, size_t x, size_t n)
{
  if (x >= p->len) {
    vstr_clear(p);
    return;
  }
  if (n > p->len - x) n = p->len - x;
  elect(p, x, n);
}

void vstr_relect(vstr *p, size_t x, size_t n)
{
  if (x >= p->len) {
    vstr_clear(p);
    return;
  }
  if (n > p->len - x) n = p->len - x;
  elect(p, p->len - x - n, n);
}

void vstr_electr(vstr *p, size_t x, size_t n)
{
  if (normalize_vrn(p, &x, &n)) {
    vstr_clear(p);
    return;
  }
  elect(p, p->len - x, n);
}

void vstr_relectr(vstr *p, size_t x, size_t n)
{
  if (normalize_vrn(p, &x, &n)) {
    vstr_clear(p);
    return;
  }
  elect(p, x - n, n);
}





/* Functions taking a null-terminated string */

int vstr_insert(vstr *p, size_t x, const char *s)
{
  return s ? vstr_insertn(p, x, s, strlen(s)) : 0;
}

int vstr_insert0(vstr *p, size_t x, const char *s)
{
  return s ? vstr_insertn(p, x, s, strlen(s) + 1) : 0;
}

int vstr_append(vstr *p, const char *s)
{
  return s ? vstr_appendn(p, s, strlen(s)) : 0;
}

int vstr_append0(vstr *p, const char *s)
{
  return s ? vstr_appendn(p, s, strlen(s) + 1) : 0;
}

int vstr_set(vstr *p, const char *s)
{
  if (s) {
    return vstr_setn(p, s, strlen(s));
  } else {
    vstr_reset(p);
    return 0;
  }
}

int vstr_set0(vstr *p, const char *s)
{
  if (s) {
    return vstr_setn(p, s, strlen(s) + 1);
  } else {
    vstr_reset(p);
    return 0;
  }
}





/* Varargs functions built from their v~ counterparts */

int vstr_insertf(vstr *p, size_t x, const char *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vstr_vinsertf(p, x, fmt, ap);
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

int vstr_setf(vstr *p, const char *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vstr_vsetf(p, fmt, ap);
  va_end(ap);
  return rc;
}




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

  // Return to initial shift state.
  res += wcrtomb(NULL, L'\0', &state) - 1;
  return res;
}
