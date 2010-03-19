/***************************************************************************
                          stack.c  -  description
                             -------------------
    begin                : Wed Feb  9 14:03:54 EST 2006
    copyright            : (C) 2006 by XVilka
    email                : xvilka@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
#include <stdlib.h>
#include "stack.h"

typedef struct STACKnode* link;
struct STACKnode 
{ 
	Item item; 
	link next; 
};

static link head;

link NEW(Item item, link next)
{
	link x = malloc(sizeof *x);
	x->item = item;
	x->next = next;
	return x;
}

void STACKinit(int maxN)
{
	head = NULL;
}

int STACKempty(void)
{
	return head == NULL;
}

void STACKpush(Item item)
{
	head = NEW(item, head);
}

Item STACKpop(void)
{
	Item item = head->item;
	link t = head->next;
	free(head);
	head = t;
	return item;
}

