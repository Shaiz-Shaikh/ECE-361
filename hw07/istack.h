#ifndef ISTACK_H
#define ISTACK_H

#include <stdbool.h>
#include <stddef.h>

#define ISTACK_START 8   /* room at first; doubles when full */

/* A stack of int that grows as it fills. The struct is defined only in
   istack.c, so code outside cannot reach inside it (it is "opaque"). */
typedef struct istack istack_t;

/* Takes nothing. Returns a new, empty stack with room for ISTACK_START
   values, or NULL if memory runs out (nothing is leaked). The caller must
   free it with istack_destroy. */
istack_t *istack_create(void);

/* Takes a stack. Frees the stack and everything it holds. NULL does
   nothing. Assumes the pointer is not used again afterwards. */
void istack_destroy(istack_t *s);

/* Takes a stack and a value. Pushes v on top; if the stack is full, first
   doubles the room with realloc. Returns 0, or -1 if s is NULL or memory
   runs out; then the stack is unchanged. */
int istack_push(istack_t *s, int v);

/* Takes a stack and a place for the value. Puts the top value into *v and
   removes it; v may be NULL. Returns 0, or -1 if s is NULL or the stack is
   empty; then *v is unchanged. */
int istack_pop(istack_t *s, int *v);

/* Like istack_pop, but the top value stays on the stack. */
int istack_peek(const istack_t *s, int *v);

/* Takes a stack. Returns how many values it holds; 0 for NULL. */
size_t istack_count(const istack_t *s);

/* Takes a stack. Returns true if it holds no values; true for NULL. */
bool istack_is_empty(const istack_t *s);

/* Takes a stack. Returns the room it has right now (how many values fit
   before it must grow); 0 for NULL. */
size_t istack_capacity(const istack_t *s);

/* Takes a stack and a place for a count. Returns a new array with a copy of
   the values, bottom first, and puts their number in *n. The caller owns
   the array and must free it. Returns NULL and sets *n = 0 if s is NULL or
   empty, or memory runs out. Assumes n is not NULL. */
int *istack_to_array(const istack_t *s, size_t *n);

#endif
