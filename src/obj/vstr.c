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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include <limits.h>

#include "ddslib/vstr.h"

#include "vimpl.h"

IMPL(vstr, vstr, char, int, '\0', "", strlen, memset,
     memmove, memcpy, vsnprintf, 0);

int vstr_wcsmblen(const wchar_t *s, size_t len)
{
  if (!s) return 0;
  static const mbstate_t null;
  mbstate_t state = null;
  char buf[MB_LEN_MAX];
  int res = 0;
  while (len > 0) {
    size_t rc = wcrtomb(buf, *s, &state);
    if (rc == (size_t) -1)
      return -1;
    res += rc;
    len--;
    s++;
  }

  // Return to initial shift state.
  res += wcrtomb(NULL, L'\0', &state) - 1;
  return res;
}
