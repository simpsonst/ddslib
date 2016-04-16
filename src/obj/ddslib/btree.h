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

#ifndef btree_INCLUDED
#define btree_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum { btree_LEFT = 0, btree_RIGHT = 1 } btree_dir;

#define btree_opp(X) (1-(X))

#define btree_DECL(P, T, KT) \
typedef struct { \
  T *child[2], *parent, **holder; \
} P##_elem; \
 \
void P##_init(T *); \
void P##_link(T *); \
KT P##_key(T *); \
P##_elem *P##_links(T *); \
T *P##_child(T *, btree_dir); \
T *P##_parent(T *); \
btree_dir P##_dir(T *); \
T *P##_next(T *np, btree_dir dir); \
P##_elem *P##_find(T *par, T **rootp, KT key, P##_elem *res, void *cmpctxt); \
T *P##_remove(T *np, void *cmpctxt); \
void P##_check(T *root);

#define btree_child(M, OP, D) ((OP)->M.child[D])
#define btree_parent(M, OP) ((OP)->M.parent)

#define btree_dir(M, OP) \
((btree_dir) (((OP)->M.holder - (OP)->M.parent->M.child)))

#define btree_init(M, OP) \
((void) ((OP)->M.child[0] = (OP)->M.child[1] = (OP)->M.parent = 0, \
(OP)->M.holder = 0))

#define btree_link2(M, OP, P, RP) \
((void) (*(RP) = (OP), (OP)->M.holder = (RP), (OP)->M.parent = (P)))

#define btree_link(M, OP) ((void) (*(OP)->M.holder = (OP)))

#define btree_unlink(M, OP) \
((OP)->M.parent = *(OP)->M.holder = 0, (OP)->M.holder = 0)

#define btree_checkchild(M, OP, D) ((OP)->M.child[D] != 0 && \
(assert((OP)->M.child[D]->M.holder == &(OP)->M.child[D]), 1))

#define btree_checknode(M, OP) (btree_checkchild(M, (OP), btree_LEFT), \
btree_checkchild(M, (OP), btree_RIGHT))

#define btree_IMPL(P, T, KT, M, K, CMP) \
void P##_init(T *np) \
{ \
  btree_init(M, np); \
} \
 \
void P##_link(T *np) \
{ \
  btree_link(M, np); \
} \
 \
P##_elem *P##_links(T *np) \
{ \
  return &np->M; \
} \
 \
KT P##_key(T *np) \
{ \
  return np->K; \
} \
 \
btree_dir P##_dir(T *np) \
{ \
  return btree_dir(M, np); \
} \
 \
T *P##_child(T *np, btree_dir d) \
{ \
  return btree_child(M, np, d); \
} \
 \
T *P##_parent(T *np) \
{ \
  return btree_parent(M, np); \
} \
 \
void P##_check(T *root) \
{ \
  if (!root) \
    return; \
  btree_checknode(M, root); \
  P##_check(root->M.child[0]); \
  P##_check(root->M.child[1]); \
} \
 \
P##_elem *P##_find(T *parent, T **rootp, \
      KT key, P##_elem *res, void *cmpctxt) \
{ \
  static P##_elem tmp; \
  int c; \
 \
  if (!res) res = &tmp; \
  res->parent = parent; \
 \
  while (*rootp && (c = CMP(cmpctxt, key, (*rootp)->K)) != 0) { \
    btree_dir dir = c < 0 ? btree_LEFT : btree_RIGHT; \
    res->parent = *rootp; \
    rootp = &(*rootp)->M.child[dir]; \
  } \
  res->holder = rootp; \
  return res; \
} \
 \
T *P##_remove(T *np, void *cmpctxt) \
{ \
  P##_elem tmp = np->M; \
 \
  /* Detach this node from its parent. */ \
  btree_unlink(M, np); \
 \
  /* Detach the children from this node. */ \
  if (tmp.child[0]) \
    btree_unlink(M, tmp.child[0]); \
  if (tmp.child[1]) \
    btree_unlink(M, tmp.child[1]); \
 \
  if (!tmp.child[0]) { \
    /* Make the right child replace this node, if there is no left \
       child. */ \
    if (tmp.child[1]) \
      btree_link2(M, tmp.child[1], tmp.parent, tmp.holder); \
  } else { \
    /* Make the left child replace this node, and find a new place for \
       the right child if it exists. */ \
    btree_link2(M, tmp.child[0], tmp.parent, tmp.holder); \
    if (tmp.child[1]) { \
      P##_find(tmp.parent, tmp.holder, tmp.child[1]->K, \
	       &tmp.child[1]->M, cmpctxt); \
      btree_link(M, tmp.child[1]); \
    } \
  } \
 \
  return np; \
} \
 \
static T *P##_extchild(T *np, btree_dir dir) \
{ \
  while (np->M.child[dir]) \
    np = np->M.child[dir]; \
 \
  return np; \
} \
 \
static T *P##_extparent(T *np, btree_dir dir) \
{ \
  while (np && btree_dir(M, np) != dir) \
    np = np->M.parent; \
  return np->M.parent; \
} \
 \
T *P##_next(T *np, btree_dir dir) \
{ \
  if (np->M.child[dir]) \
    return P##_extchild(np->M.child[dir], btree_opp(dir)); \
 \
  return P##_extparent(np, btree_opp(dir)); \
}

#ifdef __cplusplus
}
#endif

#endif
