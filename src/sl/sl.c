/*

=head1 NAME

sl - a small and flexible linked list implementation 

=head1 DESCRIPTION

`sl' provides a generic implementation of singly-linked lists and
stacks. 

`sl' does not do extra allocations behind the scenes for
placeholder nodes, yet users of the library can define their
node structure almost any way they want. The one important
thing is that the ->next member is the I<first> member of the
structure. 

=cut

*/

#include "sl.h"

#include <stdlib.h>		/* for free() */
#include <stddef.h>		/* for NULL */


/*

Internally the library uses a structure defined as:

  struct sl_node {
    struct sl_node *next;
  };

=cut

*/

struct sl_node {
	struct sl_node *next;
};


/*

=head1 FUNCTIONS

=over

=item void *sl_push(void *root, void *p)

Push C<p> onto the list C<root>. Return the new list.

=cut

*/
void *sl_push(void *root, void *p)
{
	struct sl_node *q = p;

	if (!q) 
		return root;

	q->next = root;
	return q;
}


/*

=item void *sl_pop(void *root)

Pop a node from a list. Return the pop'ed item, or NULL if the
list is empty.

Despite the misleading prototype, this function takes a pointer
to a pointer to a node as its argument. C does not allow C<void
**> to be used as a generic pointer to pointer type. However,
since `void *' is a generic pointer, it can also point to a
pointer to pointer. 

=cut

Thanks to Thomas Stegen of CLC for suggesting this solution.

*/
void *sl_pop(void *root)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;

	if (!p)
		return NULL;

	*pp = p->next;
	p->next = NULL;

	return p;
}


/*

=item void *sl_shift(void *root, void *p)

Shift a node onto the `far end' of a list.
This function can be used to append a list to another.
The new list is returned.

=cut

*/
void *sl_unshift(void *root, void *p)
{
	struct sl_node *q;

	if (!p) 
		return root;
	if (!root)
		return p;

	q = root;
	while (q && q->next) 
		q = q->next;

	q->next = p;
	return root;
}


/*

=item void *sl_unshift(void *)

Shift a node from the `far end' of a list.
Returns the item shifted off the list, or NULL if the list is empty.

Despite the misleading prototype, this function takes a pointer
to a pointer to a node as its argument. C does not allow C<void
**> to be used as a generic pointer to pointer type. However,
since C<void *> is a generic pointer, it can also point to a
pointer to pointer. 


=cut

Thanks to Thomas Stegen of CLC for suggesting this solution.

*/
void *sl_shift(void *root)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;
	struct sl_node *q;

	/* Initial node will only change if there is only one node in
	 * the list. But it is safe to set it to NULL if it is already
	 * NULL.
	 */
	if (!p || !p->next) {
		*pp = NULL;
		return p;
	}

	while (p && p->next) {
		q = p;
		p = p->next;
	}
	q->next = NULL;
	return p;
}


/*

=item void *sl_reverse(void *root)

Returns the reversed list.

=cut

*/
void *sl_reverse(void *root)
{
	struct sl_node *revlist, *p;

	revlist = NULL;
	while ((p = sl_pop(&root)))
		revlist = sl_push(revlist, p);

	return revlist;
}


/*

=item void *sl_map(void *root, int (*func)(void *, void *), void *data)

Map a function, C<func>, to every element in a list.
The C<data> is handed to C<func> along with each node. This
function can be used for a sequential search of a list of nodes. 

This function returns NULL on normal operation. If C<func>
returns non-zero, a pointer to the current node will be returned.

=cut

This function is inspired by Kernighan & Pike's `The Practice of
Programming'.

*/
void *sl_map(void *root, int (*func)(void *, void *), void *data)
{
	struct sl_node *p;

	for (p = root; p != NULL; p = p->next)
		if (func(p, data))
			return p;
	return NULL;
}

/*

=item void *sl_remove(void *root, int (*func)(void *, void *), void *data)

Remove a node from the list, if C<func> returns non-zero,
this node will be removed and returned.

Despite the misleading prototype, this function takes a pointer
to a pointer to a node as its argument. C does not allow C<void
**> to be used as a generic pointer to pointer type. However,
since C<void *> is a generic pointer, it can also point to a
pointer to pointer. 

=cut

*/
void *sl_remove(void *root, int (*func)(void *, void *), void *data)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;
	struct sl_node *q;

	if (!p) {
		return NULL;
	}
	
	if (func(p, data)) {
		*pp = p->next;
		p->next = NULL;
		return p;
	}

	while (p) {
		q = p;
		p = p->next;
		if (p && func(p, data)) {
			q->next = p->next;
			p->next = NULL;
			return p;
		}
	}
	return NULL;
}

/*

=item void *sl_filter(void *root, int (*func)(void *, void *), void *data)

If C<func> returns negative when it finds a match, the element is
removed from the list and returned immediatly. However, if C<func>
returns positive, it returns a list of *all* values that match, and
these elements are removed from the original list.

To return only the first 5 elements maintain a counter in C<data> and
thus return only the first 5 elements matching your criteria by
having C<func> examine C<data> and return negative instead of
positive on the fifth match.

Despite the misleading prototype, this function takes a pointer
to a pointer to a node as its argument. C does not allow C<void
**> to be used as a generic pointer to pointer type. However,
since C<void *> is a generic pointer, it can also point to a
pointer to pointer. 

=cut

*/

void *sl_filter(void *root, int (*func)(void *, void *), void *data)
{
	struct sl_node **pp = root;
	struct sl_node *p = *pp;
	struct sl_node *q;
	struct sl_node *r = NULL;
	int val;

	if (!p) {
		return NULL;
	}
	
	val = func(p, data);
	if (val < 0) {
		return sl_pop(pp);
	} else {
		while (val > 0 && p) {
			r = sl_unshift(r, sl_pop(pp));
			p = *pp;
			val = func(p, data);
		}
		if (val < 0) {
			r = sl_unshift(r, sl_pop(pp));
			return r;
		}
	}
	while (p && p->next) {
		q = p;
		p = p->next;
		val = func(p, data);
		if (val != 0) {
			q->next = p->next;
			p->next = NULL;
			r = sl_unshift(r, p);
		}
		if (val > 0) {
			p = q;
		} else if (val < 0) {
			return r;
		}
	}
	return r;
}

/*

=item void *sl_split(void *root) 

Split a list roughly on the middle; return a pointer to the second
half. 

=cut

This function is a pre-requisite for mergesort. Thanks to CB
Falconer for this code.

*/
void *sl_split(void *root)
{
	struct sl_node *p, *p1, *p2;

	if (!root)
		return NULL;

	p1 = p2 = p = root;
	do {
		p2 = p1;
		p1 = p1->next;			/* advance 1 */
		p = p->next;
		if (p) p = p->next;		/* advance 2 */
	} while (p);

	p2->next = NULL;
	return p1;
}


/*

=item void *sl_merge(void *p1, void *p2, int (*cmp)(void *, void *))

Merge two sorted lists and keep the list sorted. This function is
the heart of the mergesort routine. Thanks to CB Falconer for
this code.

=cut

*/
void *sl_merge(void *p1, void *p2, int (*cmp)(void *, void *))
{
	struct sl_node *q1 = p1, *q2 = p2;
        struct sl_node n, *root;

        root = &n;
        n.next = root;

        while (q1 && q2) {
                if (0 >= cmp(q1, q2)) {
                        root->next = q1;
                        root = q1;
                        q1 = q1->next;
                }
                else {
                        root->next = q2;
                        root = q2;
                        q2 = q2->next;
                }
        }

        /* At least one list empty now; append the other. */
	root->next = q1 ? q1 : q2;

        /* check for an empty list */
        if (n.next == &n)
		return NULL;

        return n.next;
}


/*

=item void *sl_mergesort(void *root, int (*cmp)(void *, void *))

Return the sorted list. 

=cut

Sort a list using mergesort. The algorithm is recursive.
Thanks to CB Falconer for this code.

*/
void *sl_mergesort(void *root, int (*cmp)(void *, void *))
{
	struct sl_node *p;

	/*
	 * Must be at least two nodes in a list before it can be
	 * unordered.
	 */
	p = root;
	if (p && p->next) {
		p = sl_split(root);
		root = sl_merge(sl_mergesort(root, cmp), sl_mergesort(p, cmp), cmp);
	}

	return root;
}


/*

=item int sl_count(void *p)

Returns the number of elements in a list.

=cut

*/
int sl_count(void *p)
{
        struct sl_node *q;
	int n = 0;

	for (q = p; q; q = q->next) 
		n++;

	return n;
}


/*

=item void sl__free(void *root, void (*func)(void*))

Free a list of nodes. Takes an optional argument, @p func, used to
free the node if it is defined.

=cut

*/
void sl__free(void *root, void (*func)(void*))
{
	struct sl_node *p;

	if (func == NULL)
		func = free;

	while ((p = sl_pop(&root)))
		func(p);
}

/*

=back

=head1 COPYRIGHT

Copyright (C) 2003,2004,2005 Stig Brautaset

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

=cut

*/
