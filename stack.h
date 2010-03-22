/***************************************************************************
                          stack.h  -  description
                             -------------------
    begin                : Wed Feb  9 14:03:54 EST 2006
    copyright            : (C) 2006-2010 by XVilka
    email                : xvilka@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

typedef int Item;
#define eq(A, B) (A == B)

void STACKinit(int maxN);
int STACKempty(void);
void STACKpush(Item item);
Item STACKpop(void);
