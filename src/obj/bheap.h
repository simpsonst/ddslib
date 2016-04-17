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

#ifndef ddslib_USEDEPRECATED
#error "<bheap.h> is deprecated."
#error "Use <ddslib/bheap.h>, or define ddslib_USEDEPRECATED."
#endif

#include "ddslib/btree.h"
#include "ddslib/internal.h"

#ifndef ddslib_bheap_compat_included
#define ddslib_bheap_compat_included

#ifdef __cplusplus
extern "C" {
#endif

  ddslib_deprecated(typedef bheap bheap_t);
  ddslib_deprecated(typedef bheap_elem bheap_elem_t);

#ifdef __cplusplus
}
#endif

#endif
