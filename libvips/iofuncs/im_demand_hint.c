/* demand hints
 *
 * Copyright: The National Gallery, 1993
 * Written on: 6/9/93
 * Modified on : 
 * 2/3/98 JC
 *	- IM_ANY added
 * 19/5/06
 * 	- minor change to rules: don't force ANY on no-input operations ...
 * 	  fails for image import
 * 1/12/06
 * 	- build parent/child links as well
 * 8/10/09
 * 	- gtkdoc comments
 */

/*

    This file is part of VIPS.
    
    VIPS is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/*
#define DEBUG
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /*HAVE_CONFIG_H*/
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif /*WITH_DMALLOC*/

/* Max number of images we can handle.
 */
#define MAX_IMAGES (1000)

/* Given two im_demand_type, return the most restrictive.
 */
static im_demand_type
find_least( im_demand_type a, im_demand_type b )
{
	return( (im_demand_type) IM_MIN( (int) a, (int) b ) );
}

/**
 * im_demand_hint_array: 
 * @im: image to set hint for
 * @hint: hint for this image
 * @in: array of input images to this operation
 *
 * Operations can set demand hints, that is, hints to the VIPS IO system about
 * the type of region geometry this operation works best with. For example,
 * operations which transform coordinates will usually work best with
 * %IM_SMALLTILE, operations which work on local windows of pixels will like
 * %IM_FATSTRIP.
 *
 * VIPS uses the list of input images to build the tree of operations it needs
 * for the cache invalidation system. You have to call this function, or its
 * varargs friend im_demand_hint().
 *
 * See also: im_demand_hint(), im_generate().
 *
 * Returns: 0 on success, or -1 on error.
 */
int 
im_demand_hint_array( IMAGE *im, VipsDemandStyle hint, IMAGE **in )
{
	int i, len, nany;

	/* How many input images are there? And how many are IM_ANY?
	 */
	for( i = 0, len = 0, nany = 0; in[i]; i++, len++ )
		if( in[i]->dhint == IM_ANY )
			nany++;

	if( len == 0 ) 
		/* No input images? Just set the requested hint. We don't 
		 * force ANY, since the operation might be something like 
		 * tiled read of an EXR image, where we certainly don't want 
		 * ANY.
		 */
		;
	else if( nany == len ) 
		/* Special case: if all the inputs are IM_ANY, then output can 
		 * be IM_ANY regardless of what this function wants. 
		 */
		hint = IM_ANY;
	else
		/* Find the most restrictive of all the hints available to us.
		 */
		for( i = 0; i < len; i++ )
			hint = find_least( hint, in[i]->dhint );

	im->dhint = hint;

#ifdef DEBUG
        printf( "im_demand_hint_array: set dhint for \"%s\" to %s\n",
		im->filename, im_dhint2char( im->dhint ) );
#endif /*DEBUG*/

	/* im depends on all these ims.
	 */
	for( i = 0; i < len; i++ )
		im__link_make( im, in[i] );

	/* Set a flag on the image to say we remember to call this thing.
	 * im_generate() and friends check this.
	 */
	im->hint_set = TRUE;

	return( 0 );
}

/**
 * im_demand_hint:
 * @im: image to set hint for
 * @hint: hint for this image
 * @Varargs: %NULL-terminated list of input images to this operation
 *
 * Build an array and call im_demand_hint_array().
 *
 * See also: im_demand_hint(), im_generate().
 *
 * Returns: 0 on success, or -1 on error.
 */
int 
im_demand_hint( IMAGE *im, VipsDemandStyle hint, ... )
{
	va_list ap;
	int i;
	IMAGE *ar[MAX_IMAGES];

	va_start( ap, hint );
	for( i = 0; i < MAX_IMAGES && (ar[i] = va_arg( ap, IMAGE * )); i++ ) 
		;
	va_end( ap );
	if( i == MAX_IMAGES ) {
		im_error( "im_demand_hint", 
			"%s", _( "too many images" ) );
		return( -1 );
	}

	return( im_demand_hint_array( im, hint, ar ) );
}
