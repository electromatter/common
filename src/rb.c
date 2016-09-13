/* rb.c - Red Black trees
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

#include <stdlib.h>
#include "rb.h"

struct rbnode *rbsubfirst(struct rbnode *node)
{
	struct rbnode *found = NULL;

	if (node == NULL)
		return NULL;

	while (node->left != NULL) {
		found = node;
		node = node->left;
	}

	return found;
}

struct rbnode *rbsublast(struct rbnode *node)
{
	struct rbnode *found = NULL;

	if (node == NULL)
		return NULL;

	while (node->right != NULL) {
		found = node;
		node = node->right;
	}

	return found;
}

struct rbnode *rbnext(struct rbnode *node)
{
	struct rbnode *next;

	if (node == NULL)
		return NULL;

	next = rbsubfirst(node->right);
	if (next != NULL)
		return next;

	while ((next = node->parent) != NULL) {
		if (next->left == node)
			break;
		node = next;
	}

	return next;
}

struct rbnode *rbnextpost(struct rbnode *node)
{
	if (node == NULL || node->parent == NULL)
		return NULL;

	if (node->parent->left == node)
		return rbnext(node->parent);

	return node->parent;
}

struct rbnode *rbprev(struct rbnode *node)
{
	struct rbnode *prev;

	if (node == NULL)
		return NULL;

	prev = rbsublast(node->left);
	if (prev != NULL)
		return prev;

	while ((prev = node->parent) != NULL) {
		if (prev->right == node)
			break;
		node = prev;
	}

	return prev;
}

static void update_parent(struct rbtree *tree,
		struct rbnode *node, struct rbnode *oldnode)
{
	struct rbnode *parent = node->parent;

	if (parent == NULL) {
		tree->root = node;
		node->red = 0;
		return;
	}

	if (parent->left == oldnode)
		parent->left = node;
	/*!!!DO NOT REMOVE THIS DOUBLE CHECK!!!
	 * see swap, we could be swaping a parent with a child */
	else if (parent->right == oldnode)
		parent->right = node;
}

static void swap(struct rbtree *tree, struct rbnode *a, struct rbnode *b)
{
	struct rbnode temp;

	/*!!!THE ORDER OF OPERATIONS HERE IS VERY IMPORTANT!!!*/

	/* repair the children's parent pointers */
	a->left->parent = b;
	a->right->parent = b;
	b->left->parent = a;
	b->right->parent = a;

	/* swap the pointers */
	temp = *a;
	*a = *b;
	*b = temp;

	/* repair the parent's child pointers */
	update_parent(tree, a, b);
	update_parent(tree, b, a);
}

void rbreplace(struct rbtree *tree, struct rbnode *victim, struct rbnode *neu)
{
	*neu = *victim;
	neu->left->parent = neu;
	neu->right->parent = neu;
	update_parent(tree, neu, victim);
}

static void rotate_left(struct rbtree *tree,
		struct rbnode **parentp, struct rbnode **rightp)
{
	struct rbnode *parent = *parentp, *right = *rightp,
			*grandparent = parent->parent,
			*right_left = right->left;

	right->left = parent;
	parent->parent = right;

	parent->right = right_left;
	right_left->parent = parent;

	right->parent = grandparent;
	update_parent(tree, right, parent);

	*parentp = right;
	*rightp = parent;
}

static void rotate_right(struct rbtree *tree,
		struct rbnode **parentp, struct rbnode **leftp)
{
	struct rbnode *parent = *parentp, *left = *leftp,
			*grandparent = parent->parent,
			*left_right = left->right;

	left->right = parent;
	parent->parent = left;

	parent->left = left_right;
	left_right->parent = parent;

	left->parent = grandparent;
	update_parent(tree, left, parent);

	*parentp = left;
	*leftp = parent;
}

void rbrecolorup(struct rbtree *tree, struct rbnode *item)
{
	struct rbnode *parent, *grandparent;

	/* preconditions:
	 *   item is a member of tree
	 *   item and it's parent is the only possible double red
	 *   tree is a otherwise a valid red-black tree
	 * postconditions:
	 *   tree is a valid red-black tree
	 */

	while (1) {
		/* we reached the root. */
		parent = item->parent;
		if (parent == NULL) {
			item->red = 0;
			tree->root = item;
			return;
		}

		/* no more violations. */
		if (!item->red || !parent->red)
			return;

		/* grandparent has two red children. */
		grandparent = parent->parent;
		if (grandparent->left->red && grandparent->right->red) {
			/* recolor and continue up the tree */
			grandparent->left->red = 0;
			grandparent->right->red = 0;
			grandparent->red = 1;
			item = grandparent;
			continue;
		}

		/* grandparent has one red and one black child. */
		if (grandparent->left == parent) {
			/* reduce to left left case */
			if (parent->right == item)
				rotate_left(tree, &parent, &item);
			/* recolor to preserve black height */
			parent->red = 0;
			grandparent->red = 1;
			rotate_right(tree, &grandparent, &parent);
			item = grandparent;
		} else /* if (grandparent->right == parent) */ {
			/* reduce to right right case */
			if (parent->left == item)
				rotate_right(tree, &parent, &item);
			/* recolor to preserve black height */
			parent->red = 0;
			grandparent->red = 1;
			rotate_left(tree, &grandparent, &parent);
			item = grandparent;
		}
	}
}

static void erase_rebalance(struct rbtree *tree, struct rbnode *item)
{
	struct rbnode *parent, *sibling, *near, *far;
	int base_case;

	/* preconditions:
	 *   item is a black member of tree or &tree->nil
	 *   item has a black inbalance of -1 compared
	 *     to all other paths in tree
	 *   tree is otherwise a valid red-black tree
	 * postconditions:
	 *   tree is a valid red-black tree
	 */

	while (1) {
		/* we've reached the root */
		parent = item->parent;
		if (parent == NULL) {
			item->red = 0;
			tree->root = item;
			return;
		}

		/* grab the sibling subtree */
		if (parent->left == item) {
			base_case = 1;
			sibling = parent->right;
			near = sibling->left;
			far = sibling->right;
		} else /* if (parent->right == item) */ {
			base_case = 0;
			sibling = parent->left;
			near = sibling->right;
			far = sibling->left;
		}

		if (sibling->red) {
			/* sibling red, rotate to get red parent
			 * without disturbing the black balance. */
			parent->red = 1;
			sibling->red = 0;
			if (base_case)
				rotate_left(tree, &parent, &sibling);
			else
				rotate_right(tree, &parent, &sibling);
			continue;
		}

		if (far->red) {
			/* far is red, recolor so the rotation
			 * does not cause a double red. */
			sibling->red = parent->red;
			parent->red = 0;
			far->red = 0;
		} else if (near->red) {
			/* reduce to the above case by rotating */
			near->red = parent->red;
			sibling->red = 0;
			parent->red = 0;
			if (base_case)
				rotate_right(tree, &sibling, &near);
			else
				rotate_left(tree, &sibling, &near);
		} else if (!parent->red) {
			/* all black case, nothing we can do but
			 * bubble the imbalance up to the parent */
			sibling->red = 1;
			item = parent;
			continue;
		}

		/* this rotation restores the black balance. */
		if (base_case)
			rotate_left(tree, &parent, &sibling);
		else
			rotate_right(tree, &parent, &sibling);
		return;
	}
}

void rberase(struct rbtree *tree, struct rbnode *item)
{
	struct rbnode *child;

	/* ensure item is one link away from nil */
	if (item->right == &tree->nil) {
		child = item->left;
	} else if (item->left == &tree->nil) {
		child = item->right;
	} else {
		/* both children non-nil; swap with successor */
		child = rbsubfirst(item->right);
		swap(tree, item, child);
		child = item->right;
	}

	/* unlink item */
	child->parent = item->parent;
	update_parent(tree, child, item);

	/* item was red, black height unchanged.
	 * also, item could not have been root. */
	if (item->red)
		return;

	/* child is red, changing it to black
	 * restores the black balance (-1 -> 0) */
	if (child->red) {
		child->red = 0;
		return;
	}

	/* black height was changed, child has a black imbalance of -1 */
	erase_rebalance(tree, child);
}
