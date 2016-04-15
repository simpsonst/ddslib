
/* PFX: prefix; vstr, vwcs

   TYPE: type; vstr, vwcs

   CT: character type; char, wchar_t

   IT: integer type; int, wchar_t

   NC: null character; '\0', L'\0'

   NS: empty string; "", L""

   LEN: length function; strlen, wcslen

   SET: array-set function; memset, wmemset

   MOVE: move function; memmove, wmemmove

   COPY: copy function; memcpy, wmemcpy

   PRINT: print function: vsnprintf, vswprintf

   NAIVE: If true, use PRINT with increasing allocations until it
   fits.  Otherwise, use the function once with a NULL buffer to get
   the required size, allocate that amount, and use the function again
   with the new space. */

#define IMPL(PFX, TYPE, CT, IT, NC, NS, LEN, SET, MOVE, COPY, PRINT, NAIVE) \
 \
/*** Internal functions that don't do any range checking ***/ \
 \
/* Set the capacity to an exact size. */ \
static int setcap(TYPE *p, size_t nc) \
{ \
  assert(p->len <= nc); \
  if (!nc) nc = 1; \
  void *np = realloc(p->base, nc * sizeof *p->base); \
  if (!np) return -1; \
  p->base = np; \
  p->cap = nc; \
  return 0; \
} \
 \
/* Set the contents to an array of known length, increasing the \
   capacity if necessary. */ \
static int set(TYPE *p, const CT *s, size_t n) \
{ \
  assert(s != NULL); \
  if (PFX ## _ensure(p, n) < 0) return -1; \
  COPY(p->base, s, n); \
  p->len = n; \
  return 0; \
} \
 \
/* Insert space for n elements starting at x from start.  Return a \
   pointer to the first new element. */ \
static CT *splice(TYPE *p, size_t x, size_t n) \
{ \
  assert(x <= p->len); \
 \
  /* Determine whether greater capacity is required. */ \
  if (p->len + n > p->cap) { \
    size_t nc = (p->len + n + 3u) / 4u * 5u; \
    assert(nc >= p->len + n); \
    if (setcap(p, nc) < 0) return NULL; \
  } \
 \
  /* Move the tail to the new end position. */ \
  const size_t end = x + n; \
  const size_t rem = p->len - x; \
  MOVE(p->base + end, p->base + x, rem); \
  p->len += n; \
 \
  return p->base + x; \
} \
 \
/* Reduce the capacity if the length is much smaller. */ \
static void reduce_cap(TYPE *p) \
{ \
  if (p->len < p->cap / 4u) { \
    size_t nc = (p->len + 3u) / 4u * 5u; \
    if (nc <= p->cap) { \
      int rc = setcap(p, nc); \
      assert(rc == 0); \
    } \
  } \
} \
 \
/* Remove n elements starting at x from start. */ \
static void elide(TYPE *p, size_t x, size_t n) \
{ \
  assert(x <= p->len); \
  assert(n <= p->len - x); \
 \
  /* Move the tail over the removed section. */ \
  size_t end = x + n; \
  size_t rem = p->len - end; \
  MOVE(p->base + x, p->base + end, rem); \
  p->len -= n; \
 \
  /* Determine whether to reduce the capacity. */ \
  reduce_cap(p); \
} \
 \
/* Remove all elements after x from start. */ \
static void truncate(TYPE *p, size_t x) \
{ \
  assert(p->len >= x); \
 \
  /* Discard the tail. */ \
  p->len = x; \
 \
  /* Determine whether to reduce the capacity. */ \
  reduce_cap(p); \
} \
 \
/* Remove the first x elements, and retain the next n. */ \
static void elect(TYPE *p, size_t x, size_t n) \
{ \
  assert(x <= p->len); \
  assert(n <= p->len - x); \
 \
  MOVE(p->base, p->base + x, n); \
  truncate(p, n); \
} \
 \
 \
 \
 \
/* Normalize index and length for *_vin functions.  *xp is an index \
   from the start.  *np is a length towards the end.  If *xp is beyond \
   the length, return true to indicate out-of-range.  If *xp + *np is \
   beyond the length, reduce it to hit the end exactly. */ \
static int normalize_vin(const TYPE *p, size_t *xp, size_t *np) \
{ \
  if (*xp >= p->len) \
    return 1; \
  if (*np > p->len - *xp) \
    *np = p->len - *xp; \
  return 0; \
} \
 \
/* Normalize index and length for *_vrn functions.  *xp is an index \
   from the end.  *np is a length towards the end.  If *xp is beyond \
   the start, move it to the start, and reduce *np by a corresponding \
   amount, so it effectively doesn't move.  If *np would go negative \
   or zero, return true to indicate out-of-range.  Finally, if *np \
   goes beyond the end, reduce it to hit the end exactly.  */ \
static int normalize_vrn(const TYPE *p, size_t *xp, size_t *np) \
{ \
  if (*xp >= p->len) { \
    size_t diff = *xp - p->len; \
    if (*np <= diff) \
      return 1; \
    *np -= diff; \
    *xp = p->len; \
  } \
  if (*np > *xp) \
    *np = *xp; \
  return 0; \
} \
 \
 \
 \
CT *(PFX ## _get)(const TYPE *p) \
{ \
  return p->base; \
} \
 \
size_t (PFX ## _len)(const TYPE *p) \
{ \
  return p->len; \
} \
 \
void (PFX ## _clear)(TYPE *p) \
{ \
  p->len = 0; \
} \
 \
void PFX ## _reset(TYPE *p) \
{ \
  free(p->base); \
  p->base = NULL; \
  p->cap = p->len = 0; \
} \
 \
 \
/* Optimize the capacity to the required size. */ \
void PFX ## _compact(TYPE *p) \
{ \
  if (p->cap > p->len) { \
    int rc = setcap(p, p->len); \
    assert(rc == 0); \
  } \
} \
 \
 \
 \
 \
 \
 \
 \
int PFX ## _setc(TYPE *p, IT c, size_t n) \
{ \
  if (PFX ## _ensure(p, n) < 0) return -1; \
  SET(p->base, c, n); \
  p->len = n; \
  return 0; \
} \
 \
int PFX ## _setn(TYPE *p, const CT *s, size_t n) \
{ \
  if (s) { \
    return set(p, s, n); \
  } else { \
    PFX ## _reset(p); \
    return 0; \
  } \
} \
 \
int PFX ## _setv(TYPE *to, const TYPE *from) \
{ \
  return PFX ## _setn(to, from->base, from->len); \
} \
 \
int PFX ## _setvin(TYPE *p, const TYPE *q, size_t qx, size_t qn) \
{ \
  if (q->base) { \
    if (normalize_vin(q, &qx, &qn)) \
      return set(p, NS, 0); \
    else \
      return set(p, q->base + qx, qn); \
  } else { \
    PFX ## _reset(p); \
    return 0; \
  } \
} \
 \
int PFX ## _setvi(TYPE *p, const TYPE *q, size_t qx) \
{ \
  return PFX ## _setvin(p, q, qx, q->len - qx); \
} \
 \
int PFX ## _setvn(TYPE *p, const TYPE *q, size_t qn) \
{ \
  return PFX ## _setvin(p, q, 0, qn); \
} \
 \
int PFX ## _setvr(TYPE *p, const TYPE *q, size_t qx) \
{ \
  if (q->base) { \
    if (qx >= q->len) \
      return set(p, q->base, q->len); \
    else \
      return set(p, q->base + q->len - qx, qx); \
  } else { \
    PFX ## _reset(p); \
    return 0; \
  } \
} \
 \
int PFX ## _setvrn(TYPE *p, const TYPE *q, size_t qx, size_t qn) \
{ \
  if (q->base) { \
    if (normalize_vrn(q, &qx, &qn)) \
      return set(p, NS, 0); \
    else \
      return set(p, q->base + q->len - qx, qn); \
  } else { \
    PFX ## _reset(p); \
    return 0; \
  } \
} \
 \
int PFX ## _vsetf(TYPE *p, const CT *fmt, va_list ap) \
{ \
  if (PFX ## _empty(p) < 0) return -1; \
  return PFX ## _vinsertf(p, 0, fmt, ap); \
} \
 \
 \
 \
 \
 \
 \
 \
CT *PFX ## _splice(TYPE *p, size_t x, size_t n) \
{ \
  if (x > p->len) x = p->len; \
  return splice(p, x, n); \
} \
 \
CT *PFX ## _rsplice(TYPE *p, size_t x, size_t n) \
{ \
  CT *r = PFX ## _splicer(p, x, n); \
  return r ? r + n : NULL; \
} \
 \
CT *PFX ## _splicer(TYPE *p, size_t x, size_t n) \
{ \
  if (x > p->len) x = p->len; \
  return splice(p, p->len - x, n); \
} \
 \
CT *PFX ## _rsplicer(TYPE *p, size_t x, size_t n) \
{ \
  CT *r = PFX ## _splice(p, x, n); \
  return r ? r + n : NULL; \
} \
 \
 \
 \
 \
 \
void PFX ## _truncate(TYPE *p, size_t x) \
{ \
  if (p->base && x < p->len) \
    truncate(p, x); \
} \
 \
void PFX ## _rtruncate(TYPE *p, size_t x) \
{ \
  if (p->base) \
    truncate(p, x < p->len ? p->len - x : 0); \
} \
 \
void PFX ## _neck(TYPE *p, size_t x) \
{ \
  if (x < p->len) \
    elide(p, 0, x); \
  else \
    PFX ## _clear(p); \
} \
 \
void PFX ## _rneck(TYPE *p, size_t x) \
{ \
  if (x < p->len) \
    elide(p, 0, p->len - x); \
  else \
    PFX ## _clear(p); \
} \
 \
 \
 \
 \
CT *PFX ## _extract(TYPE *p) \
{ \
  PFX ## _compact(p); \
  CT *r = p->base; \
  p->len = p->cap = 0; \
  p->base = NULL; \
  return r; \
} \
 \
int PFX ## _setcap(TYPE *p, size_t ncap) \
{ \
  if (ncap < p->len) return -1; \
  return setcap(p, ncap); \
} \
 \
int PFX ## _ensure(TYPE *p, size_t cap) \
{ \
  if (cap <= p->cap) return 0; \
  return PFX ## _setcap(p, cap); \
} \
 \
int PFX ## _vinsertf(TYPE *p, size_t x, const CT *fmt, va_list ap)	\
{									\
  if (NAIVE) {								\
    if (x > p->len) x = p->len;						\
    size_t done = 0, guess = 100;					\
    va_list ap2;							\
    int rc;								\
    do {								\
      CT *pos = splice(p, x, guess);					\
      if (pos == NULL) {						\
	elide(p, x, done);						\
	return -1;							\
      }									\
      done += guess;							\
      guess += guess / 2;						\
      va_copy(ap2, ap);							\
      rc = PRINT(pos, done, fmt, ap2);					\
      va_end(ap2);							\
    } while (rc < 0);							\
    elide(p, x + rc, done - x - rc);					\
    return rc;								\
  } else {								\
    /* How much space is required? */					\
    int req;								\
    va_list ap2;							\
    va_copy(ap2, ap);							\
    req = PRINT(NULL, 0, fmt, ap2);					\
    va_end(ap2);							\
    if (req < 0) return -1;						\
									\
    /* Do we need to allocate an extra byte at the end for the NC	\
       which printf will add? */					\
    int gap = x >= p->len;						\
    if (gap) x = p->len;						\
									\
    /* Try to make that space available. */				\
    CT *pos = splice(p, x, req + gap);					\
    if (!pos) return -1;						\
									\
    CT old = NC;							\
    if (!gap)								\
      old = p->base[x + req];						\
									\
    /* Now write the characters in. */					\
    int rc = PRINT(pos, req + 1, fmt, ap);				\
    assert(rc == req);							\
									\
    if (gap)								\
      /* Remove the trailing NC. */					\
      p->len--;								\
    else								\
      /* Restore the original character overwritten by the NC. */	\
      p->base[x + req] = old;						\
    return rc;								\
  }									\
}									\
 \
int PFX ## _empty(TYPE *p) \
{ \
  PFX ## _clear(p); \
  return p->base ? setcap(p, 1) : 0; \
} \
 \
int PFX ## _insertc(TYPE *p, size_t x, IT c, size_t n) \
{ \
  if (!n) return 0; \
  CT *pos = PFX ## _splice(p, x, n); \
  if (!pos) return -1; \
  assert(pos - p->base >= 0); \
  assert((size_t) (pos - p->base) <= p->cap); \
  assert((size_t) (pos - p->base) + n <= p->cap); \
  SET(pos, c, n); \
  return 0; \
} \
 \
int PFX ## _insertn(TYPE *p, size_t x, const CT *s, size_t n) \
{ \
  if (!s || !n) return 0; \
  CT *pos = PFX ## _splice(p, x, n); \
  if (!pos) return -1; \
  assert(pos - p->base >= 0); \
  assert((size_t) (pos - p->base) <= p->cap); \
  assert((size_t) (pos - p->base) + n <= p->cap); \
  COPY(pos, s, n); \
  return 0; \
} \
 \
int PFX ## _insertvin(TYPE *p, size_t x, \
		   const TYPE *q, size_t qx, size_t qn) \
{ \
  if (qx >= PFX ## _len(q)) return 0; \
  if (qx + qn > PFX ## _len(q)) qn = PFX ## _len(q) - qx; \
  return PFX ## _insertn(p, x, PFX ## _get(q) + qx, qn); \
} \
 \
int PFX ## _insertvrn(TYPE *p, size_t x, \
		   const TYPE *q, size_t qx, size_t qn) \
{ \
  if (qx >= PFX ## _len(q)) return 0; \
  if (qx + qn > PFX ## _len(q)) qn = PFX ## _len(q) - qx; \
  qx = PFX ## _len(q) - qx - qn; \
  return PFX ## _insertn(p, x, PFX ## _get(q) + qx, qn); \
} \
 \
int PFX ## _insertvi(TYPE *p, size_t x, const TYPE *q, size_t qx) \
{ \
  if (qx >= PFX ## _len(q)) return 0; \
  return PFX ## _insertvin(p, x, q, qx, PFX ## _len(q) - qx); \
} \
 \
int PFX ## _insertvn(TYPE *p, size_t x, const TYPE *q, size_t qn) \
{ \
  if (qn > PFX ## _len(q)) qn = PFX ## _len(q); \
  return PFX ## _insertvin(p, x, q, 0, qn); \
} \
 \
int PFX ## _insertvr(TYPE *p, size_t x, const TYPE *q, size_t qx) \
{ \
  if (qx > PFX ## _len(q)) return 0; \
  qx = PFX ## _len(q) - qx; \
  return PFX ## _insertn(p, x, PFX ## _get(q), qx); \
} \
 \
int PFX ## _insertv(TYPE *p, size_t x, const TYPE *q) \
{ \
  return PFX ## _insertn(p, x, PFX ## _get(q), PFX ## _len(q)); \
} \
 \
int PFX ## _term(TYPE *p) \
{ \
  if (!p->base) return 0; \
  if (p->len == 0 || p->base[p->len - 1] != NC) \
    if (PFX ## _appendn(p, NS, 1) < 0) return -1; \
  return 0; \
} \
 \
int PFX ## _unterm(TYPE *p) \
{ \
  if (p->len == 0 || p->base[p->len - 1] != NC) \
    return 0; \
  truncate(p, p->len - 1); \
  return 0; \
} \
 \
 \
 \
int PFX ## _appendn(TYPE *p, const CT *s, size_t n) \
{ \
  return PFX ## _insertn(p, PFX ## _len(p), s, n); \
} \
 \
int PFX ## _appendc(TYPE *p, IT c, size_t n) \
{ \
  return PFX ## _insertc(p, PFX ## _len(p), c, n); \
} \
 \
int PFX ## _vappendf(TYPE *p, const CT *fmt, va_list ap) \
{ \
  return PFX ## _vinsertf(p, PFX ## _len(p), fmt, ap); \
} \
 \
int PFX ## _appendvin(TYPE *p, const TYPE *q, size_t qx, size_t qn) \
{ \
  return PFX ## _insertvin(p, PFX ## _len(p), q, qx, qn); \
} \
 \
int PFX ## _appendvrn(TYPE *p, const TYPE *q, size_t qx, size_t qn) \
{ \
  return PFX ## _insertvrn(p, PFX ## _len(p), q, qx, qn); \
} \
 \
int PFX ## _appendvi(TYPE *p, const TYPE *q, size_t qx) \
{ \
  return PFX ## _insertvi(p, PFX ## _len(p), q, qx); \
} \
 \
int PFX ## _appendvn(TYPE *p, const TYPE *q, size_t qn) \
{ \
  return PFX ## _insertvn(p, PFX ## _len(p), q, qn); \
} \
 \
int PFX ## _appendvr(TYPE *p, const TYPE *q, size_t qx) \
{ \
  return PFX ## _insertvr(p, PFX ## _len(p), q, qx); \
} \
 \
int PFX ## _appendv(TYPE *p, const TYPE *q) \
{ \
  return PFX ## _insertv(p, PFX ## _len(p), q); \
} \
 \
 \
 \
 \
void PFX ## _elide(TYPE *p, size_t x, size_t n) \
{ \
  if (x >= p->len) return; \
  if (n > p->len - x) n = p->len - x; \
  elide(p, x, n); \
} \
 \
void PFX ## _relide(TYPE *p, size_t x, size_t n) \
{ \
  if (x >= p->len) return; \
  if (n > p->len - x) n = p->len - x; \
  elide(p, p->len - x - n, n); \
} \
 \
void PFX ## _elider(TYPE *p, size_t x, size_t n) \
{ \
  if (normalize_vrn(p, &x, &n)) return; \
  elide(p, p->len - x, n); \
} \
 \
void PFX ## _relider(TYPE *p, size_t x, size_t n) \
{ \
  if (normalize_vrn(p, &x, &n)) return; \
  elide(p, x - n, n); \
} \
 \
 \
void PFX ## _elect(TYPE *p, size_t x, size_t n) \
{ \
  if (x >= p->len) { \
    PFX ## _clear(p); \
    return; \
  } \
  if (n > p->len - x) n = p->len - x; \
  elect(p, x, n); \
} \
 \
void PFX ## _relect(TYPE *p, size_t x, size_t n) \
{ \
  if (x >= p->len) { \
    PFX ## _clear(p); \
    return; \
  } \
  if (n > p->len - x) n = p->len - x; \
  elect(p, p->len - x - n, n); \
} \
 \
void PFX ## _electr(TYPE *p, size_t x, size_t n) \
{ \
  if (normalize_vrn(p, &x, &n)) { \
    PFX ## _clear(p); \
    return; \
  } \
  elect(p, p->len - x, n); \
} \
 \
void PFX ## _relectr(TYPE *p, size_t x, size_t n) \
{ \
  if (normalize_vrn(p, &x, &n)) { \
    PFX ## _clear(p); \
    return; \
  } \
  elect(p, x - n, n); \
} \
 \
 \
 \
 \
 \
/* Functions taking a null-terminated string */ \
 \
int PFX ## _insert(TYPE *p, size_t x, const CT *s) \
{ \
  return s ? PFX ## _insertn(p, x, s, LEN(s)) : 0; \
} \
 \
int PFX ## _insert0(TYPE *p, size_t x, const CT *s) \
{ \
  return s ? PFX ## _insertn(p, x, s, LEN(s) + 1) : 0; \
} \
 \
int PFX ## _append(TYPE *p, const CT *s) \
{ \
  return s ? PFX ## _appendn(p, s, LEN(s)) : 0; \
} \
 \
int PFX ## _append0(TYPE *p, const CT *s) \
{ \
  return s ? PFX ## _appendn(p, s, LEN(s) + 1) : 0; \
} \
 \
int PFX ## _set(TYPE *p, const CT *s) \
{ \
  if (s) { \
    return PFX ## _setn(p, s, LEN(s)); \
  } else { \
    PFX ## _reset(p); \
    return 0; \
  } \
} \
 \
int PFX ## _set0(TYPE *p, const CT *s) \
{ \
  if (s) { \
    return PFX ## _setn(p, s, LEN(s) + 1); \
  } else { \
    PFX ## _reset(p); \
    return 0; \
  } \
} \
 \
 \
 \
 \
 \
/* Varargs functions built from their v~ counterparts */ \
 \
int PFX ## _insertf(TYPE *p, size_t x, const CT *fmt, ...) \
{ \
  va_list ap; \
  int rc; \
 \
  va_start(ap, fmt); \
  rc = PFX ## _vinsertf(p, x, fmt, ap); \
  va_end(ap); \
  return rc; \
} \
 \
int PFX ## _appendf(TYPE *p, const CT *fmt, ...) \
{ \
  va_list ap; \
  int rc; \
 \
  va_start(ap, fmt); \
  rc = PFX ## _vappendf(p, fmt, ap); \
  va_end(ap); \
  return rc; \
} \
 \
int PFX ## _setf(TYPE *p, const CT *fmt, ...) \
{ \
  va_list ap; \
  int rc; \
 \
  va_start(ap, fmt); \
  rc = PFX ## _vsetf(p, fmt, ap); \
  va_end(ap); \
  return rc; \
} \
 \
struct PFX ## _dummy\

