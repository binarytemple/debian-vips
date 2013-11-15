/* Convert 1 to 4-band 8 or 16-bit VIPS images to/from PNG.
 *
 * 19/12/11
 * 	- just a stub
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

#include <vips/vips.h>
#include <vips/internal.h>

int
im_png2vips( const char *name, IMAGE *out )
{
	char filename[FILENAME_MAX];
	char mode[FILENAME_MAX];
	char *p, *q;
	gboolean sequential;
	VipsImage *x;

	im_filename_split( name, filename, mode );

	sequential = FALSE;
	p = &mode[0];
	if( (q = im_getnextoption( &p )) ) {
		if( im_isprefix( "seq", q ) )
			sequential = TRUE;
	}

	if( vips_pngload( filename, &x, 
		"sequential", sequential,
		NULL ) )
		return( -1 );

	if( vips_image_write( x, out ) ) {
		g_object_unref( x );
		return( -1 );
	}
	g_object_unref( x );

	return( 0 );
}

static int
ispng( const char *filename )
{
	return( vips_foreign_is_a( "pngload", filename ) );
}

static const char *png_suffs[] = { ".png", NULL };

typedef VipsFormat VipsFormatPng;
typedef VipsFormatClass VipsFormatPngClass;

static void
vips_format_png_class_init( VipsFormatPngClass *class )
{
	VipsObjectClass *object_class = (VipsObjectClass *) class;
	VipsFormatClass *format_class = (VipsFormatClass *) class;

	object_class->nickname = "png";
	object_class->description = _( "PNG" );

	format_class->is_a = ispng;
	format_class->load = im_png2vips;
	format_class->save = im_vips2png;
	format_class->suffs = png_suffs;
}

static void
vips_format_png_init( VipsFormatPng *object )
{
}

G_DEFINE_TYPE( VipsFormatPng, vips_format_png, VIPS_TYPE_FORMAT );

