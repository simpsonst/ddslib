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
  int (*cmp)(void *, htab_const, htab_const);
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

/* This is a hack to persuade the compiler not to warn about
   dereferencing type-punned pointers.  This is only done in this file
   to convert pointers to htab_obj into pointers to htab_const.  These
   union types are identical, except that one member is (void *) in
   one and (const void *) in the other.  Is there any way this can
   fail? */
static htab_const *get_const(htab_obj *p)
{
  union {
    htab_obj *nc;
    htab_const *c;
  } var;
  var.nc = p;
  return var.c;
}

htab htab_open(size_t n, void *ctxt,
               size_t (*hash)(void *, htab_const),
               int (*cmp)(void *, htab_const, htab_const),
               htab_obj (*copy_key)(void *ctxt, htab_const),
               htab_obj (*copy_value)(void *ctxt, htab_const),
               void (*release_key)(void *ctxt, htab_obj),
               void (*release_value)(void *ctxt, htab_obj val))
{
  size_t i;

  htab self = malloc(sizeof *self);
  if (!self) return NULL;

  self->base = malloc(n * sizeof self->base[0]);
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
  if (!self) return;
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

static htab_apprc clear_item(void *vp, htab_const key, htab_obj val)
{
  return htab_REMOVE;
}

void htab_clear(htab self)
{
  htab_apply(self, NULL, &clear_item);
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

int htab_cmp_str(void *ctxt, htab_const a, htab_const b)
{
  return strcmp(a.pointer, b.pointer);
}

int htab_cmp_wcs(void *ctxt, htab_const a, htab_const b)
{
  return wcscmp(a.pointer, b.pointer);
}

htab_obj htab_copy_str(void *ctxt, htab_const in)
{
  htab_obj out;
  if (!in.pointer) {
    out.pointer = NULL;
    return out;
  }
  size_t len = strlen(in.pointer);
  out.pointer = malloc(len + 1);
  if (out.pointer)
    strcpy(out.pointer, in.pointer);
  return out;
}

htab_obj htab_copy_wcs(void *ctxt, htab_const in)
{
  htab_obj out;
  if (!in.pointer) {
    out.pointer = NULL;
    return out;
  }
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
  while (*res && (*self->cmp)(self->ctxt, key, *get_const(&(*res)->key)))
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
  if (old) {
    *old = (*pos)->value;
  } else if (self->release_value) {
    (*self->release_value)(self->ctxt, (*pos)->value);
  }
  e = *pos;
  *pos = e->next;
  if (self->release_key)
    (*self->release_key)(self->ctxt, e->key);
  return true;
}

htab_rplc htab_rpl(htab self, htab_const key, htab_obj *old, htab_const val)
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
      return htab_ERROR;
    (*pos)->next = NULL;
    if (self->copy_key)
      (*pos)->key = (*self->copy_key)(self->ctxt, key);
    else {
      assert(sizeof key == sizeof (*pos)->key);
      memcpy(&(*pos)->key, &key, sizeof key);
    }
  }
  if (self->copy_value)
    (*pos)->value = (*self->copy_value)(self->ctxt, val);
  else {
    assert(sizeof val == sizeof (*pos)->value);
    memcpy(&(*pos)->value, &val, sizeof val);
  }
  return r ? htab_REPLACED : htab_OKAY;
}

_Bool htab_put(htab self, htab_const key, htab_const val)
{
  switch (htab_rpl(self, key, NULL, val)) {
  case htab_ERROR:
    return false;
  default:
    return true;
  }
}

void htab_apply(htab self, void *ctxt,
                htab_apprc (*op)(void *, htab_const, htab_obj))
{
  for (size_t i = 0; i < self->len; i++) {
    struct entry *n, *e, **eh = &self->base[i];
    for (e = *eh; e && (n = e->next, true); e = n) {
      htab_apprc rc = (*op)(ctxt, *get_const(&e->key), e->value);
      if (rc & htab_REMOVE) {
        if (self->release_value)
          (*self->release_value)(self->ctxt, e->value);
        if (self->release_key)
          (*self->release_key)(self->ctxt, e->key);
        free(e);
        *eh = n;
      } else
        eh = &e->next;
      if (rc & htab_STOP)
        return;
    }
  }
}

htab_DEFN(sp, const char *, void *, void *, pointer, pointer, NULL);
htab_DEFN(ss, const char *, char *, const char *, pointer, pointer, NULL);
htab_DEFN(wp, const wchar_t *, void *, void *, pointer, pointer, NULL);
htab_DEFN(ww, const wchar_t *, wchar_t *, const wchar_t *,
          pointer, pointer, NULL);
htab_DEFN(ws, const wchar_t *, char *, const char *, pointer, pointer, NULL);
htab_DEFN(sw, const char *, wchar_t *, const wchar_t *,
          pointer, pointer, NULL);
htab_DEFN(pp, const void *, void *, void *, pointer, pointer, NULL);
htab_DEFN(wu, const wchar_t *, uintmax_t, uintmax_t,
          pointer, unsigned_integer, 0);
htab_DEFN(su, const char *, uintmax_t, uintmax_t,
          pointer, unsigned_integer, 0);
