/*
 * Unitex
 *
 * Copyright (C) 2001-2019 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
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

#include "LocateMatches.h"
#include "Error.h"
#include "Ustring.h"

#ifndef HAS_UNITEX_NAMESPACE
#define HAS_UNITEX_NAMESPACE 1
#endif

namespace unitex {

/**
 * Allocates, initializes and returns a new match list element.
 */
struct match_list* new_match(int start,int end,unichar* output,int weight,struct match_list* next,Abstract_allocator prv_alloc) {
return new_match(start,end,-1,-1,-1,-1,output,weight,next,prv_alloc);
}


/**
 * Allocates, initializes and returns a new match list element.
 */
struct match_list* new_match(int start,int end,int start_char,int end_char,
                             int start_letter,int end_letter,unichar* output,
                             int weight,struct match_list* next,Abstract_allocator prv_alloc) {
struct match_list *l;
l=(struct match_list*)malloc_cb(sizeof(struct match_list),prv_alloc);
if (l==NULL) {
   fatal_alloc_error("new_match");
}
l->m.start_pos_in_token=start;
l->m.end_pos_in_token=end;
l->weight=weight;
if (output==NULL) {
   l->output=NULL;
} else {
   l->output=u_strdup(output,prv_alloc);
}
l->m.start_pos_in_char=start_char;
l->m.end_pos_in_char=end_char;
l->m.start_pos_in_letter=start_letter;
l->m.end_pos_in_letter=end_letter;
l->next=next;
return l;
}


/**
 * Frees a single match list element.
 */
void free_match_list_element(struct match_list* l,Abstract_allocator prv_alloc) {
  if (l==NULL) {
    return;
  }
  if (l->output!=NULL) {
    free_cb(l->output,prv_alloc);
  }

  free_cb(l,prv_alloc);

  // protecting against dangling pointer bugs
  l = NULL;
}


/**
 * Frees a single match list element.
 */
void free_match_list(struct match_list* l,Abstract_allocator prv_alloc) {
while (l!=NULL) {
   struct match_list* ptr=l;
   l=l->next;
   if (ptr->output!=NULL) free_cb(ptr->output,prv_alloc);
   free_cb(ptr,prv_alloc);
}
}




/**
 * Loads a match list. Match lists are supposed to have been
 * generated by the Locate program.
 */
struct match_list* load_match_list(U_FILE* f,OutputPolicy *output_policy,unichar *header,Abstract_allocator prv_alloc) {
struct match_list* l=NULL;
struct match_list* end_of_list=NULL;
int start,end,start_char,end_char,start_letter,end_letter;
Ustring* line=new_Ustring();
char is_an_output;
/* We read the header */
unichar foo=0;
if (header==NULL) {
  header=&foo;
}
u_fscanf(f,"#%C\n",header);
OutputPolicy policy;
switch(*header) {
   case 'D': {
     policy=DEBUG_OUTPUTS;
     /* In debug mode, we have to skip the debug header */
     int n_graphs;
     u_fscanf(f,"%d\n",&n_graphs);
     while ((n_graphs--)>-1) {
       /* -1, because we also have to skip the #[IMR] line */
       readline(line,f);
     }
     break;
   }
   case 'M': policy=MERGE_OUTPUTS; break;
   case 'R':
   case 'T':
   case 'X': policy=REPLACE_OUTPUTS; break;
   case 'I':
   default: policy=IGNORE_OUTPUTS; break;
}
if (output_policy!=NULL) {
   (*output_policy)=policy;
}
while (6==u_fscanf(f,"%d.%d.%d %d.%d.%d",&start,&start_char,&start_letter,&end,&end_char,&end_letter)) {
   /* We look if there is an output or not, i.e. a space or a new line */
   int c=u_fgetc(f);
   if (c==' ') {
      /* If we have an output to read */
    readline(line,f);
    /* In debug mode, we have to stop at the char #1 */
      int i=-1;
      while (line->str[++i]!=1 && line->str[i]!='\0') {
    }
      line->str[i]='\0';
   }
   is_an_output=(policy!=IGNORE_OUTPUTS);
   if (l==NULL) {
      l=new_match(start,end,start_char,end_char,start_letter,end_letter,is_an_output?line->str:NULL,-1,NULL,prv_alloc);
      end_of_list=l;
   } else {
      end_of_list->next=new_match(start,end,start_char,end_char,start_letter,end_letter,is_an_output?line->str:NULL,-1,NULL,prv_alloc);
      end_of_list=end_of_list->next;
   }
}
free_Ustring(line);
return l;
}


/**
 * Returns 1 if the given matches correspond to ambiguous outputs
 * for the same sequence; 0 otherwise.
 */
int are_ambiguous(struct match_list* a,struct match_list* b) {
if (compare_matches(&(a->m),&(b->m))!=A_EQUALS_B) return 0;
return 1;
}


/**
 * This function removes all non ambiguous outputs from the given match list.
 * If renumber is non NULL, we have renumber[x]=y, where x is the position
 * of a match in the filtered list, and y its corresponding number in the
 * unfiltered original one.
 */
void filter_unambiguous_outputs(struct match_list* *list,vector_int* renumber) {
struct match_list* tmp;
if (*list==NULL) return;
struct match_list* previous=NULL;
struct match_list* l=*list;
int previous_was_identical=0;
int original_match_number=-1;
while (l!=NULL) {
  original_match_number++;
  if (previous==NULL) {
    /* Case 1: we are at the beginning of the list */
    /* Case 1a: there is only one cell */
    if (l->next==NULL) {
      free_match_list(l);
      *list=NULL;
      return;
    }
    /* Case 1b: there is a next cell, but it's not ambiguous with the current one */
    if (!are_ambiguous(l,l->next)) {
      /* We have to delete the current cell */
      tmp=l->next;
      free_match_list_element(l);
      l=tmp;
      continue;
    }
    /* Case 1c: the next cell is an ambiguous one, we can move on */
    /* Now we know the list head element */
    *list=l;
    previous=l;
    previous_was_identical=1;
    l=l->next;
    vector_int_add(renumber,original_match_number);
    continue;
  } else {
    /* Case 2: there is a previous cell */
    if (previous_was_identical) {
      vector_int_add(renumber,original_match_number);
      /* Case 2a: we know that we have to keep this current cell, but
       * we must check if the next is also an ambiguous one */
      if (l->next==NULL) {
        /* No next cell ? We're done then */
        return;
      }
      previous_was_identical=are_ambiguous(l,l->next);
      previous=l;
      l=l->next;
      continue;
    }
    /* Case 2b: previous cell is different, so we have to test the next one
     * to know whether we must keep the current one or not */
    if (l->next==NULL) {
      /* No next cell ? We have to delete the current one and then
       * we are done */
      free_match_list_element(l);
      previous->next=NULL;
      return;
    }
    previous_was_identical=are_ambiguous(l,l->next);
    if (previous_was_identical) {
      /* We have to keep the current cell */
      previous=l;
      l=l->next;
      vector_int_add(renumber,original_match_number);
      continue;
    }
    /* Final case, the next cell is not ambiguous, so we have to delete
     * the current one */
    tmp=l;
    l=l->next;
    free_match_list_element(tmp);
    previous->next=l;
    continue;
  }
}
}

} // namespace unitex
