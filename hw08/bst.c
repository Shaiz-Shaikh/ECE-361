#include <stdlib.h>
#include "bst.h"

/* One node: the original's struct node, with the same names. */
struct node {
    int data;
    struct node *leftChild;
    struct node *rightChild;
};

/* The tree. The original's global root is now a field, so every
 * tree has its own root. size is how many keys it holds. */
struct bst {
    struct node *root;
    size_t size;
};

/* bst_create: new in this port. The original had a global root. */
bst_t *bst_create(void)
{
    bst_t *t = malloc(sizeof *t);

    if (t == NULL)
        return NULL;
    t->root = NULL;
    t->size = 0;
    return t;
}

/* bst_insert: ported from the original insert(). The global root is
 * now t->root, malloc is checked, and the size is counted. */
int bst_insert(bst_t *t, int key)
{
    struct node *tempNode;
    struct node *current;
    struct node *parent;

    if (t == NULL)
        return -1;
    tempNode = malloc(sizeof *tempNode);
    if (tempNode == NULL)
        return -1;
    tempNode->data = key;
    tempNode->leftChild = NULL;
    tempNode->rightChild = NULL;

    /* if the tree is empty */
    if (t->root == NULL) {
        t->root = tempNode;
    } else {
        current = t->root;
        parent = NULL;

        while (1) {
            parent = current;

            /* go to the left of the tree */
            if (key < parent->data) {
                current = current->leftChild;
                if (current == NULL) {      /* insert to the left */
                    parent->leftChild = tempNode;
                    break;
                }
            } else {                        /* go to the right of the tree */
                current = current->rightChild;
                if (current == NULL) {      /* insert to the right */
                    parent->rightChild = tempNode;
                    break;
                }
            }
        }
    }
    t->size++;                              /* one more key */
    return 0;
}

/* bst_contains: ported from the original search(). The global root is
 * now t->root, the printing is gone (visits counts instead), and it
 * returns true or false. Fixed: the original read current->data before
 * checking for NULL, so it crashed on an empty tree. */
bool bst_contains(bst_t *t, int key, size_t *visits)
{
    struct node *current;
    size_t count = 0;

    if (visits != NULL)
        *visits = 0;
    if (t == NULL)
        return false;
    current = t->root;
    if (current == NULL)                    /* the fix: an empty tree */
        return false;

    while (current->data != key) {
        count++;                            /* was: printf("%d ", current->data) */

        /* data at current node > key we are looking for,
         * so go to the left tree */
        if (current->data > key) {
            current = current->leftChild;
        }
        /* else go to the right tree */
        else {
            current = current->rightChild;
        }

        /* not found */
        if (current == NULL) {
            if (visits != NULL)
                *visits = count;
            return false;
        }
    }

    if (visits != NULL)
        *visits = count + 1;                /* the node that matched */
    return true;
}

/* bst_size: the count kept by bst_insert. */
size_t bst_size(const bst_t *t)
{
    return t == NULL ? 0 : t->size;
}

/* height_of: the number of nodes on the longest path from n down.
 * An empty spot has height 0; a node is 1 plus its taller child. */
static size_t height_of(const struct node *n)
{
    size_t left;
    size_t right;

    if (n == NULL)
        return 0;
    left = height_of(n->leftChild);
    right = height_of(n->rightChild);
    return 1 + (left > right ? left : right);
}

/* bst_height: new in this port. */
size_t bst_height(const bst_t *t)
{
    if (t == NULL)
        return 0;
    return height_of(t->root);
}

/* free_nodes: frees n and everything below it. The children are freed
 * first, because n holds the only pointers to them. */
static void free_nodes(struct node *n)
{
    if (n == NULL)
        return;
    free_nodes(n->leftChild);
    free_nodes(n->rightChild);
    free(n);
}

/* bst_destroy: new in this port. The original never freed anything. */
void bst_destroy(bst_t *t)
{
    if (t == NULL)
        return;
    free_nodes(t->root);
    free(t);
}
