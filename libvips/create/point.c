/* make a test pattern to show the point's frequency response
 *
 * Copyright: 1990, 1991, N.Dessipris.
 *
 * Author N. Dessipris
 * Written on 30/05/1990
 * Updated on: 27/01/1991, 07/03/1991,
 * 22/7/93 JC
 *	- im_outcheck() added
 * 30/8/95 JC
 *	- modernized
 * 1/2/11
 * 	- gtk-doc
 * 13/6/13
 * 	- redo as a class
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/*
#define VIPS_DEBUG
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /*HAVE_CONFIG_H*/
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#include "pcreate.h"
#include "point.h"

G_DEFINE_ABSTRACT_TYPE( VipsPoint, vips_point, VIPS_TYPE_CREATE );

static int
vips_point_gen( VipsRegion *or, void *seq, void *a, void *b,
	gboolean *stop )
{
	VipsPoint *point = (VipsPoint *) a;
	VipsPointClass *class = VIPS_POINT_GET_CLASS( point ); 
	VipsRect *r = &or->valid;
	int le = r->left;
	int to = r->top;
	int ri = VIPS_RECT_RIGHT( r );
	int bo = VIPS_RECT_BOTTOM( r );

	int x, y;

	for( y = to; y < bo; y++ ) {
		float *q = (float *) VIPS_REGION_ADDR( or, le, y );

		for( x = le; x < ri; x++ ) 
			*q++ = class->point( point, x, y ); 
	}

	return( 0 );
}

static int
vips_point_build( VipsObject *object )
{
	VipsCreate *create = VIPS_CREATE( object );
	VipsPoint *point = (VipsPoint *) object;
	VipsImage **t = (VipsImage **) vips_object_local_array( object, 3 );
	VipsImage *in;

	if( VIPS_OBJECT_CLASS( vips_point_parent_class )->build( object ) )
		return( -1 );

	t[0] = vips_image_new();
	vips_image_init_fields( t[0],
		point->width, point->height, 1,
		VIPS_FORMAT_FLOAT, VIPS_CODING_NONE, VIPS_INTERPRETATION_B_W,
		1.0, 1.0 );
	vips_demand_hint( t[0], 
		VIPS_DEMAND_STYLE_ANY, NULL );
	if( vips_image_generate( t[0], 
		NULL, vips_point_gen, NULL, point, NULL ) )
		return( -1 );

	in = t[0];
	if( point->uchar ) {
		if( vips_linear1( in, &t[1], 127.5, 127.5, NULL ) ||
			vips_cast( t[1], &t[2], VIPS_FORMAT_UCHAR, NULL ) )
			return( -1 );
		in = t[2];
	}

	if( vips_image_write( in, create->out ) )
		return( -1 );

	return( 0 );
}

static void
vips_point_class_init( VipsPointClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	VipsObjectClass *vobject_class = VIPS_OBJECT_CLASS( class );

	gobject_class->set_property = vips_object_set_property;
	gobject_class->get_property = vips_object_get_property;

	vobject_class->nickname = "point";
	vobject_class->description = _( "make a point image" );
	vobject_class->build = vips_point_build;

	VIPS_ARG_INT( class, "width", 2, 
		_( "Width" ), 
		_( "Image width in pixels" ),
		VIPS_ARGUMENT_REQUIRED_INPUT,
		G_STRUCT_OFFSET( VipsPoint, width ),
		1, 1000000, 1 );

	VIPS_ARG_INT( class, "height", 3, 
		_( "Height" ), 
		_( "Image height in pixels" ),
		VIPS_ARGUMENT_REQUIRED_INPUT,
		G_STRUCT_OFFSET( VipsPoint, height ),
		1, 1000000, 1 );

	VIPS_ARG_BOOL( class, "uchar", 7, 
		_( "Uchar" ), 
		_( "Output an unsigned char image" ),
		VIPS_ARGUMENT_OPTIONAL_INPUT,
		G_STRUCT_OFFSET( VipsPoint, uchar ),
		FALSE );

}

static void
vips_point_init( VipsPoint *point )
{
}

