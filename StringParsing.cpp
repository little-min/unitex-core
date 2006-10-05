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

#include "StringParsing.h"


/**
 * Returns 1 if 'c' is in the string 'array'; 0 otherwise.
 */
int is_in_array(unichar c,unichar* array) {
for (int i=0;array[i]!='\0';i++) {
   if (c==array[i]) return 1;
}
return 0;
}


/**
 * Parses the string 's' from '*ptr' until it finds '\0' or a character that is in 'stop_chars'.
 * 'chars_to_keep_protected' is used to define which chars must keep their protection 
 * char. Note that forbidden chars and stop_chars will be taken into account if and only
 * if they are not protected. The intersection between 'stop_chars' and 'forbidden_chars'
 * is supposed to be null. If not, 'stop_chars' is considered first. '*ptr' is updated.
 * If an error occurs, it returns an error OK; otherwise, the substring obtained
 * is stored in 'result' and P_OK is returned.
 * 
 * Example: parse("E\=\mc\2 is a formula",result," ","m","2") will produce the
 *          result = "E=mc\2"
 */
int parse(unichar* s,int *ptr,unichar* result,unichar* stop_chars,unichar* forbidden_chars,
          unichar* chars_to_keep_protected) {
int j=0;
while (s[*ptr]!='\0') {
   if (s[*ptr]==PROTECTION_CHAR) {
      /* If there is a protection character (backslash) */
      if (s[(*ptr)+1]=='\0') {
         /* It must not appear at the end of the string */
         return P_BACKSLASH_AT_END;
      }
      if (is_in_array(s[(*ptr)+1],chars_to_keep_protected)) {
         /* If the character must keep its backslash */
         result[j++]=PROTECTION_CHAR;
      }
      result[j++]=s[(*ptr)+1];
      (*ptr)=(*ptr)+2;
   } else {
      /* If we have an unprotected character */
      if (is_in_array(s[*ptr],stop_chars)) {
         /* If it is a stop char, we have finished */
         result[j]='\0';
         return P_OK;
      }
      if (is_in_array(s[*ptr],forbidden_chars)) {
         /* If it is a forbidden char, it's an error */
         return P_FORBIDDEN_CHAR;
      }
      /* If it's a normal char, we copy it */
      result[j++]=s[(*ptr)++];
   }
}
/* If we arrive here, we have reached the end of the string without error */
result[j]='\0';
return P_OK;
}
