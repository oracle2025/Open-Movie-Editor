/*
 * sl - a small and efficient linked list library
 *
 * Copyright (C) 2003,2004,2005 Stig Brautaset
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef sl__sl_h
#define sl__sl_h

#ifdef __cplusplus      /* let C++ coders use this library */
extern "C" {
#endif

void *sl_push(void *root, void *p);
void *sl_pop(void *root);

void *sl_unshift(void *root, void *p);
void *sl_shift(void *root);

void *sl_reverse(void *root);
void *sl_map(void *root, int (*func)(void *, void *), void *data);
void *sl_remove(void *root, int (*func)(void *, void *), void *data);
void *sl_filter(void *root, int (*func)(void *, void *), void *data);

void *sl_split(void *root);
void *sl_merge(void *p1, void *p2, int (*cmp)(void *, void *));
void *sl_mergesort(void *root, int (*cmp)(void *, void *));

int sl_count(void *p);

#define sl_free(root, func) sl__free(root, func)
void sl__free(void *root, void (*func)(void *));


#ifdef __cplusplus
}
#endif
#endif	/* !sl__sl_h */
