/* find image minimum
 *
 * Copyright: 1990, J. Cupitt
 *
 * Author: J. Cupitt
 * Written on: 02/05/1990
 * Modified on : 18/03/1991, N. Dessipris
 * 23/11/92 JC
 *	- correct result for more than 1 band now.
 * 23/7/93 JC
 *	- im_incheck() added
 * 20/6/95 JC
 *	- now returns double for value, like im_max()
 * 4/9/09
 * 	- gtkdoc comment
 * 8/9/09
 * 	- rewrite, from im_maxpos()
 * 30/8/11
 * 	- rewrite as a class
 * 5/9/11
 * 	- abandon scan if we find minimum possible value
 * 24/2/12
 * 	- avoid NaN in float/double/complex images
 * 	- allow +/- INFINITY as a result
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
#include <math.h>
#include <limits.h>

#include <vips/vips.h>
#include <vips/internal.h>

#include "statistic.h"

typedef struct _VipsMin {
	VipsStatistic parent_instance;

	gboolean set;		/* FALSE means no value yet */

	/* The current miniumum. When scanning complex images, we keep the
	 * square of the modulus here and do a single sqrt() right at the end.
	 */
	double min;

	/* And its position.
	 */
	int x, y;
} VipsMin;

typedef VipsStatisticClass VipsMinClass;

G_DEFINE_TYPE( VipsMin, vips_min, VIPS_TYPE_STATISTIC );

static int
vips_min_build( VipsObject *object )
{
	VipsStatistic *statistic = VIPS_STATISTIC( object ); 
	VipsMin *min = (VipsMin *) object;

	if( VIPS_OBJECT_CLASS( vips_min_parent_class )->build( object ) )
		return( -1 );

	/* Don't set if there's no value (eg. if every pixel is NaN). This
	 * will trigger an error later.
	 */
	if( min->set ) {
		double m;

		/* For speed we accumulate min^2 for complex.
		 */
		m = min->min;
		if( vips_bandfmt_iscomplex( 
			vips_image_get_format( statistic->in ) ) )
			m = sqrt( m );

		/* We have to set the props via g_object_set() to stop vips
		 * complaining they are unset.
		 */
		g_object_set( min, 
			"out", m,
			"x", min->x,
			"y", min->y,
			NULL );
	}

	return( 0 );
}

/* New sequence value. Make a private VipsMin for this thread.
 */
static void *
vips_min_start( VipsStatistic *statistic )
{
	VipsMin *min;

	min = g_new( VipsMin, 1 );
	min->set = FALSE;

	return( (void *) min );
}

/* Merge the sequence value back into the per-call state.
 */
static int
vips_min_stop( VipsStatistic *statistic, void *seq )
{
	VipsMin *global = (VipsMin *) statistic;
	VipsMin *min = (VipsMin *) seq;

	if( min->set && 
		(!global->set || min->min < global->min) ) {
		global->min = min->min;
		global->x = min->x;
		global->y = min->y;
		global->set = TRUE;
	}

	g_free( min );

	return( 0 );
}

/* real min with a lower bound.
 */
#define LOOPL( TYPE, LOWER ) { \
	TYPE *p = (TYPE *) in; \
	TYPE m; \
	\
	if( min->set ) \
		m = min->min; \
	else { \
		m = p[0]; \
		min->x = x; \
		min->y = y; \
	} \
	\
	for( i = 0; i < sz; i++ ) { \
		if( p[i] < m ) { \
			m = p[i]; \
			min->x = x + i / bands; \
			min->y = y; \
			if( m <= LOWER ) { \
				statistic->stop = TRUE; \
				break; \
			} \
		} \
	} \
	\
	min->min = m; \
	min->set = TRUE; \
} 

/* float/double min ... no limits, and we have to avoid NaN.
 *
 * NaN compares false to every float value, so if we were to take the first
 * point in this buffer as our start min (as we do above) and it was NaN, we'd
 * never replace it with a true value.
 */
#define LOOPF( TYPE ) { \
	TYPE *p = (TYPE *) in; \
	TYPE m; \
	gboolean set; \
	\
	set = min->set; \
	m = min->min; \
	\
	for( i = 0; i < sz; i++ ) { \
		if( set ) { \
			if( p[i] < m ) { \
				m = p[i]; \
				min->x = x + i / bands; \
				min->y = y; \
			} \
		} \
		else if( !isnan( p[i] ) ) {  \
			m = p[i]; \
			min->x = x + i / bands; \
			min->y = y; \
			set = TRUE; \
		} \
	} \
	\
	if( set ) { \
		min->min = m; \
		min->set = TRUE; \
	} \
} 

/* As LOOPF, but complex. Track min(mod) to avoid sqrt().
 */
#define LOOPC( TYPE ) { \
	TYPE *p = (TYPE *) in; \
	TYPE m; \
	gboolean set; \
	\
	set = min->set; \
	m = min->min; \
	\
	for( i = 0; i < sz; i++ ) { \
		TYPE mod; \
		\
		mod = p[0] * p[0] + p[1] * p[1]; \
		p += 2; \
		\
		if( set ) { \
			if( mod > m ) { \
				m = mod; \
				min->x = x + i / bands; \
				min->y = y; \
			} \
		} \
		else if( !isnan( mod ) ) {  \
			m = mod; \
			min->x = x + i / bands; \
			min->y = y; \
			set = TRUE; \
		} \
	} \
	\
	if( set ) { \
		min->min = m; \
		min->set = TRUE; \
	} \
} 

/* Loop over region, adding to seq.
 */
static int
vips_min_scan( VipsStatistic *statistic, void *seq, 
	int x, int y, void *in, int n )
{
	VipsMin *min = (VipsMin *) seq;
	const int bands = vips_image_get_bands( statistic->in );
	const int sz = n * bands;

	int i;

	switch( vips_image_get_format( statistic->in ) ) {
	case VIPS_FORMAT_UCHAR:		
		LOOPL( unsigned char, 0 ); break; 
	case VIPS_FORMAT_CHAR:		
		LOOPL( signed char, SCHAR_MIN ); break; 
	case VIPS_FORMAT_USHORT:		
		LOOPL( unsigned short, 0 ); break; 
	case VIPS_FORMAT_SHORT:		
		LOOPL( signed short, SHRT_MIN ); break; 
	case VIPS_FORMAT_UINT:		
		LOOPL( unsigned int, 0 ); break;
	case VIPS_FORMAT_INT:		
		LOOPL( signed int, INT_MIN ); break; 

	case VIPS_FORMAT_FLOAT:		
		LOOPF( float ); break; 
	case VIPS_FORMAT_DOUBLE:		
		LOOPF( double ); break; 

	case VIPS_FORMAT_COMPLEX:	
		LOOPC( float ); break; 
	case VIPS_FORMAT_DPCOMPLEX:	
		LOOPC( double ); break; 

	default:  
		g_assert( 0 );
	}

	return( 0 );
}

static void
vips_min_class_init( VipsMinClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	VipsObjectClass *object_class = (VipsObjectClass *) class;
	VipsStatisticClass *sclass = VIPS_STATISTIC_CLASS( class );

	gobject_class->set_property = vips_object_set_property;
	gobject_class->get_property = vips_object_get_property;

	object_class->nickname = "min";
	object_class->description = _( "find image minimum" );
	object_class->build = vips_min_build;

	sclass->start = vips_min_start;
	sclass->scan = vips_min_scan;
	sclass->stop = vips_min_stop;

	VIPS_ARG_DOUBLE( class, "out", 1, 
		_( "Output" ), 
		_( "Output value" ),
		VIPS_ARGUMENT_REQUIRED_OUTPUT,
		G_STRUCT_OFFSET( VipsMin, min ),
		-INFINITY, INFINITY, 0.0 );

	VIPS_ARG_INT( class, "x", 2, 
		_( "x" ), 
		_( "Horizontal position of minimum" ),
		VIPS_ARGUMENT_OPTIONAL_OUTPUT,
		G_STRUCT_OFFSET( VipsMin, x ),
		0, 1000000, 0 );

	VIPS_ARG_INT( class, "y", 2, 
		_( "y" ), 
		_( "Vertical position of minimum" ),
		VIPS_ARGUMENT_OPTIONAL_OUTPUT,
		G_STRUCT_OFFSET( VipsMin, y ),
		0, 1000000, 0 );
}

static void
vips_min_init( VipsMin *min )
{
}

/**
 * vips_min:
 * @in: input #VipsImage
 * @out: output pixel maximum
 * @...: %NULL-terminated list of optional named arguments
 *
 * Optional arguments:
 *
 * @x: horizontal position of minimum
 * @y: vertical position of minimum
 *
 * This operation finds the minimum value in an image. 
 *
 * If the image contains several minimum values, only the first one found is
 * returned.
 *
 * It operates on all 
 * bands of the input image: use vips_stats() if you need to find an 
 * minimum for each band. 
 *
 * For complex images, this operation finds the minimum modulus.
 *
 * See also: vips_max(), vips_stats().
 *
 * Returns: 0 on success, -1 on error
 */
int
vips_min( VipsImage *in, double *out, ... )
{
	va_list ap;
	int result;

	va_start( ap, out );
	result = vips_call_split( "min", ap, in, out );
	va_end( ap );

	return( result );
}
