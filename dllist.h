/*
    DDSLib: Dynamic data structures
    Copyright (C) 2000, 2002  Steven Simpson

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

#ifndef dllist_INCLUDED
#define dllist_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define dllist_hdr(T) struct { T *first; T *last; }
#define dllist_elem(T) struct { T *prev; T *next; }

#define dllist_HDRINIT { 0, 0 }

#define dllist_first(H) ((H)->first)
#define dllist_last(H) ((H)->last)
#define dllist_prev(M,E) ((E)->M.prev)
#define dllist_next(M,E) ((E)->M.next)

#define dllist_init(H) (dllist_first(H) = dllist_last(H) = 0)
#define dllist_isempty(H) (dllist_first(H) == 0)

#define dllist_link(H,M,P,N,E) ((E)->M.prev = (P), (E)->M.next = (N), \
                   *(((E)->M.prev)?&((E)->M.prev)->M.next:&(H)->first) = \
                   *(((E)->M.next)?&((E)->M.next)->M.prev:&(H)->last) = (E))

#define dllist_insertbefore(H,M,N,E) \
  dllist_link(H,M,(N)?(N)->M.prev:(H)->last,N,E)

#define dllist_insertafter(H,M,P,E) \
  dllist_link(H,M,P,(P)?(P)->M.next:(H)->first,E)

#define dllist_append(H,M,E) dllist_insertafter(H,M,(H)->last,E)
#define dllist_prepend(H,M,E) dllist_insertbefore(H,M,(H)->first,E)

#define dllist_unlink(H,M,E) \
  (*(((E)->M.prev)?&(E)->M.prev->M.next:&(H)->first)=(E)->M.next, \
   *(((E)->M.next)?&(E)->M.next->M.prev:&(H)->last)=(E)->M.prev, \
   (E)->M.prev = (E)->M.next = 0)

#define dllist_loophdr(T) struct { T *entry; }

#define dllist_LOOPHDRINIT { 0 }

#define dllist_loopinit(H) ((H)->entry = 0)
#define dllist_loopentry(H) ((H)->entry)
#define dllist_loopisempty(H) ((H)->entry == 0)

#define dllist_looplink(M,P,N,E) ((E)->M.prev = (P), (E)->M.next = (N), \
                   ((E)->M.prev)->M.next = ((E)->M.next)->M.prev = (E))

#define dllist_loopinsertafter(M,P,E) dllist_looplink(M,P,(P)->M.next,E)
#define dllist_loopinsertbefore(M,N,E) dllist_looplink(M,(N)->M.prev,N,E)

#define dllist_loopunlinkdir(H,M,E,D) ((H)->entry == (E) ? \
   (H)->entry = ((E)->M.D == (E) ? 0 : (E)->M.D) : \
   ((E)->M.prev->M.next=(E)->M.next, (E)->M.next->M.prev=(E)->M.prev))

#define dllist_loopunlink(H,M,E) dllist_loopunlinkdir(H,M,E,next)

#define dllist_loopinsert1st(H,M,E) \
   ((H)->entry = (E)->M.prev = (E)->M.next = (E))

#define dllist_loopinsertrel(H,M,E,R) ((H)->entry ? \
   dllist_loopinsert##R(M,(H)->entry,E) : \
   dllist_loopinsert1st(H,M,E))

#define dllist_loopinsert(H,M,E) dllist_loopinsertrel(H,M,E,after)

#define dllist_loopinc(H,M,D) ((H)->entry = (H)->entry ? (H)->entry->M.D : 0)

#ifdef __cplusplus
}
#endif

#endif
