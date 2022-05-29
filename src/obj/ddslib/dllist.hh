// -*- c-basic-offset: 2; indent-tabs-mode: nil -*-

/*
 * DDSLib: Dynamic data structures
 * Copyright (C) 2002-3,2005-6,2012,2016  Steven Simpson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 *
 * Author contact: Email to s.simpson at lancaster.ac.uk
 */

#ifndef dllistxx_INCLUDED
#define dllistxx_INCLUDED

namespace ddslib {
  namespace dllist {
    template <class T>
    class header;

    template <class T>
    class loopheader;

    template <class T>
    class element {
      friend class header<T>;
      friend class loopheader<T>;
      T *pr, *ne;

    public:
      T *previous() const { return pr; }
      T *next() const { return ne; }
    };

    template <class T>
    class loopheader {
      T *r;

      T *link(element<T> T::*m, T *p, T *n, T *e) {
        (e->*m).pr = p;
        (e->*m).ne = n;
        ((e->*m).pr->*m).ne = e;
        ((e->*m).ne->*m).pr = e;
        return e;
      }

    public:
      loopheader() : r(0) { }

      int is_empty() const { return !r; }

      T *entry() const { return r; }

      T *insert_after(element<T> T::*m, T *p, T *e) {
        return link(m, p, (p->*m).ne, e);
      }

      T *insert_before(element<T> T::*m, T *n, T *e) {
        return link(m, (n->*m).pr, n, e);
      }

      T *insert(element<T> T::*m, T *e) {
        if (r)
          return insert_after(m, r, e);
        r = e;
        return link(m, e, e, e);
      }

      T *unlink(element<T> T::*m, T *e) {
        if (r == e)
          r = (e->*m).ne;
        if (r == e)
          r = 0;
        ((e->*m).ne->*m).pr = (e->*m).pr;
        ((e->*m).pr->*m).ne = (e->*m).ne;
        (e->*m).pr = (e->*m).ne = 0;
        return e;
      }

      void join(loopheader &b, element<T> T::*m, T *e1, T *e2) {
        r = (e1->*m).pr;
        ((e1->*m).pr->*m).ne = e2;
        ((e2->*m).pr->*m).ne = e1;
        (e1->*m).pr = (e2->*m).pr;
        (e2->*m).pr = r;
        b.r = 0;
      }

      void split(loopheader &b, element<T> T::*m, T *e1, T *e2) {
        r = (e1->*m).pr;
        b.r = (e2->*m).pr;
        ((e1->*m).pr->*m).ne = e2;
        ((e2->*m).pr->*m).ne = e1;
        (e1->*m).pr = b.r;
        (e2->*m).pr = r;
      }
    };

    template <class T>
    class header {
      T *f, *l;

    public:
      T *first() const { return f; }
      T *last() const { return l; }
      int is_empty() const { return !f; }
      header() : f(0), l(0) {}

      class iterator {
        T *e;
        element<T> T::*m;
        const header<T> *h;

        friend class header<T>;
        iterator(const header<T> *h,
                 element<T> T::*m,
                 T *e) : e(e), m(m), h(h) { }
                 
      public:
        friend bool operator ==(const iterator &le, const iterator &ri) {
          return le.h == ri.h && le.m == ri.m && le.e == ri.e;
        }

        friend bool operator !=(const iterator &le, const iterator &ri) {
          return !(le == ri);
        }

        iterator &operator ++(int) {
          e = (e->*m).ne;
          return *this;
        }

        iterator &operator --(int) {
          e = (e->*m).pr;
          return *this;
        }

        T *operator *() const { return e; }
        T *operator ->() const { return e; }
      };

      iterator begin(element<T> T::*m) const {
        return iterator(this, m, first());
      }

      iterator end(element<T> T::*m) const {
        return iterator(this, m, 0);
      }

    private:
      T *link(element<T> T::*m, T *p, T *n, T *e) {
        (e->*m).pr = p;
        (e->*m).ne = n;
        if ((e->*m).pr)
          ((e->*m).pr->*m).ne = e;
        else
          f = e;
        if ((e->*m).ne)
          ((e->*m).ne->*m).pr = e;
        else
          l = e;
        return e;
      }

    public:
      T *insert_after(element<T> T::*m, T *p, T *e)
      { return link(m, p, p ? (p->*m).ne : f, e); }
      T *insert_before(element<T> T::*m, T *n, T *e)
      { return link(m, n ? (n->*m).pr : l, n, e); }
      T *append(element<T> T::*m, T *e)
      { return insert_after(m, l, e); }
      T *prepend(element<T> T::*m, T *e)
      { return insert_before(m, f, e); }
      T *unlink(element<T> T::*m, T *e) {
        if ((e->*m).pr)
          ((e->*m).pr->*m).ne = (e->*m).ne;
        else
          f = (e->*m).ne;
        if ((e->*m).ne)
          ((e->*m).ne->*m).pr = (e->*m).pr;
        else
          l = (e->*m).pr;
        (e->*m).pr = (e->*m).ne = 0;
        return e;
      }
    };
  }
}

#endif
