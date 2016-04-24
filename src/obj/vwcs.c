/*
    DDSLib: Dynamic data structures
    Copyright (C) 2002-3,2005-6,2012,2016  Steven Simpson

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
#include <wchar.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "ddslib/vwcs.h"

#include "vimpl.h"

#if defined __riscos__ || defined __riscos
/* Compiling with GCC for RISC OS seems to lack a declaration for
   vwsprintf, so we have to disable vinsertf, which will in turn
   disable all of v?(set|insert|append)f. */
#define MODE 2
#define PRINT dummy
static int dummy(wchar_t *buf, size_t len, const wchar_t *fmt, va_list ap)
{
  return -1;
}

#elif defined __WIN32__
/* The Windows declaration of vswprintf is not C95-compliant, but it
   does declare something with identical functionality under a
   different name. */
#define MODE 1
#define PRINT _vsnwprintf

#else
#define MODE 1
#define PRINT vswprintf
#endif

IMPL(vwcs, vwcs, wchar_t, wchar_t, L'\0', L"", wcslen, wmemset,
       wmemmove, wmemcpy, PRINT, MODE);
