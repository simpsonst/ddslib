#include <stdio.h>
#include <assert.h>

#include "btree.h"

struct element;

btree_DECL(number, struct element, int);

struct element {
  int value;
  number_elem others;
};

void print_tree(struct element *root)
{
  int i;

  if (!root) {
    printf("_");
    return;
  }

  printf("%d ", root->value);
  if (number_child(root, btree_LEFT) ||
      number_child(root, btree_RIGHT)) {
    printf("(");
    print_tree(number_child(root, btree_LEFT));
    printf(" ");
    print_tree(number_child(root, btree_RIGHT));
    printf(")");
  }
}

int main()
{
  struct element *root = NULL;
  struct element a, b, c, d, e, f;

  a.value = 7;
  b.value = 9;
  c.value = 2;
  d.value = 8;
  e.value = 5;
  f.value = 11;

  number_init(&a);
  number_init(&b);
  number_init(&c);
  number_init(&d);
  number_init(&e);
  number_init(&f);

  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, a.value, number_links(&a), NULL);
  number_link(&a);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, b.value, number_links(&b), NULL);
  number_link(&b);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, c.value, number_links(&c), NULL);
  number_link(&c);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, d.value, number_links(&d), NULL);
  number_link(&d);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, e.value, number_links(&e), NULL);
  number_link(&e);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_remove(&c, NULL);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, f.value, number_links(&f), NULL);
  number_link(&f);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_remove(&b, NULL);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, c.value, number_links(&c), NULL);
  number_link(&c);
  print_tree(root);
  putchar('\n');
  number_check(root);

  number_find(NULL, &root, b.value, number_links(&b), NULL);
  number_link(&b);
  print_tree(root);
  putchar('\n');
  number_check(root);

  return 0;
}

#define MYCMP(A,B,C) ((B)-(C))

btree_IMPL(number, struct element, int, others, value, MYCMP);
