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

#ifndef alarm_H
#define alarm_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dllist.h"

/* type-independent declarations */
typedef enum {
  alarm_OFF = -1,
  alarm_OK = 0,
  alarm_PENDING = 1,
  alarm_ERROR = 2
} alarm_rc;

enum { alarm_SEQ, alarm_READY, alarm_DONE };

typedef void alarm_f(void *);

/* T must be a typedef name */
#define alarm_DECL(P,T) \
typedef struct P##_tag *P##_h; \
typedef struct P##_db_tag { \
  dllist_hdr(struct P##_tag) seq, ready, done; \
} P##_db; \
extern void          P##_init      (P##_db *); \
extern void          P##_term      (P##_db *); \
extern alarm_rc      P##_poll      (P##_db *, const T *now, T *next); \
extern alarm_rc      P##_polldelay (P##_db *, const T *now, T *delay); \
extern int           P##_process   (P##_db *, const T *now); \
extern int           P##_processn  (P##_db *, const T *now, int n); \
extern P##_h         P##_set       (P##_db *, const T *, alarm_f *, void *); \
extern void          P##_cancel    (P##_db *, P##_h)

#ifdef __cplusplus
}
#endif

#endif
