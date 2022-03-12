#ifndef KSPOBJECT_H

#define KSPOBJECT_H

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

class kspObject
{
	public:
		kspObject(Display *display, int ID, int X, int Y, unsigned int W, unsigned int H, const char *Label, const char *Text, bool Interactive);
		~kspObject(void);
		
		virtual void draw(Display *display, Window window, Drawable d);
		void setText(const char *Text);
		void activate(void);
		void deactivate(void);
		int ID(void) {return _ID;};
		
		bool hitTest(int X, int Y);		// true is the point is within the object
		
		
	protected:
		int _ID;
		bool _isInteractive = false;
		int _x;
		int _y;
		unsigned int _w;
		unsigned int _h;
		bool _isActive = false;		// used to show binary state
		
		char _Buffer[40] = {0};
		char _Label[40] = {0};
		bool _showLabel = false;
		
		XFontStruct *_textFont;
		XFontStruct *_labelFont;
		
		char _activeRGB[8] = "#00FF00";
		char _inactiveRGB[8]= "#003300";
};

class kspBarIndicator : public kspObject
{
	public:
		kspBarIndicator(Display *display, int ID, int X, int Y, unsigned int W, unsigned int H, const char *Label, const char *Text, bool Interactive);
		void draw(Display *display, Window window, Drawable d);
		void setValue(float Value);
		void setBarColour(const char *activeRGB, const char *inactiveRGB);
		
	private:
		float _value = 0.0;
		char _activeRGB[8] = "#00AA00";
		char _inactiveRGB[8]= "#003300";
};

class kspLEDIndicator : public kspObject
{
	public:
		kspLEDIndicator(Display *display, int ID, int X, int Y, unsigned int W, unsigned int H, const char *Label, const char *Text, bool Interactive);
	
		void draw(Display *display, Window window, Drawable d);
		
		void setLEDColour(const char *activeRGB, const char *inactiveRGB);
		
		
};

#endif
