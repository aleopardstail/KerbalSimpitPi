#include "kspObject.h"

kspObject::kspObject(Display *display, int ID, int X, int Y, unsigned int W, unsigned int H, const char *Label, const char *Text, bool Interactive)
{
	_ID = ID;
	_isInteractive = Interactive;
	_x = X;
	_y = Y;
	_w = W;
	_h = H;
	
	sprintf(_Buffer, Text);
	sprintf(_Label, Label);
	if (Label != NULL)
	{
		_showLabel = true;
	}
	
	_labelFont = XLoadQueryFont(display, "-adobe-helvetica-medium-r-normal--11-80-100-100-p-56-iso8859-1");
	_textFont = XLoadQueryFont(display, "-adobe-helvetica-bold-r-normal--20-140-100-100-p-103-iso8859-1");
	
	return;
}

kspObject::~kspObject(void)
{
	
	return;
}

void kspObject::draw(Display *display, Window window, Drawable drawable)
{
	XColor frame_col, fill_col;
	
	GC gc = XCreateGC(display, drawable, 0, NULL);
	XSetGraphicsExposures(display, gc, false);
	Colormap colormap = DefaultColormap(display, 0);
	if (_isActive)
	{
		XParseColor(display, colormap, "#00FF00", &frame_col);
		XParseColor(display, colormap, "#003300", &fill_col);
	}
	else
	{
		XParseColor(display, colormap, "#00BCD4", &frame_col);
		XParseColor(display, colormap, "#333333", &fill_col);
	}
	XAllocColor(display, colormap, &frame_col);
	XAllocColor(display, colormap, &fill_col);
	
	if (_isInteractive)
	{
		XSetLineAttributes(display, gc, 3, LineSolid, CapRound, JoinBevel);
	}
	else
	{
		XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinBevel);
	}
	
	
	// draw the background
	XSetForeground(display, gc, fill_col.pixel);	
	XFillRectangle(display, drawable, gc, _x, _y, _w, _h);
	
	// add a frame
	XSetForeground(display, gc, frame_col.pixel);	
	XDrawRectangle(display, drawable, gc, _x, _y, _w, _h);
	
	// draw the label, if there is one
	if (_showLabel)
	{
		//XFontStruct *myFont = XLoadQueryFont(display, "-adobe-helvetica-medium-r-normal--11-80-100-100-p-56-iso8859-1");
		XSetFont(display, gc, _labelFont->fid);
		int th = _labelFont->ascent + _labelFont->descent;
		
		XDrawString(display, drawable, gc, _x+5, _y+th, _Label, strlen(_Label));
		//XUnloadFont(display, myFont->fid);
	}
	
	
	// draw the text (centred)
	//XFontStruct *myFont = XLoadQueryFont(display, "-adobe-helvetica-bold-r-normal--20-140-100-100-p-103-iso8859-1");
	XSetFont(display, gc, _textFont->fid);
	// get the dimensions of our text
	int tw = XTextWidth(_textFont, _Buffer, strlen(_Buffer));
	int th = _textFont->ascent + _textFont->descent;
	
	XDrawString(display, drawable, gc, (_x + (_w/2) - (tw/2)), (_y + (_h/2) + (th/2)), _Buffer, strlen(_Buffer));
	
	//XUnloadFont(display, myFont->fid);	// we loaded it, so we unload it
	
	XFreeGC(display, gc);				// we created it, so we get rid of it
	XFlush(display);					// directly tell the X Server to actually draw this stuff
	return;
}

void kspObject::setText(const char *Text)
{
	sprintf(_Buffer, Text);
	
	return;
}

void kspObject::activate(void)
{
	//printf("\nActivate!"); fflush(stdout);
	_isActive = true;
	return;
}

void kspObject::deactivate(void)
{
	//printf("\nDeactivate!"); fflush(stdout);
	_isActive = false;
	return;
}

bool kspObject::hitTest(int X, int Y)
{
	if ((X >= _x) && (X <= _x + (int)_w) && (Y >= _y) && (Y <= _y + (int)_h))
	{
		return true;
	}
	
	return false;
}

// --------------------------------------------------------------------

kspBarIndicator::kspBarIndicator(Display *display, int ID, int X, int Y, unsigned int W, unsigned int H, const char *Label, const char *Text, bool Interactive) : kspObject(display, ID, X, Y, W, H, Label, Text, Interactive)
{
	
	_isActive = true;
	return;
}

void kspBarIndicator::draw(Display *display, Window window, Drawable drawable)
{
	XColor frame_col, fill_col, bar_col;
	
	GC gc = XCreateGC(display, drawable, 0, NULL);
	XSetGraphicsExposures(display, gc, false);
	Colormap colormap = DefaultColormap(display, 0);
	if (_isActive)
	{
		XParseColor(display, colormap, "#00BCD4", &frame_col);
		XParseColor(display, colormap, "#333333", &fill_col);
		XParseColor(display, colormap, _activeRGB, &bar_col);
	}
	else
	{
		XParseColor(display, colormap, "#00BCD4", &frame_col);
		XParseColor(display, colormap, "#333333", &fill_col);
		XParseColor(display, colormap, _inactiveRGB, &bar_col);
	}
	XAllocColor(display, colormap, &frame_col);
	XAllocColor(display, colormap, &fill_col);
	XAllocColor(display, colormap, &bar_col);
	
	if (_isInteractive)
	{
		XSetLineAttributes(display, gc, 3, LineSolid, CapRound, JoinBevel);
	}
	else
	{
		XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinBevel);
	}
	
	// draw the background
	XSetForeground(display, gc, fill_col.pixel);	
	XFillRectangle(display, drawable, gc, _x, _y, _w, _h);
	
	// add a frame
	XSetForeground(display, gc, frame_col.pixel);	
	XDrawRectangle(display, drawable, gc, _x, _y, _w, _h);
	
	// draw the label, if there is one
	if (_showLabel)
	{
		//XFontStruct *myFont = XLoadQueryFont(display, "-adobe-helvetica-medium-r-normal--11-80-100-100-p-56-iso8859-1");
		XSetFont(display, gc, _labelFont->fid);
		int th = _labelFont->ascent + _labelFont->descent;
		
		XDrawString(display, drawable, gc, _x+5, _y+th, _Label, strlen(_Label));
		//XUnloadFont(display, myFont->fid);
	}
	
	// draw the bar
	int bw = (int)(((float)(_w - 10) / 100.0) * _value);
	
	XSetForeground(display, gc, bar_col.pixel);	
	XDrawRectangle(display, drawable, gc, _x+5, _y + _h - (_h/2) - 5, _w - 10, _h/2);
	XFillRectangle(display, drawable, gc, _x+5, _y + _h - (_h/2) - 5, bw, _h/2);
	
	
	XFreeGC(display, gc);				// we created it, so we get rid of it
	XFlush(display);					// directly tell the X Server to actually draw this stuff
	
	return;
}

void kspBarIndicator::setValue(float Value)
{
	_value = Value;
	return;
}

void kspBarIndicator::setBarColour(const char *activeRGB, const char *inactiveRGB)
{
	sprintf(_activeRGB, activeRGB);
	sprintf(_inactiveRGB, inactiveRGB);
	return;
}

// --------------------------------------------------------------------

kspLEDIndicator::kspLEDIndicator(Display *display, int ID, int X, int Y, unsigned int W, unsigned int H, const char *Label, const char *Text, bool Interactive) : kspObject(display, ID, X, Y, W, H, Label, Text, Interactive)
{
	_textFont = XLoadQueryFont(display, "-adobe-helvetica-medium-r-normal--14-100-100-100-p-78-iso10646-1");
	
	return;
}

void kspLEDIndicator::draw(Display *display, Window window, Drawable drawable)
{
	XColor frame_col, fill_col, LED_col;
	
	GC gc = XCreateGC(display, drawable, 0, NULL);
	XSetGraphicsExposures(display, gc, false);
	Colormap colormap = DefaultColormap(display, 0);
	if (_isActive)
	{
		XParseColor(display, colormap, "#00BCD4", &frame_col);
		XParseColor(display, colormap, "#333333", &fill_col);
		XParseColor(display, colormap, _activeRGB, &LED_col);
	}
	else
	{
		XParseColor(display, colormap, "#00BCD4", &frame_col);
		XParseColor(display, colormap, "#333333", &fill_col);
		XParseColor(display, colormap, _inactiveRGB, &LED_col);
	}
	XAllocColor(display, colormap, &frame_col);
	XAllocColor(display, colormap, &fill_col);
	XAllocColor(display, colormap, &LED_col);
	
	if (_isInteractive)
	{
		XSetLineAttributes(display, gc, 3, LineSolid, CapRound, JoinBevel);
	}
	else
	{
		XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinBevel);
	}
	
	
	// draw the background
	XSetForeground(display, gc, fill_col.pixel);	
	XFillRectangle(display, drawable, gc, _x, _y, _w, _h);
		
	//XDrawRectangle(display, drawable, gc, _x, _y, _w, _h);
	
	// draw the LED
	XSetForeground(display, gc, LED_col.pixel);
	XFillArc(display, drawable, gc, _x + 1, _y+1, _h-2, _h-2, 0, 360*64);
	
	// draw the text (centred)
	XSetForeground(display, gc, frame_col.pixel);
	//XFontStruct *myFont = XLoadQueryFont(display, "-adobe-helvetica-medium-r-normal--14-100-100-100-p-78-iso10646-1");
	XSetFont(display, gc, _textFont->fid);
	// get the dimensions of our text
	//int tw = XTextWidth(myFont, _Buffer, strlen(_Buffer));
	int th = _textFont->ascent + _textFont->descent;
	
	XDrawString(display, drawable, gc, (_x + _h + 5), (_y + (_h/2) + (th/2)), _Buffer, strlen(_Buffer));
	
	//XUnloadFont(display, myFont->fid);	// we loaded it, so we unload it
	XFreeGC(display, gc);				// we created it, so we get rid of it
	XFlush(display);					// directly tell the X Server to actually draw this stuff
	
	return;
}

void kspLEDIndicator::setLEDColour(const char *activeRGB, const char *inactiveRGB)
{
	sprintf(_activeRGB, activeRGB);
	sprintf(_inactiveRGB, inactiveRGB);
	return;
}
