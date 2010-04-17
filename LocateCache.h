 /*
  * Unitex
  *
  * Copyright (C) 2001-2010 Universit� Paris-Est Marne-la-Vall�e <unitex@univ-mlv.fr>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
  *
  */


#ifndef LocateCache_H
#define LocateCache_H

#include "LocateMatches.h"
#include "Vector.h"

/**
 * This library provides a cache for storing match lists associated to token
 * sequences, using a ternary search tree.
 */


typedef struct locate_cache {
	struct locate_cache* left;
	struct locate_cache* middle;
	struct locate_cache* right;
	int token;
	struct match_list* matches;
}* LocateCache;


LocateCache new_LocateCache(int token,struct match_list* matches);
void free_LocateCache(LocateCache c);
void cache_match(struct match_list* matches,int* tab,int start,int end,LocateCache* c);
int consult_cache(int* tab,int start,int tab_size,LocateCache* c,vector_ptr* res);

#endif