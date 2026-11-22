/* store.c: the store as a binary search tree.
 *
 * Each distinct key (a 0.5 C step, see record_key) is ONE tree node.
 * The node holds a list of every record with that key, in arrival order.
 * So the height depends on how many different keys were seen, not on how
 * many readings arrived.
 */

#include <math.h>
#include <stdlib.h>
#include "store.h"

/* rec_node: one record inside a key's list */
struct rec_node {
    record_t rec;
    struct rec_node *next;
};

/* tree_node: one distinct key */
struct tree_node {
    struct rec_node *first;     /* earliest arrival with this key */
    struct rec_node *last;      /* latest arrival, so adding is quick */
    struct tree_node *left;     /* keys below this one */
    struct tree_node *right;    /* keys above this one */
};

struct store {
    struct tree_node *root;
    int n;                      /* records in all the lists together */
};

int record_key(const record_t *r)
{
    return (int)floor((double)r->reading.temp_c * 2.0);
}

int record_compare(const record_t *a, const record_t *b)
{
    int ka = record_key(a);
    int kb = record_key(b);

    if (ka < kb)
        return -1;
    if (ka > kb)
        return 1;
    return 0;
}

store_t *store_create(void)
{
    store_t *s = malloc(sizeof *s);

    if (s == NULL)
        return NULL;
    s->root = NULL;
    s->n = 0;
    return s;
}

/* free_tree: frees both subtrees first, then the node's list, then
   the node itself (post-order, like bst_destroy). */
static void free_tree(struct tree_node *node)
{
    struct rec_node *rec;
    struct rec_node *next;

    if (node == NULL)
        return;
    free_tree(node->left);
    free_tree(node->right);

    rec = node->first;
    while (rec != NULL) {
        next = rec->next;       /* save it before the free */
        free(rec);
        rec = next;
    }
    free(node);
}

void store_destroy(store_t *s)
{
    if (s == NULL)
        return;
    free_tree(s->root);
    free(s);
}

/* store_insert: copy the record into a new rec_node first. Then walk
   down from the root: less goes left, more goes right, equal means the
   key already has a node, so the record goes at the end of its list.
   At an empty slot we make a new tree_node. If a malloc fails, whatever
   was allocated is freed and the store is unchanged. */
int store_insert(store_t *s, const record_t *r)
{
    struct rec_node *rec;
    struct tree_node *node;
    struct tree_node *parent;
    int cmp;

    if (s == NULL || r == NULL)
        return -1;

    rec = malloc(sizeof *rec);
    if (rec == NULL)
        return -1;
    rec->rec = *r;
    rec->next = NULL;

    parent = NULL;
    cmp = 0;
    node = s->root;
    while (node != NULL) {
        cmp = record_compare(r, &node->first->rec);
        if (cmp == 0)
            break;
        parent = node;
        node = (cmp < 0) ? node->left : node->right;
    }

    if (node != NULL) {
        /* the key is already there: add to the end of its list */
        node->last->next = rec;
        node->last = rec;
    } else {
        /* an empty slot: make a new node for this key */
        node = malloc(sizeof *node);
        if (node == NULL) {
            free(rec);
            return -1;
        }
        node->first = rec;
        node->last = rec;
        node->left = NULL;
        node->right = NULL;
        if (parent == NULL)
            s->root = node;
        else if (cmp < 0)
            parent->left = node;
        else
            parent->right = node;
    }
    s->n++;
    return 0;
}

/* store_find: walk down comparing keys. node is checked for NULL before
   we look inside it (the thing the original search got wrong). */
const record_t *store_find(const store_t *s, int key)
{
    const struct tree_node *node;
    int k;

    if (s == NULL)
        return NULL;
    node = s->root;
    while (node != NULL) {
        k = record_key(&node->first->rec);
        if (key == k)
            return &node->first->rec;
        node = (key < k) ? node->left : node->right;
    }
    return NULL;
}

/* walk_node: in order. The left subtree first (smaller keys), then this
   key's whole list from first to last, then the right subtree. */
static void walk_node(const struct tree_node *node,
                      void (*visit)(const record_t *r, void *arg), void *arg)
{
    const struct rec_node *rec;

    if (node == NULL)
        return;
    walk_node(node->left, visit, arg);
    for (rec = node->first; rec != NULL; rec = rec->next)
        visit(&rec->rec, arg);
    walk_node(node->right, visit, arg);
}

void store_walk(const store_t *s,
                void (*visit)(const record_t *r, void *arg), void *arg)
{
    if (s == NULL || visit == NULL)
        return;
    walk_node(s->root, visit, arg);
}

int store_count(const store_t *s)
{
    return s == NULL ? 0 : s->n;
}

/* height_of: the number of nodes on the longest path down; an empty tree
   is 0. A node is a key, so 100 equal readings still give height 1. */
static int height_of(const struct tree_node *node)
{
    int left;
    int right;

    if (node == NULL)
        return 0;
    left = height_of(node->left);
    right = height_of(node->right);
    return 1 + (left > right ? left : right);
}

int store_height(const store_t *s)
{
    return s == NULL ? 0 : height_of(s->root);
}
