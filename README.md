# Purpose

This is a C library for manipulating miscellaneous dynamic data structures:

1. Doubly-linked lists

1. Binary heaps

1. Binary trees

1. Hash tables

1. Variable-length strings

The first two get used a fair amount in the author's other libraries, so they should be quite robust.
The rest are really just thrown in for good measure, and haven't seen a lot of use.
Most code pre-dates C99, and so could benefit from a rewrite taking advantage of new features, like the `__Generic` operator.


# Installation

Installation of this library requires [Binodeps](https://github.com/simpsonst/binodeps).

To install in `/usr/local`, the usual commands should work with the default configuration options:

```
$ make
$ sudo make install
```

However, you may want to configure compilation options or install in a different location.
You can override the default configuration by either:

1. specifying variables on the command line, or by

1. placing them in a file called `ddslib-env.mk` in either:

   - the source directory, or in

   - the path that make searches (e.g., `/usr/local/include` or `/usr/include`, or any extra directories you have configured).

For example, you could have this file placed in `~/.local/include/include-config`:

```
## Example ddslib-env.mk

## C options, language variant and optimisations
CC=gcc
CFLAGS += -std=gnu99
CFLAGS += -O2

## C++ options, language variant and optimisations
CXX=g++
CXXFLAGS += -std=gnu++98
CXXFLAGS += -O2

## Place to install the library
PREFIX=$(HOME)/.install

## To disable the compilation and installation of
## problematic headers if you don't have a full C99
## implementation:
#ENABLE_C99=no

## Similarly for C++:
#ENABLE_CXX=no

## Create ZIP for expansion on RISC OS:
ENABLE_RISCOS=yes
```

...and the environment variable `MAKEFLAGS` set to `"-I${HOME}/.install/include-config"`.
When you run GNU make, the settings above override the defaults.

# Doubly-linked lists

```
#include <ddslib/dllist.h>
```

This header assumes that you want to maintain a list of structures in which you've incorporated the necessary ‘previous’ and ‘next’ pointers.

The list can be open, i.e., it has a start and an end, or closed, i.e., it loops continuously.

As an example, let's assume that we have a structure for each list element:

```
struct mydata {
  int m1;
  char m2;
  /* ... */

  dllist_elem(struct mydata) others;
};
```

The member `others` provides the necessary pointers for linking.

## Open lists

An open list of elements has the type `dllist_hdr(struct mydata)`.
A `typedef` for the header type is recommended if you intend to pass headers around, otherwise you might be asking the compiler to compare identical, anonymous types:

```
typedef dllist_hdr(struct mydata) mylist;
```

We can declare and initialise an empty, open list with:

```
mylist list = dllist_HDRINIT;
```

...or initialise with:

```
dllist_init(&list);
```

Now if we have several elements to add:

```
struct mydata e1, e2, e3, e4;
```

...we can add these to the list in various ways:

```
/* add to end */
dllist_append(&list, others, &e1);

/* add to start */
dllist_prepend(&list, others, &e2);

/* add after e2 */
dllist_insertafter(&list, others, &e2, &e3);

/* add before e3 */
dllist_insertbefore(&list, others, &e3, &e4);
```

...putting the elements in the order `e2`, `e4`, `e3`, `e1`.

To traverse a list:

```
struct mydata *p;

for (p = dllist_first(&list); p; p = dllist_next(others, p)) {
  /* ... */
}
```

Similarly, there are macros `dllist_last` and `dllist_prev`.

`dllist_isempty(&list)` is non-zero if the list is empty.

`dllist_unlink(&list, others, &e2)` removes element `e2` from the list.

## Closed lists

This form uses a simple header — just a pointer to any one of the elements:

```
typedef dllist_loophdr(struct mydata) mylooplist;
mylooplist loop = dllist_LOOPHDRINIT;

/* or */
dllist_loopinit(&loop);
```

To add elements:

```
/* adding elements in arbitrary order */
dllist_loopinsert(&loop, others, &e1);
dllist_loopinsert(&loop, others, &e2);

/* insert e3 after e1 */
dllist_loopinsertafter(others, &e1, &e3);

/* insert e4 before e1 */
dllist_loopinsertbefore(others, &e1, &e4);
```

To traverse this list:

```
struct mydata *p, *s;

p = s = dllist_loopentry(&loop);
do {
  /* ... */
  p = dllist_next(others, p);
} while (p != s);
```

To remove an element:

```
dllist_loopunlink(&loop, others, &e2);
```

If the element removed is the current head, you may want to specify whether to move it to the next or to the previous element:

```
dllist_loopunlinkdir(&loop, others, &e2, next);
/* or */
dllist_loopunlinkdir(&loop, others, &e2, prev);
```

To test for an empty list, use `dllist_loopisempty(&loop)`.

To change move the head pointer by one element, use:

```
dllist_loopinc(&loop, next);
/* or */
dllist_loopinc(&loop, prev);
```

# Binary trees

[Note: I've made the binary-heap code generic in a much better way than I have for binary trees, so I may later implement trees the same way.]

Suppose you want to maintain a binary tree of struct number objects, whose key fields are of type `int`.
You also want to traverse the tree in any direction (left/right child; parent). A header file is included to maintain the necessary information

You need to declare that the structure type exists, then declare types and functions to manipulate such structures, and then you can complete your structure definition.
The following is suitable for a header file:

```
#include <ddslib/btree.h>

/* user's structure type */
struct element;

btree_DECL(number, struct element, int);

struct element {
  int value;
  number_elem others;
};
```

The first argument of btree_DECL is a prefix:
all functions and types declared by the macro will have this prefix.
The second is the element type, and the third is the key type.

You also need to provide an implementation.
Make sure the above declarations are in scope, then define a macro of the form `CMP(void *, int, int)` (i.e., the parameters are keys), before providing it as part of the implementation:

```
#define MYCMP(A,B,C) ((B)-(C))

btree_IMPL(number, struct element, int, others, value, MYCMP);
```

The first three arguments are as before.
The fourth is the name of the member of the element type that will hold the pointers necessary for maintaining a binary tree (which we'll call the ‘link member’).
The fifth is the name of the member containing an element's key.
The sixth is the comparison macro.

With the above declarations in scope, you can use the following functions:

```
void number_init(struct element *ep);
```

Initialise the element.
This is mainly to ensure that the child links are null.

```
number_elem *number_find(struct element *parent,
                         struct element **rootp, int key,
                         number_elem *res,
                         void *cmpctxt);
```

Find the position for the given key.
`*rootp` points to the root element on entry (and may be null), and points to the found element (or is null if not found) on exit.
`*parent` should be the element containing `*rootp` on entry, or null if the pointer does not belong to an element (such as the pointer to the true root of the tree).
`cmpctxt` is passed as the first argument to the comparator.
`*res` (or a static object if `res==0`) will hold the necessary connection information for `number_link()` (see below).
`res` (or a pointer to the static object) is returned.

```
void number_link(struct element *np);
```

Insert the element into the tree.
It is assumed that a call to `number_find` has been called, the element's key is the same as the just sought key, and the link member has been copied from `*res`.
For example:

```
#include <stddef.h>

struct element *root;
struct element newelem;

newelem.value = /* ... */;
number_find(NULL, &root, newelem.value,
            number_links(&newelem), NULL);
number_link(&newelem);
```

```
struct element *number_parent(struct element *);
```

Get the parent of the given element.

```
struct element *number_child(struct element *np,
                             btree_dir dir);
```

Get the left (`dir==btree_LEFT`) or right (`dir==btree_RIGHT`) child of the given element.

```
struct element *number_next(struct element *np,
                            btree_dir dir);
```

Get the element before (`dir==btree_LEFT`) or after (`dir==btree_RIGHT`) the given element.

```
struct element *number_remove(struct element *np,
                              void *cmpctxt);
```

Remove the element.
Other parts of the tree may be moved around to maintain it, so a comparison context is needed.

# Binary heaps

A binary heap maintains a sequence of elements allowing rapid insertion and removal at any position in the sequence.
The header `<ddslib/bheap.h>` defines types, macros and functions for maintaining a binary heap of identical structures in a user-defined order.

The user is expected to define a type to represent a heap element, and ensure that it contains a member of the type `bheap_elem`, for example:

```
#include <ddslib/bheap.h>

struct myelem {
  int value;
  bheap_elem others;
};
```

The user must also define an ordering for a heap, by providing a comparison function and a context:

```
struct mycontext ctxt;
int mycmp(void *cp, const void *a, const void *b);
```

`mycmp` should expect `cp` to point to `ctxt`, and `a` and `b` to point to the two elements to be compared, returning:
a negative integer if the first should appear in the sequence before the second;
a positive integer if the first should appear in the sequence after the second;
zero if the ordering of the two elements is unimportant.

Now the user can create an object of type `bheap` to represent a heap, which must be initialised with:

```
bheap myheap;

bheap_init(&myheap, struct myelem, others, &ctxt, &mycmp);
```

The heap is maintained using the following functions or function-like macros:

```
void *bheap_peek(bheap *h);
```

Obtain the element at the start of the sequence `h`, or `NULL` if there is none.

```
void bheap_insert(bheap *h, void *p);
```

Insert an element pointed to by `p` at the correct position within the sequence `h`.

```
void bheap_remove(bheap *h, void *p);
```

Remove the element pointed to by `p` from the sequence `h`.

```
void *bheap_pop(bheap *h);
```

Remove the first element from the sequence `h`, and return a pointer to it.

You need to link the library `ddslib` with your program in order to use these functions.

Note that no memory allocation is performed — the user provides that himself.

# Hash tables

```
#include <ddslib/htab.h>
```

This header defines a type `htab` representing a hash table for hashing any type to any other.
Keys and values are represented by the unions `htab_const` and `htab_obj`, which are identical except that the pointer member in one of them points to a const object.

## Creating a hash table

To create hash table, you need to define several functions to manipulate data that will go in it.
Firstly, a function to compute a hash code for any given key is required:

```
size_t my_hash(void *context, htab_const key);
```

The functions `htab_hash_str` and `htab_hash_wcs` are already provided to compute hash codes from null-terminated strings (multibyte and wide, respectively).

Also required is a function to compare a sought key against one already in the table:

```
int my_cmp(void *context, htab_const sought, htab_const extant);
```

The functions `htab_cmp_str` and `htab_cmp_wcs` are already provided to compare null-terminated strings (multibyte and wide, respectively).

The remaining functions are optional, and can be `NULL`.

Two functions, possibly the same one, can be specified to copy a key or value:

```
htab_obj my_copykey(void *context, htab_const key);
htab_obj my_copyvalue(void *context, htab_const value);
```

The functions `htab_copy_str` and `htab_copy_wcs` are already provided to copy null-terminated strings (multibyte and wide, respectively) by dynamically allocating with `malloc`.

The remaining two functions, which may also be identical, can be specified to release a key or value object:

```
void my_freekey(void *context, htab_obj key);
void my_freevalue(void *context, htab_obj value);
```

A single function `htab_release_free` is provided to release anything allocated using `malloc` by calling `free`.

With these functions in place, you can call:

```
htab my_table;

my_table = htab_open(size,
                     context,
                     &my_hash,
                     &my_cmp,
                     &my_copykey,
                     &my_copyvalue,
                     &my_freekey,
                     &my_freevalue);
```

A `NULL` return indicates failure.

The `context` argument is supplied on each call to configuration functions.

## Destroying a hash table

To discard a table after use, call:

```
htab_close(my_table);
```

`htab_close(NULL)` has no effect.

## Insertion

To place an entry in the table, use:

```
if (htab_put(my_table, key, value)) {
  // Insertion was successful.
}
```

The old value, if any, will be released using the deallocation function specified when the table was created.
If you want to extract it instead, use:

```
htab_obj oldvalue;

switch (htab_rpl(my_table, key, &oldvalue, value)) {
case htab_REPLACED:
  // There was a previous value.
  break;
case htab_ERROR:
  // Insertion failed.
  break;
case htab_OKAY:
  // Insertion was successful.
  break;
}
```

## Removal

To remove an entry from the table, use:

```
if (htab_del(my_table, key)) {
  // There was an old value.
}
```

The old value will be released using the deallocation function specified when the table was created.
If you want to extract it instead, use:

```
htab_obj value;
if (htab_pop(my_table, key, &value)) {
  // There was an old value.
}
```

## Inspection

You can obtain the value for a specific key with:

```
htab_obj value;
if (htab_get(my_table, key, &value)) {
  // Value found...
}
```

Give `NULL` as the third argument if you only want to test for existence, or use the equivalent `htab_tst(my_table, key)`.

## Traversal

To apply a function of the following form:

```
htab_apprc my_func(void *app_ctxt, htab_const key, htab_obj value);
```

...to all entries in a table, use:

```
htab_apply(my_table, app_ctxt, &my_func);
```

The function should return a bit-wise OR of the following values, as required:

```
htab_REMOVE
```

Remove the entry just supplied.

```
htab_STOP
```

Halt the traversal.

The function should not otherwise attempt to modify the table.

## Adaptation functions

Some functions are provided to conveniently adapt the hash-table interface to the types it actually uses.

```
CV htab_getST(htab, K key);
V htab_popST(htab, K key);
V htab_rplST(htab, K key, CV val);
_Bool htab_putST(htab, K key, CV val);
_Bool htab_tstST(htab, K key);
_Bool htab_delST(htab, K key);
```

Each corresponds to one of the native functions already described, except that the get and pop calls return the current/removed value, or a ‘miss’ value.

The `S` character indicates the key type `K`, while `T` corresponds to the value type `V`.
`CV` is the unmodifiable version of `V` — where a `CV` is returned, the table still holds that value, and it should not be released;
otherwise, the caller is expected to be responsible for releasing it.

| S or T | Type | Field of `htab_obj` or `htab_const` | ‘Miss’ value |
| s | Null-terminated `char` string | pointer | NULL |
| w | Null-terminated `wchar_t` stting | pointer | NULL |
| p | `void*` | pointer | `NULL` |
| u | `uintmax_t` | unsigned_integer | 0 |

The suites for the following `ST` combinations are defined:

- `sp`
- `ss`
- `wp`
- `ww`
- `ws`
- `sw`
- `pp`
- `su`

More may be added in future.
You can add your own static ones with (for example):

```
htab_IMPL(suffix, key-type,
          value-type, const-value-type,
          static inline, key-member, value-member,
          ‘miss’-value);
```

# Variable-length strings

```
#include <ddslib/vstr.h>
#include <ddslib/vwcs.h>
```

These headers respectively define the types vstr and vwcs to hold strings of multibyte or wide characters.
They are held as a base address and a length, and include a capacity which is at least the length.
Insertions and deletions automatically adjust the capacity as necessary.
Null-terminated, measured or printf-format strings can be inserted.
Strings can be made null-terminated for compatibility easily.

The APIs for wide-character strings are identical to multibyte-character strings, except for changing the prefix `vstr_` to `vwcs_`, and `char` to `wchar_t`.
The rest of this documentation describes only the `vstr_*` functions.

## Null state

A non-zero-initialised variable variable should be initialised with `vstr_NULL`.
In this state, it is safe to destroy.
To reset to this state, use:

```
vstr str;
vstr_reset(&str);
```

## Interoperability

Use `vstr_get` to obtain the address of the first character (or a null pointer if in its null state), and `vstr_len` to obtain its length.
For example, to print out a string containing no null characters:

```
printf("%.*s", (int) vstr_len(&str), vstr_get(&str));
```

`vstr_term` will ensure that the string terminates with a null character.
If it already does, the operation does nothing.
The `vstr_unterm` function performs the inverse operation.

```
vstr_term(&str);
vstr_unterm(&str); // Should always be successful.
```

They return zero on success or `-1` on error.

## Insertions

To make space for inserting len characters starting at position start, use:

```
vstr_splice(&str, start, len);
```

To insert an array of characters, including nulls:

```
vstr_insertn(&str, start, "Hello", 5);
```

To insert a single character several times:

```
vstr_insertc(&str, start, 'H', 5);
```

To insert a null-terminated string, excluding the null:

```
vstr_insert(&str, start, "Hello");
```

`vstr_insertf` takes a printf-style format string and a variable number of arguments:

```
vstr_insertf(&str, start, "Size %d x %d", width, height);
```

`vstr_vinsertf` is similar but takes a `va_list` instead of the variable arguments.

There is also a suite of functions which insert from another `vstr`.
In regular-expression format, their names are: `vstr_insertv[ir]?n?`.
For each of these, the first two arguments are as before: `vstr*` of the string to be modified, and a `size_t` giving the insertion point.
The next argument is then a `const vstr*`, giving the source string.
An `i` or `r` in the name indicates that a further `size_t` is required to specify the start point within the source string (`i` means ‘index from the start’;
`r` means ‘index from the end’).
An `n` indicates an extra `size_t` specifying the maximum number of characters to be copied.

All insertion functions clamp the insertion point to within the original array.
For each `vstr_*insert*` function, there is a corresponding `vstr_*append*` function which inserts at the end — the second argument to the insertion function, the string index, is absent.

All the insertion functions and `vstr_splice` return zero on success and `-1` on failure.

## Inserting a wide-character string into a multibyte string

If you want to write a wide-character string into a vstr, ensure the locale is set to that snprintf will convert correctly.
If the wide-character string is at `wbase`, and is null-terminated, this should be enough:

```
vstr_insertf(&str, start, "%ls", wbase);
```

However, if the string is not null-terminated, but you have its length (in `wlen`, below), you might expect the following to be reliable:

```
vstr_insertf(&str, start, "%.*ls", (int) wlen, wbase);
```

Unfortunately, if the inserted string contains wide characters that will be converted into multibyte characters, you will find some of the trailing characters truncated.
This is because the precision field is specified in terms of output bytes, not input wide characters (or output multibyte characters).
You need to compute how many actual bytes are generated by the wide-character string first, and a utility `vstr_wcsmblen` is provided for that:

```
vstr_insertf(&str, start, "%.*ls", vstr_wcsmblen(wbase, wlen), wbase);
```

The function is also suitable for `snprintf` and related functions, as `vstr_insertf` simply passes the format string and arguments straight to that.
Removals

To remove several characters, use:

```
// Remove characters 7, 8, 9, 10.
vstr_elide(&str, 7, 3);
```

`vstr_relide` is similar, but indexed from the end, and reaching towards the start.
For example:

```
// Remove the last seven characters.
vstr_relide(&str, 0, 7);
```

`vstr_elect` and `vstr_relect` provide the complements of `vstr_elide` and `vstr_relide` respectively.
They remove the head and tail of the string, leaving the specified centre.
For example:

```
// Keep the last seven characters.
vstr_relect(&str, 0, 7);
```

To ensure that the string is no longer than a certain number of characters by removing the tail, use:

```
vstr_truncate(&str, len);
```

It has no effect if the string is already that short, or more so.
```vstr_rtruncate``` is similar, but indexed from the end.

To remove all characters without adjusting the capacity, use:

```
vstr_clear(&str);
```

This doesn't change the string to a null pointer, although it might be already.

To make an empty, but non-null string, use:

```
if (vstr_empty(&str) < 0) {
  // Memory allocation failed.
}
```

## Optimisation

The following makes the string capacity match the length exactly, so it uses the minimum amount of memory:

```
vstr_compact(&str);
```

`vstr_setcap` tries to set the capacity to the specified amount, but won't truncate the string.
`vstr_ensure` is similar, but never reduces the capacity:

```
vstr_setcap(&str, 100);
vstr_ensure(&str, 100);
```

They return 0 on success, and -1 on failure — either the memory could not be allocated, or the string would have been truncated.

## Extraction

If you don't want your string to be managed by the library any more, you can extract it with:

```
char *ptr = vstr_extract(&str);
```

`str` becomes `vstr_NULL`, but `ptr` now points to a dynamically allocated string suitable for use with `free` or `realloc`.
`vstr_extract` does not null-terminate the string, but does compact it, before returning it.
