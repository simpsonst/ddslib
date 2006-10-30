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

#define vwcs_NULL ((vwcs) { NULL, 0, 0 })

#if defined __GNUC__
#define vwcs_inline extern inline
#define vwcs_INLINEBODY(B) B struct tm
#else
#define vwcs_inline inline
#define vwcs_INLINEBODY(B)
#endif

  vwcs_inline wchar_t *vwcs_get(const vwcs *p) { return p->base; }
  vwcs_inline size_t vwcs_len(const vwcs *p) { return p->len; }
  vwcs_inline void vwcs_clear(vwcs *p) { p->len = 0; }

  int vwcs_empty(vwcs *p);
  void vwcs_reset(vwcs *);
  void vwcs_compact(vwcs *);
  int vwcs_term(vwcs *p);
  int vwcs_unterm(vwcs *);
  int vwcs_setcap(vwcs *p, size_t nc);
  int vwcs_ensure(vwcs *p, size_t cap);
  wchar_t *vwcs_splice(vwcs *, size_t index, size_t n);
  int vwcs_insertf(vwcs *, size_t index, const wchar_t *fmt, ...);
  int vwcs_vinsertf(vwcs *, size_t index, const wchar_t *fmt, va_list ap);
  int vwcs_insertn(vwcs *, size_t index, const wchar_t *, size_t);
  int vwcs_insertvin(vwcs *p, size_t index,
		     const vwcs *q, size_t qi, size_t qn);
  int vwcs_insertvrn(vwcs *p, size_t index,
		     const vwcs *q, size_t qi, size_t qn);
  int vwcs_insertvi(vwcs *p, size_t index, const vwcs *q, size_t qi);
  int vwcs_insertvn(vwcs *p, size_t index, const vwcs *q, size_t qn);
  int vwcs_insertvr(vwcs *p, size_t index, const vwcs *q, size_t qi);
  int vwcs_insertv(vwcs *p, size_t index, const vwcs *q);
  void vwcs_elide(vwcs *, size_t index, size_t);
  void vwcs_truncate(vwcs *, size_t index);
  int vwcs_appendf(vwcs *p, const wchar_t *fmt, ...);
  int vwcs_appendvin(vwcs *p, const vwcs *q, size_t qi, size_t qn);
  int vwcs_appendvrn(vwcs *p, const vwcs *q, size_t qi, size_t qn);
  int vwcs_appendvi(vwcs *p, const vwcs *q, size_t qi);
  int vwcs_appendvn(vwcs *p, const vwcs *q, size_t qn);
  int vwcs_appendvr(vwcs *p, const vwcs *q, size_t qi);
  int vwcs_appendv(vwcs *p, const vwcs *q);

  vwcs_inline int vwcs_vappendf(vwcs *p, const wchar_t *fmt, va_list ap) {
    return vwcs_vinsertf(p, vwcs_len(p), fmt, ap);
  }

  vwcs_inline int vwcs_insert(vwcs *p, size_t index, const wchar_t *s) {
    return vwcs_insertn(p, index, s, wcslen(s));
  }

  vwcs_inline int vwcs_appendn(vwcs *p, const wchar_t *s, size_t n) {
    return vwcs_insertn(p, vwcs_len(p), s, n);
  }

  vwcs_inline int vwcs_append(vwcs *p, const wchar_t *s) {
    return vwcs_appendn(p, s, wcslen(s));
  }

#ifdef __cplusplus
}
#endif

#endif
