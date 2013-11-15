/* base class for all unary arithmetic operations
 */

/*

    Copyright (C) 1991-2005 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#ifndef VIPS_UNARY_H
#define VIPS_UNARY_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "arithmetic.h"

#define VIPS_TYPE_UNARY (vips_unary_get_type())
#define VIPS_UNARY( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), VIPS_TYPE_UNARY, VipsUnary ))
#define VIPS_UNARY_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), VIPS_TYPE_UNARY, VipsUnaryClass))
#define VIPS_IS_UNARY( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), VIPS_TYPE_UNARY ))
#define VIPS_IS_UNARY_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), VIPS_TYPE_UNARY ))
#define VIPS_UNARY_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), VIPS_TYPE_UNARY, VipsUnaryClass ))

typedef struct _VipsUnary {
	VipsArithmetic parent_instance;

	VipsImage *in;

} VipsUnary;

typedef VipsArithmeticClass VipsUnaryClass;

GType vips_unary_get_type( void );

int vips_unary_copy( VipsUnary *unary );

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*VIPS_UNARY_H*/


