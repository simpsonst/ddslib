/*
    DDSLib: Dynamic data structures
    Copyright (C) 2005  Steven Simpson

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

#ifndef dllistxx_INCLUDED
#define dllistxx_INCLUDED

namespace ddslib {
  namespace dllist {
    template <class T>
    class header;

    template <class T>
    class element {
      friend header;
      T *pr, *ne;

    public:
      T *previous() const { return pr; }
      T *next() const { return ne; }
    };

    template <class T>
    class header {
      T *f, *l;

    public:
      T *first() const { return f; }
      T *last() const { return l; }
      int is_empty() const { return !f; }
      header() : f(0), l(0) {}

    private:
      T *link(element T::*m, T *p, T *n, T *e) {
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
      T *insert_after(element T::*m, T *p, T *e)
      { return link(m, p, p ? (p->*m).ne : f, e); }
      T *insert_before(element T::*m, T *n, T *e)
      { return link(m, n ? (n->*m).pr : l, n, e); }
      T *append(element T::*m, T *e)
      { return insert_after(m, l, e); }
      T *prepend(element T::*m, T *e)
      { return insert_before(m, f, e); }
      T *unlink(element T::*m, T *e) {
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
