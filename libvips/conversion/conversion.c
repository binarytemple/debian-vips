/* base class for all conversion operations
 *
 * properties:
 * 	- single output image
 */

/*

    Copyright (C) 1991-2005 The National Gallery

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA

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
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#include "pconversion.h"

/** 
 * SECTION: conversion
 * @short_description: convert images in some way: change band format, change header, insert, extract, join
 * @see_also: <link linkend="libvips-resample">resample</link>
 * @stability: Stable
 * @include: vips/vips.h
 *
 * These operations convert an image in some way. They can be split into a two
 * main groups.
 *
 * The first set of operations change an image's format in some way. You
 * can change the band format (for example, cast to 32-bit unsigned
 * int), form complex images from real images, convert images to
 * matrices and back, change header fields, and a few others.
 *
 * The second group move pixels about in some way. You can flip, rotate,
 * extract, insert and join pairs of images in various ways.
 *
 */

/** 
 * VipsAlign:
 * @VIPS_ALIGN_LOW: align low coordinate edge
 * @VIPS_ALIGN_CENTRE: align centre
 * @VIPS_ALIGN_HIGH: align high coordinate edge
 *
 * See vips_join() and so on.
 *
 * Operations like vips_join() need to be told whether to align images on the
 * low or high coordinate edge, or centre.
 *
 * See also: vips_join().
 */

/** 
 * VipsAngle:
 * @VIPS_ANGLE_0: no rotate
 * @VIPS_ANGLE_90: 90 degrees anti-clockwise
 * @VIPS_ANGLE_180: 180 degree rotate
 * @VIPS_ANGLE_270: 90 degrees clockwise
 *
 * See vips_rot() and so on.
 *
 * Fixed rotate angles.
 *
 * See also: vips_rot().
 */

/** 
 * VipsExtend:
 * @VIPS_EXTEND_BLACK: extend with black (all 0) pixels
 * @VIPS_EXTEND_COPY: copy the image edges
 * @VIPS_EXTEND_REPEAT: repeat the whole image
 * @VIPS_EXTEND_MIRROR: mirror the whole image
 * @VIPS_EXTEND_WHITE: extend with white (all bits set) pixels
 * @VIPS_EXTEND_BACKGROUND: extend with colour from the @background property
 *
 * See vips_embed(), vips_conv(), vips_affine() and so on.
 *
 * When the edges of an image are extended, you can specify
 * how you want the extension done. 
 *
 * #VIPS_EXTEND_BLACK --- new pixels are black, ie. all bits are zero. 
 *
 * #VIPS_EXTEND_COPY --- each new pixel takes the value of the nearest edge
 * pixel
 *
 * #VIPS_EXTEND_REPEAT --- the image is tiled to fill the new area
 *
 * #VIPS_EXTEND_MIRROR --- the image is reflected and tiled to reduce hash
 * edges
 *
 * #VIPS_EXTEND_WHITE --- new pixels are white, ie. all bits are set
 *
 * #VIPS_EXTEND_BACKGROUND --- colour set from the @background property
 *
 * We have to specify the exact value of each enum member since we have to 
 * keep these frozen for back compat with vips7.
 *
 * See also: vips_embed().
 */

/** 
 * VipsDirection:
 * @VIPS_DIRECTION_HORIZONTAL: left-right 
 * @VIPS_DIRECTION_VERTICAL: top-bottom
 *
 * See vips_flip(), vips_join() and so on.
 *
 * Operations like vips_flip() need to be told whether to flip left-right or
 * top-bottom. 
 *
 * See also: vips_flip(), vips_join().
 */

G_DEFINE_ABSTRACT_TYPE( VipsConversion, vips_conversion, VIPS_TYPE_OPERATION );

static int
vips_conversion_build( VipsObject *object )
{
	VipsConversion *conversion = VIPS_CONVERSION( object );

#ifdef DEBUG
	printf( "vips_conversion_build: " );
	vips_object_print_name( object );
	printf( "\n" );
#endif /*DEBUG*/

	g_object_set( conversion, "out", vips_image_new(), NULL ); 

	if( VIPS_OBJECT_CLASS( vips_conversion_parent_class )->build( object ) )
		return( -1 );

	return( 0 );
}

static void
vips_conversion_class_init( VipsConversionClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	VipsObjectClass *vobject_class = VIPS_OBJECT_CLASS( class );

	gobject_class->set_property = vips_object_set_property;
	gobject_class->get_property = vips_object_get_property;

	vobject_class->nickname = "conversion";
	vobject_class->description = _( "conversion operations" );
	vobject_class->build = vips_conversion_build;

	VIPS_ARG_IMAGE( class, "out", 1, 
		_( "Output" ), 
		_( "Output image" ),
		VIPS_ARGUMENT_REQUIRED_OUTPUT, 
		G_STRUCT_OFFSET( VipsConversion, out ) );
}

static void
vips_conversion_init( VipsConversion *conversion )
{
}

/* Called from iofuncs to init all operations in this dir. Use a plugin system
 * instead?
 */
void
vips_conversion_operation_init( void )
{
	extern GType vips_copy_get_type( void ); 
	extern GType vips_tile_cache_get_type( void ); 
	extern GType vips_line_cache_get_type( void ); 
	extern GType vips_sequential_get_type( void ); 
	extern GType vips_cache_get_type( void ); 
	extern GType vips_embed_get_type( void ); 
	extern GType vips_flip_get_type( void ); 
	extern GType vips_insert_get_type( void ); 
	extern GType vips_join_get_type( void ); 
	extern GType vips_extract_area_get_type( void ); 
	extern GType vips_extract_band_get_type( void ); 
	extern GType vips_replicate_get_type( void ); 
	extern GType vips_cast_get_type( void ); 
	extern GType vips_bandjoin_get_type( void ); 
	extern GType vips_black_get_type( void ); 
	extern GType vips_rot_get_type( void ); 
	extern GType vips_ifthenelse_get_type( void ); 
	extern GType vips_recomb_get_type( void ); 
	extern GType vips_bandmean_get_type( void ); 
	extern GType vips_flatten_get_type( void ); 
	extern GType vips_bandbool_get_type( void ); 
	extern GType vips_gaussnoise_get_type( void ); 
	extern GType vips_grid_get_type( void ); 
	extern GType vips_scale_get_type( void ); 
	extern GType vips_wrap_get_type( void ); 
	extern GType vips_zoom_get_type( void ); 
	extern GType vips_subsample_get_type( void ); 
	extern GType vips_msb_get_type( void ); 
#ifdef HAVE_PANGOFT2
	extern GType vips_text_get_type( void ); 
#endif /*HAVE_PANGOFT2*/
	extern GType vips_xyz_get_type( void ); 
	extern GType vips_falsecolour_get_type( void ); 
	extern GType vips_gammacorrect_get_type( void ); 

	vips_copy_get_type();
	vips_tile_cache_get_type(); 
	vips_line_cache_get_type(); 
	vips_sequential_get_type(); 
	vips_cache_get_type(); 
	vips_embed_get_type();
	vips_flip_get_type();
	vips_insert_get_type();
	vips_join_get_type();
	vips_extract_area_get_type();
	vips_extract_band_get_type();
	vips_replicate_get_type();
	vips_cast_get_type();
	vips_bandjoin_get_type();
	vips_black_get_type();
	vips_rot_get_type();
	vips_ifthenelse_get_type();
	vips_recomb_get_type(); 
	vips_bandmean_get_type(); 
	vips_flatten_get_type(); 
	vips_bandbool_get_type(); 
	vips_gaussnoise_get_type(); 
	vips_grid_get_type(); 
	vips_scale_get_type(); 
	vips_wrap_get_type(); 
	vips_zoom_get_type(); 
	vips_subsample_get_type(); 
	vips_msb_get_type(); 
#ifdef HAVE_PANGOFT2
	vips_text_get_type(); 
#endif /*HAVE_PANGOFT2*/
	vips_xyz_get_type(); 
	vips_falsecolour_get_type(); 
	vips_gammacorrect_get_type(); 
}

/* The common part of most binary conversion
 * operators. We:
 *
 * - check in and out
 * - cast in1 and in2 up to a common format
 * - equalise bands 
 * - make an input array
 * - return the matched images in vec[0] and vec[1]
 *
 * A left-over, remove soon.
 */
IMAGE **
im__insert_base( const char *domain, 
	IMAGE *in1, IMAGE *in2, IMAGE *out ) 
{
	IMAGE *t[4];
	IMAGE **vec;

	if( im_piocheck( in1, out ) || 
		im_pincheck( in2 ) ||
		im_check_bands_1orn( domain, in1, in2 ) ||
		im_check_coding_known( domain, in1 ) ||
		im_check_coding_same( domain, in1, in2 ) )
		return( NULL );

	/* Cast our input images up to a common format and bands.
	 */
	if( im_open_local_array( out, t, 4, domain, "p" ) ||
		im__formatalike( in1, in2, t[0], t[1] ) ||
		im__bandalike( domain, t[0], t[1], t[2], t[3] ) ||
		!(vec = im_allocate_input_array( out, t[2], t[3], NULL )) )
		return( NULL );

	/* Generate the output.
	 */
	if( im_cp_descv( out, vec[0], vec[1], NULL ) ||
		im_demand_hint_array( out, IM_SMALLTILE, vec ) )
		return( NULL );

	return( vec );
}

/**
 * im_insertset:
 * @main: big image
 * @sub: small image
 * @out: output image
 * @n: number of positions
 * @x: left positions of @sub
 * @y: top positions of @sub
 *
 * Insert @sub repeatedly into @main at the positions listed in the arrays @x,
 * @y of length @n. @out is the same
 * size as @main. @sub is clipped against the edges of @main. 
 *
 * This operation is fast for large @n, but will use a memory buffer the size
 * of @out. It's useful for things like making scatter plots.
 *
 * If the number of bands differs, one of the images 
 * must have one band. In this case, an n-band image is formed from the 
 * one-band image by joining n copies of the one-band image together, and then
 * the two n-band images are operated upon.
 *
 * The two input images are cast up to the smallest common type (see table 
 * Smallest common format in 
 * <link linkend="VIPS-arithmetic">arithmetic</link>).
 *
 * See also: im_insert(), im_lrjoin().
 *
 * Returns: 0 on success, -1 on error
 */
int
im_insertset( IMAGE *main, IMAGE *sub, IMAGE *out, int n, int *x, int *y )
{
	IMAGE **vec;
	IMAGE *t;
	int i;

	if( !(vec = im__insert_base( "im_insert", main, sub, out )) )
		return( -1 );

	/* Copy to a memory image, zap that, then copy to out.
	 */
	if( !(t = im_open_local( out, "im_insertset", "t" )) ||
		im_copy( vec[0], t ) )
		return( -1 );

	for( i = 0; i < n; i++ ) 
		if( im_insertplace( t, vec[1], x[i], y[i] ) )
			return( -1 );

	if( im_copy( t, out ) )
		return( -1 );

	return( 0 );
}
