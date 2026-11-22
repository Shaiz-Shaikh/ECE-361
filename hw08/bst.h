#ifndef BST_H
#define BST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct bst bst_t;          /* opaque */

/* bst_create: takes nothing. Returns a new, empty tree, or NULL if
 * memory runs out. The caller owns the tree and must call
 * bst_destroy on it. */
bst_t *bst_create(void);

/* bst_destroy: takes a tree. Frees every node and the tree itself.
 * NULL does nothing. The tree must not be used afterwards. */
void bst_destroy(bst_t *t);

/* bst_insert: takes a tree and a key. Adds the key in a new node:
 * smaller keys go left, all others go right, so an equal key is
 * kept, to the right. Returns 0 on success, or -1 if t is NULL or
 * memory runs out (then the tree is unchanged). */
int bst_insert(bst_t *t, int key);

/* bst_contains: takes a tree, a key, and visits, which may be NULL.
 * Returns true if key is in the tree. If visits is not NULL,
 * *visits is the number of nodes compared, found or not. For a NULL
 * t it returns false and *visits is 0. It prints nothing and does
 * not change the tree. */
bool bst_contains(bst_t *t, int key, size_t *visits);

/* bst_size: takes a tree. Returns the number of keys in it. For a
 * NULL t it returns 0. */
size_t bst_size(const bst_t *t);

/* bst_height: takes a tree. Returns the number of nodes on the
 * longest path from the root down: 0 for an empty tree, 1 for a
 * single node. For a NULL t it returns 0. */
size_t bst_height(const bst_t *t);

#endif
