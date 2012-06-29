/*=========================================================================

 gif-io.c, part of ENIAC. 
 Electrical Network Interactive Console - 2008

 Based around original GIF code by David Koblas.
 Modified by Adam D. Moss.
 Modified by Sven Neumann.
 Modified by Andrea Viarengo.
 
 ========================================================================== 
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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 ==========================================================================*/

/* GIF saving and loading routines stripped out of the GIF filter for The GIMP.
 *
 * Modified for Pipmak game Engine by Andrea Viarengo in 2008 <andrea.viarengo@gmail.com>
 * Modified for blinkentools in 2001 by Sven Neumann  <sven@gimp.org>.
 * Included into blib in 2002 by Sven Neumann  <sven@gimp.org>.
 *
 * GIMP plug-in written by Adam D. Moss  <adam@gimp.org> <adam@foxbox.org>
 *
 * Based around original GIF code by David Koblas.
 *
 * This filter uses code taken from the "giftopnm" and "ppmtogif" programs
 *    which are part of the "netpbm" package.
 *
 *  "The Graphics Interchange Format(c) is the Copyright property of
 *  CompuServe Incorporated.  GIF(sm) is a Service Mark property of
 *  CompuServe Incorporated."
 */

/* Copyright notice for GIF code from which this plugin was long ago     *
 * derived (David Koblas has granted permission to relicense):           *
 * +-------------------------------------------------------------------+ *
 * | Copyright 1990, 1991, 1993, David Koblas.  (koblas@extra.com)     | *
 * +-------------------------------------------------------------------+ */

#ifndef __GIF_IO_H__
#define __GIF_IO_H__

#ifdef  _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE 
#define _CRT_NONSTDC_NO_DEPRECATE
#endif  /* _MSC_VER */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* Typedef specifications */
typedef enum GIFDisposeType {DISPOSE_UNSPECIFIED, DISPOSE_COMBINE, DISPOSE_REPLACE } GIFDisposeType;
typedef enum GIFRecordType {IMAGE, GRAPHIC_CONTROL_EXTENSION, COMMENT_EXTENSION, UNKNOWN_EXTENSION, TERMINATOR } GIFRecordType;

/* Saving procedures */
void GIFEncodeHeader            (FILE *fp, int gif89, int Width, int Height, int Background, int BitsPerPixel, unsigned char *cmap);
void GIFEncodeGraphicControlExt (FILE *fp, GIFDisposeType Disposal, int Delay, int Animation, int Transparent);
void GIFEncodeImageData         (FILE *fp, int Width, int Height, int BitsPerPixel, int offset_x, int offset_y, unsigned char *data);
void GIFEncodeClose             (FILE *fp);
void GIFEncodeCommentExt        (FILE *fp, char *comment);
void GIFEncodeLoopExt           (FILE *fp, int num_loops);

/* Loading procedures */
int  GIFDecodeHeader            (FILE *io, int skip_magic, int *Width, int *Height, int *Background, int *colors, unsigned char  **cmap);
int  GIFDecodeRecordType        (FILE *io, GIFRecordType *type );
int  GIFDecodeImage             (FILE *io, int *Width, int *Height, int *offx, int *offy, int *colors,unsigned char **cmap, unsigned char *data);
int  GIFDecodeGraphicControlExt (FILE *io, GIFDisposeType *Disposal, int *Delay, int *Transparent);
int  GIFDecodeCommentExt        (FILE *io, char **comment);
void GIFDecodeUnknownExt        (FILE *io);

#ifdef  __cplusplus
}
#endif

#endif /* __GIF_IO_H__ */
