/*
 * Motif
 *
 * Copyright (c) 2026 atomchild411.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
#ifndef _XmIrixStyleI_h
#define _XmIrixStyleI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The return glyph on the default button: its width, its gap from the
 * right edge of the face, and how far the label moves left to make room. */
#define IRIX_GLYPH_WIDTH	8
#define IRIX_GLYPH_INSET	5
#define IRIX_GLYPH_SHIFT	((IRIX_GLYPH_WIDTH + IRIX_GLYPH_INSET) / 2)

extern void _XmIrixDrawButtonShadows(Widget w, Drawable d, Pixel face,
				     int x, int y, int width, int height,
				     int thickness, Boolean armed);
extern void _XmIrixDrawDefaultGlyph(Widget w, Drawable d, Pixel face,
				    Pixel fg, int right, int baseline);

#ifdef __cplusplus
}
#endif

#endif /* _XmIrixStyleI_h */
