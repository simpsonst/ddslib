#include <assert.h>
#include <stdlib.h>

#include "ddslib/vstr.h"

void vstr_cancel(vstr *p)
{
  free(p->base);
  p->base = NULL;
  p->cap = p->len = 0;
}

void vstr_compact(vstr *p)
{
  if (p->cap > p->len) {
    void *np = realloc(p->base, p->len);
    if (!np) return;
    p->base = np;
    p->cap = p->len;
  }
}

int vstr_insertn(vstr *p, size_t index, const char *s, size_t n)
{
  if (p->len + n > p->cap) {
    size_t nc = (p->len + n + 3u) / 4u * 5u;
    assert(nc >= p->len + n);
    void *np = realloc(p->base, nc);
    if (!np) return -1;
    p->base = np;
    p->cap = nc;
  }
  if (index > p->len) index = p->len;
  size_t end = index + n;
  size_t rem = p->len - end;
  memmove(p->base + end, p->base + index, rem);
  memcpy(p->base + p->len, s, n);
  p->len += n;
  return 0;
}

int vstr_elide(vstr *p, size_t index, size_t n)
{
  if (index >= p->len) return 0;
  if (index + n > p->len) n = p->len - index;
  size_t end = index + n;
  size_t rem = p->len - end;
  memmove(p->base + index, p->base + end, rem);
  p->len -= n;
  if (p->len < p->cap / 4u) {
    size_t nc = (p->len + 3u) / 4u * 5u;
    assert(nc >= p->len);
    void *np = realloc(p->base, nc);
    if (np) {
      p->base = np;
      p->cap = nc;
    }
  }
  return 0;
}

extern char *vstr_get(const vstr *);
extern size_t vstr_len(const vstr *);
extern int vstr_insert(vstr *p, size_t index, const char *s);
extern int vstr_appendn(vstr *p, const char *s, size_t n);
extern int vstr_append0(vstr *p, const char *s);
