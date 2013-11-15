/* Private include file ... if we've been configured without gthread, we need
 * to point the g_thread_*() and g_mutex_*() functions at our own stubs.
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

#ifndef VIPS_THREAD_H
#define VIPS_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/* Stack size for each thread. We need to set this explicitly because some
 * systems have a very low default.

 	FIXME ...  should have an environment variable for this?

 */
#define VIPS__DEFAULT_STACK_SIZE (2 * 1024 * 1024)

#ifndef HAVE_THREADS
#undef g_thread_supported
#define g_thread_supported() (0)

#define g_thread_init vips__g_thread_init
#define g_thread_join vips__g_thread_join
#define g_thread_self vips__g_thread_self
#define g_thread_create_full vips__g_thread_create_full

/* We don't need a shadow imlementation of g_thread_create(), even though we
 * use it, because it's just a macro over g_thread_create_full().
 */

void vips__g_thread_init( GThreadFunctions *vtable );
gpointer vips__g_thread_join( GThread * );
gpointer vips__g_thread_self( void );
GThread *vips__g_thread_create_full( GThreadFunc, 
	gpointer, gulong, gboolean, gboolean, GThreadPriority, GError ** );

#undef g_mutex_new
#undef g_mutex_free
#undef g_mutex_lock
#undef g_mutex_unlock

#define g_mutex_new vips__g_mutex_new
#define g_mutex_free vips__g_mutex_free
#define g_mutex_lock vips__g_mutex_lock
#define g_mutex_unlock vips__g_mutex_unlock

GMutex *vips__g_mutex_new( void );
void vips__g_mutex_free( GMutex * );
void vips__g_mutex_lock( GMutex * );
void vips__g_mutex_unlock( GMutex * );
#endif /*!HAVE_THREADS*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*VIPS_THREAD_H*/
