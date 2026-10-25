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

/*
 * Drawing for the IRIX look (XmNenableIrixStyle): push buttons outside
 * menus get a dark outline around a two-tone bevel, and the default
 * button a return-key glyph instead of the default button shadow.
 * The shades are measured from IRIX 6.5 and derived from the face colour.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "XmI.h"
#include "IrixStyleI.h"

enum { OUTLINE_TL, OUTLINE_BR, LIGHT, LIGHT_INNER, DARK, DARK_INNER,
       GLYPH_SHADOW, NSHADES };

/* Each shade as a fraction (/256) of the way from the face colour to
 * white (positive) or to black (negative). */
static const int shade_mix[NSHADES] = {
	-87,	/* outline, top and left */
	-145,	/* outline, bottom and right */
	256,	/* outer light ring */
	136,	/* inner light rings */
	-87,	/* outer dark ring */
	-39,	/* inner dark rings */
	-26,	/* under the return glyph */
};

typedef struct _IrixShades {
	Display *display;
	Colormap colormap;
	Pixel face;
	Pixel pixel[NSHADES];
	struct _IrixShades *next;
} IrixShades;

static IrixShades *shades_cache;

static unsigned short
Mix(unsigned short c, int mix)
{
	if (mix >= 0)
		return c + (unsigned short)(((long)(65535 - c) * mix) >> 8);
	return c - (unsigned short)(((long)c * -mix) >> 8);
}

static IrixShades *
GetShades(Widget w, Pixel face)
{
	Widget cw = XtIsWidget(w) ? w : XtParent(w);
	Display *dpy = XtDisplay(cw);
	Colormap cmap = cw->core.colormap;
	IrixShades *s;
	XColor base, c;
	int i;

	_XmProcessLock();
	for (s = shades_cache; s; s = s->next)
		if (s->display == dpy && s->colormap == cmap && s->face == face)
			break;
	if (!s) {
		s = (IrixShades *)XtMalloc(sizeof *s);
		s->display = dpy;
		s->colormap = cmap;
		s->face = face;
		base.pixel = face;
		XQueryColor(dpy, cmap, &base);
		for (i = 0; i < NSHADES; i++) {
			c.red = Mix(base.red, shade_mix[i]);
			c.green = Mix(base.green, shade_mix[i]);
			c.blue = Mix(base.blue, shade_mix[i]);
			c.flags = DoRed | DoGreen | DoBlue;
			s->pixel[i] = XAllocColor(dpy, cmap, &c) ? c.pixel
				: (shade_mix[i] > 0 ? WhitePixelOfScreen(XtScreen(cw))
				   : BlackPixelOfScreen(XtScreen(cw)));
		}
		s->next = shades_cache;
		shades_cache = s;
	}
	_XmProcessUnlock();
	return s;
}

static GC
ShadeGC(Widget w, Pixel pixel)
{
	XGCValues v;

	v.foreground = pixel;
	return XtGetGC(XtIsWidget(w) ? w : XtParent(w), GCForeground, &v);
}

/* One ring: top and left full length, bottom and right one pixel in. */
static void
Ring(Display *dpy, Drawable d, GC tl, GC br, int x, int y, int w, int h)
{
	XFillRectangle(dpy, d, tl, x, y, w, 1);
	XFillRectangle(dpy, d, tl, x, y + 1, 1, h - 1);
	XFillRectangle(dpy, d, br, x + 1, y + h - 1, w - 1, 1);
	XFillRectangle(dpy, d, br, x + w - 1, y + 1, 1, h - 2);
}

void
_XmIrixDrawButtonShadows(Widget w, Drawable d, Pixel face,
			 int x, int y, int width, int height,
			 int thickness, Boolean armed)
{
	Display *dpy = XtDisplay(w);
	IrixShades *s = GetShades(w, face);
	GC gc[NSHADES], tl, br;
	int i;

	for (i = 0; i < NSHADES; i++)
		gc[i] = ShadeGC(w, s->pixel[i]);
	for (i = 0; i < thickness && width > 2 * i && height > 2 * i; i++) {
		if (i == 0) {
			tl = gc[OUTLINE_TL];
			br = gc[OUTLINE_BR];
		} else if (i == 1) {
			tl = gc[LIGHT];
			br = gc[DARK];
		} else {
			tl = gc[LIGHT_INNER];
			br = gc[DARK_INNER];
		}
		if (armed && i > 0) {
			GC t = tl; tl = br; br = t;
		}
		Ring(dpy, d, tl, br, x + i, y + i, width - 2 * i, height - 2 * i);
	}
	for (i = 0; i < NSHADES; i++)
		XtReleaseGC(XtIsWidget(w) ? w : XtParent(w), gc[i]);
}

void
_XmIrixDrawDefaultGlyph(Widget w, Drawable d, Pixel face, Pixel fg,
			int right, int baseline)
{
	Widget cw = XtIsWidget(w) ? w : XtParent(w);
	IrixShades *s = GetShades(w, face);
	GC ink = ShadeGC(w, fg), shadow = ShadeGC(w, s->pixel[GLYPH_SHADOW]);
	int left = right - IRIX_GLYPH_WIDTH + 1;

	/* A stem down to a foot pointing left, shadowed underneath. */
	XFillRectangle(XtDisplay(w), d, ink, right, baseline - 6, 1, 6);
	XFillRectangle(XtDisplay(w), d, ink, left, baseline, IRIX_GLYPH_WIDTH, 1);
	XFillRectangle(XtDisplay(w), d, shadow, left, baseline + 1,
		       IRIX_GLYPH_WIDTH, 1);
	XtReleaseGC(cw, ink);
	XtReleaseGC(cw, shadow);
}
