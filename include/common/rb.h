/* rb.h - Red Black trees
 *
 * Copyright (c) 2016, Eric Chai <electromatter@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef COMMON_RB_H
#define COMMON_RB_H

#include <stdlib.h>

#ifndef containerof
#include <stddef.h>
#include <sys/types.h>

static inline void *psub_or_null(const void *ptr, ssize_t delta)
{
	if (ptr == NULL)
		return NULL;
	return (char *)ptr - delta;
}

#define containerof(ptr, type, member)					\
	((type *)psub_or_null((0 ? &((type *)NULL)->member : (ptr)),	\
			offsetof(type, member)))
#endif

struct rbnode {
	struct rbnode *parent, *left, *right;
	int red;
};

struct rbtree {
	struct rbnode *root;
	struct rbnode nil;
};

/* compares left and right and returns:
 * <0	if left < right
 * =0	if left == right
 * >0	if left > right
 */
typedef int (*rbcmp_t)(const struct rbnode *left, const struct rbnode *right);

static inline
void rbnode_init(struct rbnode *node)
{
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	node->red = 0;
}

static inline
void rbtree_init(struct rbtree *tree)
{
	rbnode_init(&tree->nil);
	tree->root = &tree->nil;
}

/* finds the first node in a subtree */
struct rbnode *rbsubfirst(struct rbnode *node);

/* finds the last node in a subtree */
struct rbnode *rbsublast(struct rbnode *node);

/* finds the in-order successor */
struct rbnode *rbnext(struct rbnode *node);

/* finds the post-order successor */
struct rbnode *rbnextpost(struct rbnode *node);

/* finds the in-order predecessor */
struct rbnode *rbprev(struct rbnode *node);

/* finds the in-order first node in tree */
static inline
struct rbnode *rbfirst(struct rbtree *tree)
{
	return rbsubfirst(tree->root);
}

/* finds the in-order last node in tree */
static inline
struct rbnode *rblast(struct rbtree *tree)
{
	return rbsublast(tree->root);
}

/* finds the in-order successor of key regardless of if key is in tree */
static inline
struct rbnode *rbsucc(struct rbtree *tree, struct rbnode *key, rbcmp_t cmp)
{
	int dir;
	struct rbnode *next = tree->root, *node, *succ = NULL;

	while (next != &tree->nil) {
		node = next;
		dir = cmp(key, node);
		if (dir < 0) {
			succ = node;
			next = node->left;
		} else if (dir > 0) {
			next = node->right;
		} else {
			if (node->right == &tree->nil)
				break;
			return rbsubfirst(node->right);
		}
	}

	return succ;
}

/* finds the in-order predecessor of key regardless of if key is in tree */
static inline
struct rbnode *rbpred(struct rbtree *tree, struct rbnode *key, rbcmp_t cmp)
{
	int dir;
	struct rbnode *next = tree->root, *node, *pred = NULL;

	while (next != &tree->nil) {
		node = next;
		dir = cmp(key, node);
		if (dir < 0) {
			next = node->left;
		} else if (dir > 0) {
			pred = node;
			next = node->right;
		} else {
			if (node->left == &tree->nil)
				break;
			return rbsublast(node->left);
		}
	}

	return pred;
}

/* finds the node that is equal to key */
static inline
struct rbnode *rbfind(struct rbtree *tree, struct rbnode *key, rbcmp_t cmp)
{
	int dir;
	struct rbnode *next = tree->root, *node = NULL;

	while (next != &tree->nil) {
		node = next;
		dir = cmp(key, node);
		if (dir < 0)
			next = node->left;
		else if (dir > 0)
			next = node->right;
		else
			return node;
	}

	return NULL;
}

/* replaces victim with neu */
void rbreplace(struct rbtree *tree, struct rbnode *victim, struct rbnode *neu);

/* rebalance tree after inserting item */
void rbrecolorup(struct rbtree *tree, struct rbnode *item);

/* remove item from tree */
void rberase(struct rbtree *tree, struct rbnode *item);

/* insert item into tree */
static inline
struct rbnode *rbinsert(struct rbtree *tree, struct rbnode *item, rbcmp_t cmp)
{
	int dir = 0;
	struct rbnode *next = tree->root, *parent = NULL;

	/* find the insersion point */
	while (next != &tree->nil) {
		parent = next;
		dir = cmp(item, parent);
		if (dir < 0)
			next = parent->left;
		else if (dir > 0)
			next = parent->right;
		else	/* duplicate item */
			return parent;
	}

	/* initialize item */
	item->parent = parent;
	item->left = &tree->nil;
	item->right = &tree->nil;
	item->red = 1;

	/* link the item */
	if (dir > 0)
		parent->right = item;
	else if (dir < 0)
		parent->left = item;

	/* rebalance the tree */
	rbrecolorup(tree, item);
	return NULL;
}

/* remove item by key */
static inline
struct rbnode *rbremove(struct rbtree *tree, struct rbnode *key, rbcmp_t cmp)
{
	struct rbnode *item = rbfind(tree, key, cmp);

	if (item == NULL)
		return NULL;

	rberase(tree, item);
	return item;
}

#endif
