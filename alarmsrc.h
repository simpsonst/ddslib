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

#ifndef alarmsrc_INCLUDED
#define alarmsrc_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "dllist.h"
#include "alarm.h"

/*
  int CMP(const T *, const T *);
  void SUB(T *, const T *, const T *);
  void SZ(T *);
 */

#define alarm_DEFN(P,T,CMP,SUB,SZ) \
 \
struct P##_db_tag; \
 \
struct P##_tag { \
  dllist_elem(struct P##_tag) others; \
  T when; \
  alarm_f *fn; \
  void *ctxt; \
  unsigned loc : 2; \
}; \
 \
static int P##_processready(struct P##_db_tag *dbp, const T *now) \
{ \
  int n = 0; \
  P##_h ah = dllist_first(&dbp->ready); \
 \
  if (!ah) return 0; \
  while (ah) { \
    n++; \
    dllist_unlink(&dbp->ready, others, ah); \
    dllist_append(&dbp->done, others, ah); \
    ah->loc = alarm_DONE; \
    if (ah->fn) (*ah->fn)(ah->ctxt); \
    ah = dllist_first(&dbp->ready); \
  } \
  return n; \
} \
 \
static void P##_makeready(struct P##_db_tag *dbp, const T *now) \
{ \
  P##_h ah; \
 \
  while ((ah = dllist_first(&dbp->seq)) && CMP(now, &ah->when) > 0) { \
    dllist_unlink(&dbp->seq, others, ah); \
    ah->loc = alarm_READY; \
    dllist_append(&dbp->ready, others, ah); \
  } \
} \
 \
static void P##_makereadyn(struct P##_db_tag *dbp, const T *now, int n) \
{ \
  P##_h ah; \
 \
  while (n > 0 && \
         (ah = dllist_first(&dbp->seq)) && CMP(now, &ah->when) > 0) { \
    n--; \
    dllist_unlink(&dbp->seq, others, ah); \
    ah->loc = alarm_READY; \
    dllist_append(&dbp->ready, others, ah); \
  } \
} \
 \
void P##_init(struct P##_db_tag *dbp) \
{ \
  dllist_init(&dbp->seq); \
  dllist_init(&dbp->ready); \
  dllist_init(&dbp->done); \
} \
 \
void P##_term(struct P##_db_tag *dbp) \
{ \
  P##_h ah; \
 \
  while ((ah = dllist_first(&dbp->seq))) \
    P##_cancel(dbp, ah); \
  while ((ah = dllist_first(&dbp->ready))) \
    P##_cancel(dbp, ah); \
  while ((ah = dllist_first(&dbp->done))) \
    P##_cancel(dbp, ah); \
} \
 \
P##_h P##_set(struct P##_db_tag *dbp, const T *tv, alarm_f *f, void *c) \
{ \
  P##_h is, ah; \
 \
  ah = malloc(sizeof(struct P##_tag)); \
  if (!ah) return NULL; \
 \
  ah->when = *tv; \
  ah->fn = f; \
  ah->ctxt = c; \
  ah->loc = alarm_SEQ; \
 \
  is = dllist_last(&dbp->seq); \
 \
  while (is && CMP(&ah->when, &is->when) < 0) \
    is = dllist_prev(others, is); \
 \
  dllist_insertafter(&dbp->seq, others, is, ah); \
 \
  return ah; \
} \
 \
void P##_cancel(struct P##_db_tag *dbp, P##_h ah) \
{ \
  if (!ah) return; \
 \
  switch (ah->loc) { \
  case alarm_SEQ: \
    dllist_unlink(&dbp->seq, others, ah); \
    break; \
  case alarm_READY: \
    dllist_unlink(&dbp->ready, others, ah); \
    break; \
  case alarm_DONE: \
    dllist_unlink(&dbp->done, others, ah); \
    break; \
  } \
 \
  free(ah); \
} \
 \
alarm_rc P##_poll(struct P##_db_tag *dbp, const T *now, T *next) \
{ \
  P##_h af; \
 \
  af = dllist_first(&dbp->seq); \
  if (!af) return alarm_OFF; \
 \
  if (CMP(now, &af->when) >= 0) { \
    *next = *now; \
    return alarm_PENDING; \
  } \
 \
  *next = af->when; \
  return alarm_OK; \
} \
 \
alarm_rc P##_polldelay(struct P##_db_tag *dbp, const T *now, T *next) \
{ \
  P##_h af; \
 \
  af = dllist_first(&dbp->seq); \
  if (!af) return alarm_OFF; \
 \
  if (CMP(now, &af->when) >= 0) { \
    SZ(next); \
    return alarm_PENDING; \
  } \
 \
  SUB(next, &af->when, now); \
  return alarm_OK; \
} \
 \
int P##_process(struct P##_db_tag *dbp, const T *now) \
{ \
  P##_makeready(dbp, now); \
  return P##_processready(dbp, now); \
} \
 \
int P##_processn(struct P##_db_tag *dbp, const T *now, int n) \
{ \
  P##_makereadyn(dbp, now, n); \
  return P##_processready(dbp, now); \
} \
extern int P##_processn(struct P##_db_tag *, const T *, int)

#ifdef __cplusplus
}
#endif

#endif
