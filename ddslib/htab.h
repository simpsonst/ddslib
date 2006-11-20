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

#ifndef htab_INCLUDED
#define htab_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

  typedef union {
    const void *pointer;
    intmax_t integer;
    uintmax_t unsigned_integer;
    long double real;
  } htab_const;

  typedef union {
    void *pointer;
    intmax_t integer;
    uintmax_t unsigned_integer;
    long double real;
  } htab_obj;

  typedef enum {
    htab_OKAY, htab_REPLACED, htab_ERROR
  } htab_rplc;

  typedef struct htab_str *htab;

  htab htab_open(size_t n, void *,
		 size_t (*hash)(void *, htab_const),
		 int (*cmp)(void *, htab_const, htab_const),
		 htab_obj (*copy_key)(void *ctxt, htab_const),
		 htab_obj (*copy_value)(void *ctxt, htab_const),
		 void (*release_key)(void *ctxt, htab_obj),
		 void (*release_value)(void *ctxt, htab_obj val));
  void htab_close(htab);
  void htab_clear(htab);

  typedef enum { htab_REMOVE = 1, htab_STOP = 2 } htab_apprc;

  void htab_apply(htab, void *,
		  htab_apprc (*op)(void *, htab_const, htab_obj));

  // Returns true if found.
  _Bool htab_get(htab, htab_const, htab_obj *);

  // Returns true if found.
  _Bool htab_pop(htab, htab_const, htab_obj *);

  htab_rplc htab_rpl(htab, htab_const, htab_obj *, htab_const val);

  // Returns true if successful.
  _Bool htab_put(htab, htab_const, htab_const val);

  // Returns true if found.
#define htab_tst(T,K) htab_get((T),(K),0)

  // Returns true if found.
#define htab_del(T,K) htab_pop((T),(K),0)

#if __STDC_VERSION__ < 199901L
  /* Wrapper functions are as usual. */
#define htab_DECL(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
		  htab_PROTO(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE,)
#define htab_DEFN(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
		  htab_IMPL(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE,, \
			    KEY_MEMBER, VALUE_MEMBER, NULL_VALUE)

#elif defined __GNUC__
  /* GCC has wierd linkage for inlines. */
#define htab_DECL(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
		  htab_IMPL(SUFFIX, KEY_TYPE, VALUE_TYPE, \
                            CONST_VALUE_TYPE, extern inline, \
			    KEY_MEMBER, VALUE_MEMBER, NULL_VALUE)
#define htab_DEFN(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
		  htab_IMPL(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE,, \
			    KEY_MEMBER, VALUE_MEMBER, NULL_VALUE)

#else
  /* True inlines are implemented. */
#define htab_DECL(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
		  htab_IMPL(SUFFIX, KEY_TYPE, VALUE_TYPE, \
                            CONST_VALUE_TYPE, inline, \
			    KEY_MEMBER, VALUE_MEMBER, NULL_VALUE)
#define htab_DEFN(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
		  htab_PROTO(SUFFIX, KEY_TYPE, VALUE_TYPE, \
                             CONST_VALUE_TYPE, extern)

#endif

#define htab_IMPL(SUFFIX, KEY_TYPE, VALUE_TYPE, CONST_VALUE_TYPE, STORAGE, \
                  KEY_MEMBER, VALUE_MEMBER, NULL_VALUE) \
  STORAGE CONST_VALUE_TYPE htab_get##SUFFIX(htab self, KEY_TYPE key) { \
    htab_obj val; \
    if (htab_get(self, (htab_const) { .KEY_MEMBER = key }, &val)) \
      return val.VALUE_MEMBER; \
    return NULL_VALUE; \
  } \
 \
  STORAGE VALUE_TYPE htab_pop##SUFFIX(htab self, KEY_TYPE key) { \
    htab_obj val; \
    if (htab_pop(self, (htab_const) { .KEY_MEMBER = key }, &val)) \
      return val.VALUE_MEMBER; \
    return NULL_VALUE; \
  } \
 \
  STORAGE VALUE_TYPE htab_rpl##SUFFIX(htab self, \
                                      KEY_TYPE key, CONST_VALUE_TYPE val) { \
    htab_obj oldval; \
    if (htab_rpl(self, (htab_const) { .KEY_MEMBER = key }, &oldval, \
                 (htab_const) { .VALUE_MEMBER = val })) \
      return oldval.VALUE_MEMBER; \
    return NULL_VALUE; \
  } \
 \
  STORAGE _Bool htab_put##SUFFIX(htab self, \
                                 KEY_TYPE key, CONST_VALUE_TYPE val) { \
    return htab_put(self, \
                    (htab_const) { .KEY_MEMBER = key }, \
                    (htab_const) { .VALUE_MEMBER = val }); \
  } \
 \
  STORAGE _Bool htab_tst##SUFFIX(htab self, KEY_TYPE key) { \
    return htab_tst(self, (htab_const) { .KEY_MEMBER = key }); \
  } \
 \
  STORAGE _Bool htab_del##SUFFIX(htab self, KEY_TYPE key) { \
    return htab_del(self, (htab_const) { .KEY_MEMBER = key }); \
  } struct tm

#define htab_PROTO(SUFFIX, KEY_TYPE, VALUE_TYPE, STORAGE) \
  STORAGE CONST_VALUE_TYPE htab_get##SUFFIX(htab self, KEY_TYPE key); \
  STORAGE VALUE_TYPE htab_pop##SUFFIX(htab self, KEY_TYPE key); \
  STORAGE VALUE_TYPE htab_rpl##SUFFIX(htab self, \
                                      KEY_TYPE key, CONST_VALUE_TYPE val); \
  STORAGE _Bool htab_put##SUFFIX(htab self, \
                                 KEY_TYPE key, CONST_VALUE_TYPE val); \
  STORAGE _Bool htab_tst##SUFFIX(htab self, KEY_TYPE key); \
  STORAGE _Bool htab_del##SUFFIX(htab self, KEY_TYPE key)

  htab_DECL(sp, const char *, void *, void *, pointer, pointer, NULL);
  htab_DECL(ss, const char *, char *, const char *, pointer, pointer, NULL);
  htab_DECL(wp, const wchar_t *, void *, void *, pointer, pointer, NULL);
  htab_DECL(ww, const wchar_t *, wchar_t *, const wchar_t *,
	    pointer, pointer, NULL);
  htab_DECL(ws, const wchar_t *, char *, const char *, pointer, pointer, NULL);
  htab_DECL(sw, const char *, wchar_t *, const wchar_t *,
	    pointer, pointer, NULL);
  htab_DECL(pp, const void *, void *, void *, pointer, pointer, NULL);
  htab_DECL(wu, const wchar_t *, uintmax_t, uintmax_t,
	    pointer, unsigned_integer, 0);
  htab_DECL(su, const char *, uintmax_t, uintmax_t,
	    pointer, unsigned_integer, 0);

  size_t htab_hash_str(void *, htab_const);
  size_t htab_hash_wcs(void *, htab_const);
  int htab_cmp_str(void *, htab_const, htab_const);
  int htab_cmp_wcs(void *, htab_const, htab_const);
  htab_obj htab_copy_str(void *ctxt, htab_const);
  htab_obj htab_copy_wcs(void *ctxt, htab_const);
  void htab_release_free(void *, htab_obj key);

#ifdef __cplusplus
}
#endif

#endif
