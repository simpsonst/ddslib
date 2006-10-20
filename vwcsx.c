#include <assert.h>

#include "ddslib/vwcs.h"

int vwcs_vinsertf(vwcs *p, size_t index, const wchar_t *fmt, va_list ap)
{
  if (index > p->len) index = p->len;

  // How much space is required?
  int req;
  va_list ap2;
  va_copy(ap2, ap);
  req = vswprintf(NULL, 0, fmt, ap2);
  va_end(ap2);
  if (req < 0) return -1;

  // Do we need to allocate an extra byte at the end for the L'\0'
  // which wprintf will add?
  int gap = index >= p->len;

  // Try to make that space available.
  wchar_t *pos = vwcs_splice(p, index, req + gap);
  if (!pos) return -1;

  wchar_t old = '\0';
  if (!gap)
    old = p->base[index + req];

  // Now write the characters in.
  assert(pos - p->base >= 0);
  assert((size_t) (pos - p->base) <= p->cap);
  assert((size_t) (pos - p->base) + req + 1 <= p->cap);
  int rc = vswprintf(pos, req + 1, fmt, ap);
  assert(rc >= 0);

  if (gap)
    // Remove the trailing L'\0'.
    vwcs_elide(p, p->len - 1, 1);
  else
    // Restore the original character overwritten by the L'\0'.
    p->base[index + req] = old;
  return 0;
}

int vwcs_insertf(vwcs *p, size_t index, const wchar_t *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vwcs_vinsertf(p, index, fmt, ap);
  va_end(ap);
  return rc;
}

int vwcs_appendf(vwcs *p, const wchar_t *fmt, ...)
{
  va_list ap;
  int rc;

  va_start(ap, fmt);
  rc = vwcs_vappendf(p, fmt, ap);
  va_end(ap);
  return rc;
}

extern int vwcs_vappendf(vwcs *p, const wchar_t *fmt, va_list ap)
     vwcs_INLINEBODY
(
{ return vwcs_vinsertf(p, vwcs_len(p), fmt, ap); }
);
