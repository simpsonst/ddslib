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

#ifndef vwcs_INCLUDED
#define vwcs_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <wchar.h>

  typedef struct {
    wchar_t *base;
    size_t cap, len;
  } vwcs;

#define vwcs_NULL { NULL, 0, 0 }

#if defined __GNUC__
#define vwcs_inline extern inline
#define vwcs_INLINEBODY(B) B struct tm
#else
#define vwcs_inline inline
#define vwcs_INLINEBODY(B)
#endif

  vwcs_inline wchar_t *vwcs_get(const vwcs *p) { return p->base; }
  vwcs_inline size_t vwcs_len(const vwcs *p) { return p->len; }
  void vwcs_cancel(vwcs *);
  void vwcs_compact(vwcs *);
  int vwcs_term(vwcs *p);
  wchar_t *vwcs_splice(vwcs *, size_t index, size_t n);
  int vwcs_finsert(vwcs *, size_t index, const wchar_t *fmt, ...);
  int vwcs_vfinsert(vwcs *, size_t index, const wchar_t *fmt, va_list ap);
  int vwcs_insertn(vwcs *, size_t index, const wchar_t *, size_t);
  int vwcs_elide(vwcs *, size_t index, size_t);
  int vwcs_fappend(vwcs *p, const wchar_t *fmt, ...);

  vwcs_inline int vwcs_vfappend(vwcs *p, const wchar_t *fmt, va_list ap) {
    return vwcs_vfinsert(p, vwcs_len(p), fmt, ap);
  }

  vwcs_inline int vwcs_insert(vwcs *p, size_t index, const wchar_t *s) {
    return vwcs_insertn(p, index, s, wcslen(s));
  }

  vwcs_inline int vwcs_appendn(vwcs *p, const wchar_t *s, size_t n) {
    return vwcs_insertn(p, vwcs_len(p), s, n);
  }

  vwcs_inline int vwcs_append0(vwcs *p, const wchar_t *s) {
    return vwcs_appendn(p, s, wcslen(s));
  }

#ifdef __cplusplus
}
#endif

#endif
