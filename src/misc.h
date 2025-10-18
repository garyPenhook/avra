/***********************************************************************
 *
 *  AVRA - Assembler for the Atmel AVR microcontroller series
 *
 *  Copyright (C) 1998-2020 The AVRA Authors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 *
 *  Authors of AVRA can be reached at:
 *     email: jonah@omegav.ntnu.no, tobiw@suprafluid.com
 *     www: https://github.com/Ro5bert/avra
 */

/* C23 boolean types - using stdbool.h for compatibility */
#include <stdbool.h>

/* Compatibility macros for legacy True/False usage */
#define True true
#define False false

enum filetype {
	AVRSTUDIO = 0,
	GENERIC,
	INTEL,
	MOTOROLA
};

/* C23 Generic List Append Macro
 * Appends a node to a linked list with first/last pointers.
 * Works with any struct type that has a 'next' field pointing to the same type.
 * Usage: LIST_APPEND(node, container->first_list, container->last_list)
 */
#define LIST_APPEND(node, first_ptr, last_ptr) do { \
	(node)->next = NULL; \
	if ((last_ptr)) \
		(last_ptr)->next = (node); \
	else \
		(first_ptr) = (node); \
	(last_ptr) = (node); \
} while(0)

