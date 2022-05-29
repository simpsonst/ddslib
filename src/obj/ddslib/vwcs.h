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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef vwcs_INCLUDED
#define vwcs_INCLUDED

#include <stddef.h>

  typedef struct {
    wchar_t *base;
    size_t cap, len;
  } vwcs;

#define vwcs_NULL { NULL, 0, 0 }

  /* Access underlying array. */
  wchar_t *(vwcs_get)(const vwcs *p);

  /* Get used length. */
  size_t (vwcs_len)(const vwcs *p);

  /* Detach and return allocated array. */
  wchar_t *vwcs_extract(vwcs *p);

  /* Set the capacity to match the length. */
  void vwcs_compact(vwcs *p);

  /* Ensure string is null-terminated. */
  int vwcs_term(vwcs *p);

  /* Ensure string is not null-terminated. */
  int vwcs_unterm(vwcs *p);

  /* Set the capacity, or fail if truncating would occur. */
  int vwcs_setcap(vwcs *p, size_t cap);

  /* Ensure minimum capacity. */
  int vwcs_ensure(vwcs *p, size_t cap);

  /* If the capacity is significantly greater than the length, reduce
     to something close but not exact. */
  void vwcs_shorten(vwcs *p);




  /* Zeroing functions */

  void (vwcs_clear)(vwcs *p); // Set non-null string to empty.
  int vwcs_empty(vwcs *p); // Set to empty.
  void vwcs_reset(vwcs *p); // Set to null.


  /* ~: Use all of null-terminated string, except terminator.

     ~0: Use all of null-terminated string, including terminator.

     ~n: Use array of known size.

     ~v: Use all of other managed string.

     ~vi: Use all but head of other managed string.

     ~vn: Use head of other managed string.

     ~vin: Use middle of other managed string.

     ~vr: Use tail of other managed string.

     ~vrn: Use middle of other managed string, reversed with
     indexing.

     ~f: Use format string and corresponding arguments.

     v~f: Use format string and argument list. */


  /* Assignment functions: Any previous value is discarded. */

  int vwcs_set(vwcs *p, const wchar_t *s);
  int vwcs_set0(vwcs *p, const wchar_t *s);
  int vwcs_setn(vwcs *p, const wchar_t *, size_t n);
  int vwcs_setc(vwcs *p, wchar_t, size_t n);
  int vwcs_setv(vwcs *p, const vwcs *q);
  int vwcs_setvi(vwcs *p, const vwcs *q, size_t qx);
  int vwcs_setvn(vwcs *p, const vwcs *q, size_t qn);
  int vwcs_setvin(vwcs *p, const vwcs *q, size_t qx, size_t qn);
  int vwcs_setvr(vwcs *p, const vwcs *q, size_t qx);
  int vwcs_setvrn(vwcs *p, const vwcs *q, size_t qx, size_t qn);
  int vwcs_setf(vwcs *p, const wchar_t *fmt, ...);




  /* Insertion functions */
  int vwcs_insert(vwcs *p, size_t x, const wchar_t *s);
  int vwcs_insert0(vwcs *p, size_t x, const wchar_t *s);
  int vwcs_insertc(vwcs *p, size_t x, wchar_t, size_t n);
  int vwcs_insertn(vwcs *p, size_t x, const wchar_t *, size_t n);
  int vwcs_insertv(vwcs *p, size_t x, const vwcs *q);
  int vwcs_insertvi(vwcs *p, size_t x, const vwcs *q, size_t qx);
  int vwcs_insertvn(vwcs *p, size_t x, const vwcs *q, size_t qn);
  int vwcs_insertvin(vwcs *p, size_t x,
                     const vwcs *q, size_t qx, size_t qn);
  int vwcs_insertvr(vwcs *p, size_t x, const vwcs *q, size_t qx);
  int vwcs_insertvrn(vwcs *p, size_t x,
                     const vwcs *q, size_t qx, size_t qn);
  int vwcs_insertf(vwcs *p, size_t x, const wchar_t *fmt, ...);


  /* Appendage functions; All of these are equivalent to their
     insertion counterparts, but with x set to the length. */
  int vwcs_append(vwcs *p, const wchar_t *s);
  int vwcs_append0(vwcs *p, const wchar_t *s);
  int vwcs_appendn(vwcs *p, const wchar_t *s, size_t n);
  int vwcs_appendc(vwcs *p, wchar_t c, size_t n);
  int vwcs_appendv(vwcs *p, const vwcs *q);
  int vwcs_appendvi(vwcs *p, const vwcs *q, size_t qx);
  int vwcs_appendvn(vwcs *p, const vwcs *q, size_t qn);
  int vwcs_appendvin(vwcs *p, const vwcs *q, size_t qx, size_t qn);
  int vwcs_appendvr(vwcs *p, const vwcs *q, size_t qx);
  int vwcs_appendvrn(vwcs *p, const vwcs *q, size_t qx, size_t qn);
  int vwcs_appendf(vwcs *p, const wchar_t *fmt, ...);


  /* splice/elide/elect functions have the following forms:

     ~: x is index from start, n counts towards end.

     r~: x is index from end, n counts towards start.

     ~r: x is index from end, n counts towards end. 

     r~r: x is index from start, n counts towards start. */



  /* Internal allocation functions */

  /* Insert space for n elements at x from start, returning pointer to
     start of new space. */
  wchar_t *vwcs_splice(vwcs *p, size_t x, size_t n);

  /* Insert space for n elements at x from end, returning pointer to
     one-past-end of new space. */
  wchar_t *vwcs_rsplice(vwcs *p, size_t x, size_t n);

  /* Insert space for n elements at x from end, returning pointer to
     start of new space. */
  wchar_t *vwcs_splicer(vwcs *p, size_t x, size_t n);

  /* Insert space for n elements at x from start, returning pointer to
     one-past-end of new space. */
  wchar_t *vwcs_rsplicer(vwcs *p, size_t x, size_t n);

  /* Remove a central piece. */
  void vwcs_elide(vwcs *p, size_t x, size_t n);
  void vwcs_relide(vwcs *p, size_t x, size_t n);
  void vwcs_elider(vwcs *p, size_t x, size_t n);
  void vwcs_relider(vwcs *p, size_t x, size_t n);

  /* Retain a central piece. */
  void vwcs_elect(vwcs *p, size_t x, size_t n);
  void vwcs_relect(vwcs *p, size_t x, size_t n);
  void vwcs_electr(vwcs *p, size_t x, size_t n);
  void vwcs_relectr(vwcs *p, size_t x, size_t n);

  /* Keep the first n characters. */
  void vwcs_truncate(vwcs *p, size_t n);

  /* Remove the last n characters. */
  void vwcs_rtruncate(vwcs *p, size_t n);

  /* Remove the first n characters. */
  void vwcs_neck(vwcs *p, size_t n);

  /* Keep the last n characters. */
  void vwcs_rneck(vwcs *p, size_t n);



#define vwcs_get(p) ((wchar_t *) (p)->base)
#define vwcs_len(p) ((size_t) (p)->len)
#define vwcs_clear(p) ((void) ((p)->len = 0))

#endif

#ifdef va_start
  /* We can afford to re-declare functions without re-inclusion
     protection.  This allows the user to include <stdarg.h> and then
     <ddslib/vwcs.h> again to access these specific functions if an
     earlier inclusion failed to declare them. */
  int vwcs_vsetf(vwcs *p, const wchar_t *fmt, va_list ap);
  int vwcs_vinsertf(vwcs *p, size_t x, const wchar_t *fmt, va_list ap);
  int vwcs_vappendf(vwcs *p, const wchar_t *fmt, va_list ap);
#endif

#ifdef __cplusplus
}
#endif
