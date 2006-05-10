 /*
  * Unitex
  *
  * Copyright (C) 2001-2006 Universit� de Marne-la-Vall�e <unitex@univ-mlv.fr>
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

#ifndef ConcordanceH
#define ConcordanceH

#include "Text_tokens.h"


#define TEXT_ORDER 0
#define LEFT_CENTER 1
#define LEFT_RIGHT 2
#define CENTER_LEFT 3
#define CENTER_RIGHT 4
#define RIGHT_LEFT 5
#define RIGHT_CENTER 6
//#define BUFFER_SIZE 1000000
#define MAX_CONTEXT_IN_UNITS 5000
//#define MAX_HTML_CONTEXT 10000




//extern int buffer[BUFFER_SIZE];
//extern int BUFFER_LENGTH;
//extern int N_UNITS_ALLREADY_READ;
extern int open_bracket;
extern int close_bracket;
//extern int origine_courante_char;
//extern int phrase_courante;


void create_concordance(FILE*,FILE*,struct text_tokens*,int,int,int,char*,char*,
                        char*,char*,char*,int,int*,int);
                        
                        

#endif
