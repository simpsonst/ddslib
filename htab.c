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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

#include "ddslib/htab.h"

struct entry;

struct htab_str {
  struct entry **base;
  size_t len;
  void *ctxt;
  size_t (*hash)(void *, htab_const);
  int (*cmp)(void *, htab_const, htab_obj);
  htab_obj (*copy_key)(void *ctxt, htab_const);
  htab_obj (*copy_value)(void *ctxt, htab_const);
  void (*release_key)(void *ctxt, htab_obj);
  void (*release_value)(void *ctxt, htab_obj val);
};

struct entry {
  struct entry *next;
  htab_obj value;
  htab_obj key;
};

htab htab_open(size_t n, void *ctxt,
	       unsigned (*hash)(void *, htab_const),
	       int (*cmp)(void *, htab_const, htab_obj),
	       htab_obj (*copy_key)(void *ctxt, htab_const),
	       htab_obj (*copy_value)(void *ctxt, htab_const),
	       void (*release_key)(void *ctxt, htab_obj),
	       void (*release_value)(void *ctxt, htab_obj val))
{
  size_t i;

  htab self = malloc(sizeof *self);
  if (!self) return NULL;

  self->base = malloc(sizeof *self->base * n);
  if (!self->base) {
    free(self);
    return NULL;
  }

  self->len = n;
  self->ctxt = ctxt;
  self->hash = hash;
  self->cmp = cmp;
  self->copy_key = copy_key;
  self->copy_value = copy_value;
  self->release_key = release_key;
  self->release_value = release_value;
  for (i = 0; i < n; i++)
    self->base[i] = NULL;

  return self;
}

void htab_close(htab self)
{
  size_t i;
  for (i = 0; i < self->len; i++) {
    struct entry *n, *e;
    for (e = self->base[i]; e && (n = e->next, true); e = n) {
      if (self->release_value)
	(*self->release_value)(self->ctxt, e->value);
      if (self->release_key)
	(*self->release_key)(self->ctxt, e->key);
      free(e);
    }
    self->base[i] = NULL;
  }
  free(self->base);
  self->base = NULL;
  free(self);
}

size_t htab_hash_str(void *ctxt, htab_const key)
{
  size_t r = 0;
  const char *s = key.pointer;
  while (*s)
    r += *s++;
  return r;
}

size_t htab_hash_wcs(void *ctxt, htab_const key)
{
  size_t r = 0;
  const wchar_t *s = key.pointer;
  while (*s)
    r += *s++;
  return r;
}

int htab_cmp_str(void *ctxt, htab_const a, htab_obj b)
{
  return strcmp(a.pointer, b.pointer);
}

int htab_cmp_wcs(void *ctxt, htab_const a, htab_obj b)
{
  return wcscmp(a.pointer, b.pointer);
}

htab_obj htab_copy_str(void *ctxt, htab_const in)
{
  htab_obj out;
  size_t len = strlen(in.pointer);
  out.pointer = malloc(len + 1);
  if (out.pointer)
    strcpy(out.pointer, in.pointer);
  return out;
}

htab_obj htab_copy_wcs(void *ctxt, htab_const in)
{
  htab_obj out;
  size_t len = wcslen(in.pointer);
  out.pointer = malloc((len + 1) * sizeof(wchar_t));
  if (out.pointer)
    wcscpy(out.pointer, in.pointer);
  return out;
}

void htab_release_free(void *ctxt, htab_obj key)
{
  free(key.pointer);
}

static inline struct entry **find_ptr(htab self, htab_const key)
{
  struct entry **res;
  size_t hv = (*self->hash)(self->ctxt, key);
  hv %= self->len;
  res = &self->base[hv];
  while (*res && (*self->cmp)(self->ctxt, key, (*res)->key))
    res = &(*res)->next;
  return res;
}

_Bool htab_get(htab self, htab_const key, htab_obj *old)
{
  struct entry **pos = find_ptr(self, key);
  if (!pos || !*pos) return false;
  *old = (*pos)->value;
  return true;
}

_Bool htab_pop(htab self, htab_const key, htab_obj *old)
{
  struct entry *e, **pos = find_ptr(self, key);
  if (!pos || !*pos) return false;
  *old = (*pos)->value;
  e = *pos;
  *pos = e->next;
  if (self->release_key)
    (*self->release_key)(self->ctxt, e->key);
  return true;
}

_Bool htab_replace(htab self, htab_const key, htab_obj *old, htab_obj val)
{
  struct entry **pos = find_ptr(self, key);
  _Bool r = *pos;
  if (r) {
    if (old)
      *old = (*pos)->value;
    else if (self->release_value)
      (*self->release_value)(self->ctxt, (*pos)->value);
  } else {
    *pos = malloc(sizeof **pos);
    if (!*pos)
      return false;
    (*pos)->next = NULL;
    if (self->copy_key)
      (*pos)->key = (*self->copy_key)(self->ctxt, key);
    else {
      assert(sizeof key == sizeof &(*pos)->key);
      memcpy(&(*pos)->key, &key, sizeof key);
    }
  }
  if (self->copy_value)
    (*pos)->value = (*self->copy_value)(self->ctxt, *(htab_const *) &val);
  else
    (*pos)->value = val;
  return r;
}

_Bool htab_put(htab self, htab_const key, htab_obj val)
{
  htab_obj oldval;
  _Bool r = htab_replace(self, key, &oldval, val);
  if (r && self->release_value)
    (*self->release_value)(self->ctxt, oldval);
  return r;
}

_Bool htab_del(htab self, htab_const key)
{
  htab_obj val;
  _Bool r = htab_pop(self, key, &val);
  if (r && self->release_value)
    (*self->release_value)(self->ctxt, val);
  return r;
}

htab_DEFN(sp, const char *, void *, pointer, pointer, NULL);
htab_DEFN(ss, const char *, char *, pointer, pointer, NULL);
htab_DEFN(wp, const wchar_t *, void *, pointer, pointer, NULL);
htab_DEFN(pp, const void *, void *, pointer, pointer, NULL);
htab_DEFN(su, const char *, uintmax_t, pointer, unsigned_integer, 0);
htab_DEFN(wu, const char *, uintmax_t, pointer, unsigned_integer, 0);
