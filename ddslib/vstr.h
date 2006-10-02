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

#ifndef vstr_INCLUDED
#define vstr_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

  typedef struct {
    char *base;
    size_t cap, len;
  } vstr;

#define vstr_NULL { NULL, 0, 0 }

  inline char *vstr_get(const vstr *p) { return p->base; }
  inline size_t vstr_len(const vstr *p) { return p->len; }
  void vstr_cancel(vstr *);
  void vstr_compact(vstr *);
  int vstr_insertn(vstr *, size_t index, const char *, size_t);
  int vstr_elide(vstr *, size_t index, size_t);

  inline int vstr_insert(vstr *p, size_t index, const char *s) {
    return vstr_insertn(p, index, s, strlen(s));
  }

  inline int vstr_appendn(vstr *p, const char *s, size_t n) {
    return vstr_insertn(p, vstr_len(p), s, n);
  }

  inline int vstr_append0(vstr *p, const char *s) {
    return vstr_appendn(p, s, strlen(s));
  }

#ifdef __cplusplus
}
#endif

#endif
