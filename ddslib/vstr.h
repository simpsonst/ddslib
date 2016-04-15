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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef vstr_INCLUDED
#define vstr_INCLUDED

#include <stddef.h>

  typedef struct {
    char *base;
    size_t cap, len;
  } vstr;

#define vstr_NULL { NULL, 0, 0 }

  /* Access underlying array. */
  char *(vstr_get)(const vstr *p);

  /* Get used length. */
  size_t (vstr_len)(const vstr *p);

  /* Detach and return allocated array. */
  char *vstr_extract(vstr *p);

  /* Set the capacity to match the length. */
  void vstr_compact(vstr *p);

  /* Ensure string is null-terminated. */
  int vstr_term(vstr *p);

  /* Ensure string is not null-terminated. */
  int vstr_unterm(vstr *p);

  /* Set the capacity, or fail if truncating would occur. */
  int vstr_setcap(vstr *p, size_t cap);

  /* Ensure minimum capacity. */
  int vstr_ensure(vstr *p, size_t cap);




  /* Zeroing functions */

  void (vstr_clear)(vstr *p); // Set non-null string to empty.
  int vstr_empty(vstr *p); // Set to empty.
  void vstr_reset(vstr *p); // Set to null.


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

  int vstr_set(vstr *p, const char *s);
  int vstr_set0(vstr *p, const char *s);
  int vstr_setn(vstr *p, const char *, size_t n);
  int vstr_setc(vstr *p, int, size_t n);
  int vstr_setv(vstr *p, const vstr *q);
  int vstr_setvi(vstr *p, const vstr *q, size_t qx);
  int vstr_setvn(vstr *p, const vstr *q, size_t qn);
  int vstr_setvin(vstr *p, const vstr *q, size_t qx, size_t qn);
  int vstr_setvr(vstr *p, const vstr *q, size_t qx);
  int vstr_setvrn(vstr *p, const vstr *q, size_t qx, size_t qn);
  int vstr_setf(vstr *p, const char *fmt, ...);




  /* Insertion functions */
  int vstr_insert(vstr *p, size_t x, const char *s);
  int vstr_insert0(vstr *p, size_t x, const char *s);
  int vstr_insertc(vstr *p, size_t x, int, size_t n);
  int vstr_insertn(vstr *p, size_t x, const char *, size_t n);
  int vstr_insertv(vstr *p, size_t x, const vstr *q);
  int vstr_insertvi(vstr *p, size_t x, const vstr *q, size_t qx);
  int vstr_insertvn(vstr *p, size_t x, const vstr *q, size_t qn);
  int vstr_insertvin(vstr *p, size_t x,
		     const vstr *q, size_t qx, size_t qn);
  int vstr_insertvr(vstr *p, size_t x, const vstr *q, size_t qx);
  int vstr_insertvrn(vstr *p, size_t x,
		     const vstr *q, size_t qx, size_t qn);
  int vstr_insertf(vstr *p, size_t x, const char *fmt, ...);


  /* Appendage functions; All of these are equivalent to their
     insertion counterparts, but with x set to the length. */
  int vstr_append(vstr *p, const char *s);
  int vstr_append0(vstr *p, const char *s);
  int vstr_appendn(vstr *p, const char *s, size_t n);
  int vstr_appendc(vstr *p, int c, size_t n);
  int vstr_appendv(vstr *p, const vstr *q);
  int vstr_appendvi(vstr *p, const vstr *q, size_t qx);
  int vstr_appendvn(vstr *p, const vstr *q, size_t qn);
  int vstr_appendvin(vstr *p, const vstr *q, size_t qx, size_t qn);
  int vstr_appendvr(vstr *p, const vstr *q, size_t qx);
  int vstr_appendvrn(vstr *p, const vstr *q, size_t qx, size_t qn);
  int vstr_appendf(vstr *p, const char *fmt, ...);


  /* splice/elide/elect functions have the following forms:

     ~: x is index from start, n counts towards end.

     r~: x is index from end, n counts towards start.

     ~r: x is index from end, n counts towards end. 

     r~r: x is index from start, n counts towards start. */



  /* Internal allocation functions */

  /* Insert space for n elements at x from start, returning pointer to
     start of new space. */
  char *vstr_splice(vstr *p, size_t x, size_t n);

  /* Insert space for n elements at x from end, returning pointer to
     one-past-end of new space. */
  char *vstr_rsplice(vstr *p, size_t x, size_t n);

  /* Insert space for n elements at x from end, returning pointer to
     start of new space. */
  char *vstr_splicer(vstr *p, size_t x, size_t n);

  /* Insert space for n elements at x from start, returning pointer to
     one-past-end of new space. */
  char *vstr_rsplicer(vstr *p, size_t x, size_t n);

  /* Remove a central piece. */
  void vstr_elide(vstr *p, size_t x, size_t n);
  void vstr_relide(vstr *p, size_t x, size_t n);
  void vstr_elider(vstr *p, size_t x, size_t n);
  void vstr_relider(vstr *p, size_t x, size_t n);

  /* Retain a central piece. */
  void vstr_elect(vstr *p, size_t x, size_t n);
  void vstr_relect(vstr *p, size_t x, size_t n);
  void vstr_electr(vstr *p, size_t x, size_t n);
  void vstr_relectr(vstr *p, size_t x, size_t n);

  /* Keep the first n characters. */
  void vstr_truncate(vstr *p, size_t n);

  /* Remove the last n characters. */
  void vstr_rtruncate(vstr *p, size_t n);

  /* Remove the first n characters. */
  void vstr_neck(vstr *p, size_t n);

  /* Keep the last n characters. */
  void vstr_rneck(vstr *p, size_t n);



  int vstr_wcsmblen(const wchar_t *s, size_t len);

#define vstr_get(p) ((char *) (p)->base)
#define vstr_len(p) ((size_t) (p)->len)
#define vstr_clear(p) ((void) ((p)->len = 0))

#endif

#ifdef va_start
  /* We can afford to re-declare functions without re-inclusion
     protection.  This allows the user to include <stdarg.h> and then
     <ddslib/vstr.h> again to access these specific functions if an
     earlier inclusion failed to declare them. */
  int vstr_vsetf(vstr *p, const char *fmt, va_list ap);
  int vstr_vinsertf(vstr *p, size_t x, const char *fmt, va_list ap);
  int vstr_vappendf(vstr *p, const char *fmt, va_list ap);
#endif

#ifdef __cplusplus
}
#endif
