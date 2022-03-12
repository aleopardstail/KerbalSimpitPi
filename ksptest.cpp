/* ksptest
 * 
 * X11 test programme able to talk to the serial bus to accept KSPComms bus stuff
 * 
 * 
 * 
 * */

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
//#include <X11/keysymdef.h>		// keypress symbol codes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <map>
#include <string>

#include "kspSimpit.h"
#include "kspObject.h"

#define GEAR_BUTTON		10
#define LIGHT_BUTTON	11
#define RCS_BUTTON		12
#define SAS_BUTTON		13
#define BRAKE_BUTTON	14

#define XK_Escape                        0xff1b

KSPSimpit mySimpit;

void myMessageHandler(KSPMessage *);
void myHandshakeHandler(KSPMessage *);
void onClick(int ID);

uint8_t ActionGroupCurrentStatus = 0;

Display *d = NULL;		// X11 display
Window	w;		// X11 window handle
XEvent	e;		// X11 Event
int s;			// X11 screen
GC gc;
Pixmap double_buffer;
XColor white_col, pen_col;


std::map<std::string, kspObject *> objects;

int main(int argc, char **argv)
{
	//const char *msg = "KSP Test";
	
	d = XOpenDisplay(NULL);		// open a local display
	if (d == NULL)
	{
		fprintf(stderr, "Unable to open display\n");
		exit(EXIT_FAILURE);
	}
	
	// altitude information
	kspObject obj1(d, 1, 600, 100, 195, 48, "Altitude", "-", false); objects["sealevel"] = &obj1;
	kspObject obj2(d, 2, 800, 100, 195, 48, "Radar Altitude", "-", false); objects["surface"] = &obj2;
	
	// SoI information
	kspObject obj3(d, 3, 600, 10, 230, 48, "Sphere of Influence", "-", false); objects["SoI"] = &obj3;
	
	// velocity information
	kspObject obj4(d, 4, 600, 160, 128, 48, "Orbital Velocity", "-", false); 	objects["orbital_velocity"] = &obj4;
	kspObject obj5(d, 5, 733, 160, 129, 48, "Surface Velocity", "-", false); 	objects["surface_velocity"] = &obj5;
	kspObject obj6(d, 6, 865, 160, 128, 48, "Vertical Velocity", "-", false); 	objects["vertical_velocity"] = &obj6;
	
	// airspeed information
	kspObject obj7(d, 7, 600, 220, 195, 48, "Indiacted Airspeed", "-", false); 		objects["indicated_airspeed"] = &obj7;
	kspObject obj8(d, 8, 800, 220, 195, 48, "Mach No.", "-", false); 		objects["mach_number"] = &obj8;
	
	// standard action groups
	kspObject obj9(d, 9, 400, 10, 64, 64, NULL, "Stage", false); 		objects["stage_action"] = &obj9;
	kspObject obj10(d, GEAR_BUTTON,400, 120, 64, 64, NULL, "Gear", true); 		objects["gear_action"] = &obj10;
	kspObject obj11(d, LIGHT_BUTTON, 400, 190, 64, 64, NULL, "Light", true); 	objects["light_action"] = &obj11;
	kspObject obj12(d, RCS_BUTTON, 470, 120, 64, 64, NULL, "RCS", true); 		objects["rcs_action"] = &obj12;
	kspObject obj13(d, SAS_BUTTON, 200, 120, 64, 64, NULL, "SAS", true); 		objects["sas_action"] = &obj13;
	kspObject obj14(d, BRAKE_BUTTON, 470, 190, 64, 64, NULL, "Brake", true); 	objects["brakes_action"] = &obj14;
	kspObject obj15(d, 15, 470, 10, 64, 64, NULL, "Abort", false); 	objects["abort_action"] = &obj15;
	
	// flight status message
	kspLEDIndicator obj16(d, 16, 10, 100, 150, 24, NULL, "In Flight", false); 		objects["isInFlight"] = (kspObject *)&obj16;
	kspLEDIndicator obj17(d, 17, 10, 130, 150, 24, NULL, "In EVA", false); 		objects["isInEVA"] = &obj17;
	kspLEDIndicator obj18(d, 18, 10, 160, 150, 24, NULL, "Recoverable", false); 		objects["isRecoverable"] = &obj18;
	kspLEDIndicator obj19(d, 19, 10, 190, 150, 24, NULL, "Atmosphere TW", false); 		objects["isInAtmoTW"] = &obj19;
	kspLEDIndicator obj20(d, 20, 10, 220, 150, 24, NULL, "Has Target", false); 		objects["hasTarget"] = &obj20;
	kspLEDIndicator obj21(d, 21, 10, 280, 150, 24, NULL, "Control Level", false); 		objects["ControlLevel"] = &obj21;
	
	kspLEDIndicator obj22(d, 9, 10, 310, 150, 24, NULL, "Situation", false); 		objects["vesselSituation"] = &obj22;
	kspObject obj23(d, 9, 760, 340, 75, 48, "Time Warp", "-", false); 		objects["currentTWIndex"] = &obj23;
	kspObject obj24(d, 9, 840, 340, 75, 48, "Crew Capacity", "-", false); 		objects["crewCapacity"] = &obj24;
	kspObject obj25(d, 9, 840, 390, 75, 48, "Current Crew", "-", false); 		objects["crewCount"] = &obj25;
	kspBarIndicator obj26(d, 9, 10, 400, 200, 48, "Comm Net Signal", "-", false); 		objects["commNet"] = &obj26;
	kspObject obj27(d, 9, 760, 390, 75, 48, "Current Stage", "-", false); 		objects["currentStage"] = &obj27;
	kspObject obj28(d, 9, 10, 10, 250, 48, "Vessel Type", "-", false); 		objects["vesselType"] = &obj28;
	
	// atmospheric conditions
	kspLEDIndicator obj29(d, 9, 840, 8, 150, 24, NULL, "Has Atmosphere", false); 		objects["hasAtmosphere"] = &obj29;
	kspLEDIndicator obj30(d, 9, 840, 38, 150, 24, NULL, "Has Oxygen", false); 		objects["hasOxygen"] = &obj30;
	kspLEDIndicator obj31(d, 9, 10, 250, 150, 24, NULL, "In Atmosphere", false); 		objects["isVesselInAtmosphere"] = &obj31;
	kspObject obj32(d, 9, 600, 280, 128, 48, "Air Density", "-", false); 		objects["airDensity"] = &obj32;
	kspObject obj33(d, 9, 733, 280, 129, 48, "Air Temperature", "-", false); 		objects["temperature"] = &obj33;
	kspObject obj34(d, 9, 865, 280, 128, 48, "Air Pressure", "-", false); 		objects["pressure"] = &obj34;
	
	mySimpit.begin();
	mySimpit.onMessage = myMessageHandler;
	mySimpit.onHandshake = myHandshakeHandler;
	mySimpit.handshake();
	
	
	
	
	
	s = DefaultScreen(d);		// obtain a screen reference to the default screen
	
	w = XCreateSimpleWindow(d, 							// which display to use
							RootWindow(d, s), 			// Parent Window
							10, 						// int X coordinate, relative to parent
							10, 						// int Y coordinate, relative to parent
							1000, 						// unsigned int width
							500,						// unsigned int height 
							1, 							// unsigned int border width
							BlackPixel(d, s), 			// unsigned long border colour
							WhitePixel(d, s)); 			// unsigned long background colour
							
	XStoreName(d, w, "KSP Test");
	XSetWindowBackgroundPixmap(d, w, None);				// clears the default pixmap and kills flickering
							
	XSelectInput(	d, 									// which display to use
					w, 									// which sindow are we talking about
					ExposureMask | 						// we want the Expose Event
					ButtonPressMask |					// button down event
					ButtonReleaseMask |					// button up event
					KeyPressMask |						// notify for keyboard events
					StructureNotifyMask);				// notify when the window is mapped or unmapped
					
	XMapWindow(d, w);		// show the window, generates an Expose event on each InputOutput window
	XFlush(d);
	Atom WM_DELETE_WINDOW = XInternAtom(d, "WM_DELETE_WINDOW", false);
	if(!XSetWMProtocols(d, w, &WM_DELETE_WINDOW, 1))
	{
		printf("couldn't register WM_DELETE_WINDOW property\n");
	}
	
	// wait for the MapNotify event
	for (;;)
	{
		XEvent e;
		XNextEvent(d, &e);
		if (e.type == MapNotify)
		{
			break;
		}
	}
	
	gc = XCreateGC(d, w, 0, NULL);		// create a graphical context to work with
	XSetGraphicsExposures(d, gc, false);
	double_buffer = XCreatePixmap(d, w, 1000, 500, 24);		// create buffer pixmap
	
	Colormap colormap = DefaultColormap(d, 0);
	
	XParseColor(d, colormap, "#222222", &white_col);
	XAllocColor(d, colormap, &white_col);
	XSetForeground(d, gc, white_col.pixel);
	XFillRectangle(d, double_buffer, gc, 0, 0, 1000, 500);			// fill our rectangle with white
	
	// reset to a coloured foreground colour for later
	XParseColor(d, colormap, "#FF0000", &pen_col);
	XAllocColor(d, colormap, &pen_col);
	XSetForeground(d, gc, pen_col.pixel);
	//XFillRectangle(d, double_buffer, gc, 20, 20, 10, 10);
	//XDrawString(d, double_buffer, gc, 10, 50, msg, strlen(msg));
	
	std::map<std::string, kspObject *>::iterator it;
	for (it = objects.begin(); it != objects.end(); ++it)
	{
		it->second->draw(d, w, double_buffer);
	}
	
	// main event loop
	int WindowOpen = 1;
	while (WindowOpen)
	{
		while(XPending(d))
		{
			XNextEvent(d, &e);		// note this a a _blocking_ call
			switch (e.type)
			{
				case Expose:
				{
					// Expose event indicates the rectangle to display, or can redraw the lot
					
					// if there are many expose events ignore all but the last one
					if (e.xexpose.count > 0) {break;}	
										
					// copy the buffer to the screen
					XCopyArea(d, double_buffer, w, gc, 0, 0, 1000, 500, 0, 0);
					break;
				}
				
				// fired when a key is pressed
				case KeyPress:
				{
					XKeyPressedEvent *E = (XKeyPressedEvent *) &e;
					
					// E->keycode gets the keycode of the key being pressed, note the kycode is hardware specific
					
					// translate the keycode into something usable
					KeySym K = XkbKeycodeToKeysym(d, E->keycode, 0, E->state & ShiftMask ? 1 : 0);
					
					fprintf(stdout, "\nKey: %s\n", XKeysymToString(K));
					
					// on "escape" we exit
					if (K == XK_Escape)
					{
						XCloseDisplay(d);
						return EXIT_SUCCESS;
					}
					break;
				}
				
				// fired when a mouse button is pressed
				case ButtonPress:
				{
					XButtonPressedEvent *E = (XButtonPressedEvent *) &e;	// get the XButtonPressedEvent data
					
					//XFillRectangle(d, double_buffer, gc, E->x - 5, E->y - 5, 10, 10);	// draw something at that point
						
					// see if the click is within an object
					kspObject *myObject = NULL;
					std::map<std::string, kspObject *>::iterator it;
					for (it = objects.begin(); it != objects.end(); ++it)
					{
						if(it->second->hitTest(E->x, E->y))
						{
							myObject = it->second;
						}
					}
					
					if (myObject != NULL)
					{
						printf("\nhit %d", myObject->ID()); fflush(stdout);
						onClick(myObject->ID());
					}
					
						
					// trigger a redraw, note this could be better as still flickers a bit	
					XClearArea(d, w, 0, 0, 1000, 500, True);
					break;
				}
				
				// fired when a mouse button is released
				case ButtonRelease:
				{
					XButtonReleasedEvent *E = (XButtonReleasedEvent *) &e;	// get the XButtonRelasedEvent data
					
					E=E;
					
					break;
				}
				
				case DestroyNotify:
				{
					XDestroyWindowEvent *ev = (XDestroyWindowEvent *) &e;
					if (ev->window == w)
					{
						WindowOpen = 0;
					}
					break;
				}
				
				case ClientMessage:
				{
					XClientMessageEvent *ev = (XClientMessageEvent *) &e;
					if ((Atom)ev->data.l[0] == WM_DELETE_WINDOW)
					{
						XDestroyWindow(d, w);
						WindowOpen = 0;
					}
				}
				
				default:
				{
					
					break;
				}
			
			}
		}
		
		
		mySimpit.loop();
	}
	
	XFreePixmap(d, double_buffer);	// destroy the pixmap we created
	
	XCloseDisplay(d);
	return EXIT_SUCCESS;
}

void myMessageHandler(KSPMessage *msg)
{
	switch (msg->msgType)
	{
		case SYNC_MESSAGE:
		{
			break;
		}
		
		case ALTITUDE_MESSAGE:
		{
			altitudeMessage *data = (altitudeMessage *)msg->msgData;
			
			// we will write these into two of our objects
			char myBuffer[40];
			
			kspObject *o = objects["sealevel"];
			sprintf(myBuffer, "%0.2f m", data->sealevel);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			o = objects["surface"];
			sprintf(myBuffer, "%0.2f m", data->surface);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			XClearArea(d, w, 0, 0, 1000, 500, True);
			break;
		}
		
		case VELOCITY_MESSAGE:
		{
			velocityMessage *data = (velocityMessage *)msg->msgData;
			char myBuffer[40];
			
			kspObject *o = objects["orbital_velocity"];
			sprintf(myBuffer, "%0.2f m/s", data->orbital);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			o = objects["surface_velocity"];
			sprintf(myBuffer, "%0.2f m/s", data->surface);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			o = objects["vertical_velocity"];
			sprintf(myBuffer, "%0.2f m/s", data->vertical);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			XClearArea(d, w, 0, 0, 1000, 500, True);
			
			break;
		}
		
		case AIRSPEED_MESSAGE:
		{
			airspeedMessage *data = (airspeedMessage *)msg->msgData;
			
			// we will write these into two of our objects
			char myBuffer[40];
			
			kspObject *o = objects["indicated_airspeed"];
			sprintf(myBuffer, "%0.2f m/s", data->IAS);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			o = objects["mach_number"];
			sprintf(myBuffer, "%0.2f", data->mach);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			XClearArea(d, w, 0, 0, 1000, 500, True);
			break;
		}
		
		case FLIGHT_STATUS_MESSAGE:
		{
			flightStatusMessage *data = (flightStatusMessage *)msg->msgData;
			
			kspObject *o;
			kspLEDIndicator *oi;
			o = objects["isInFlight"]; 	(data->isInFlight()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["isInEVA"]; 	(data->isInEVA()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["isRecoverable"]; 	(data->isRecoverable()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["isInAtmoTW"]; 	(data->isInAtmoTW()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["hasTarget"]; 	(data->hasTarget()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			
			// grab the control level infomation
			char Buffer[40];
			oi = (kspLEDIndicator *)objects["ControlLevel"]; 
			oi->activate();
			
			switch(data->getControlLevel())
			{	
				case 0:		// no control
				{
					sprintf(Buffer, "No Control");
					oi->setLEDColour("#FF0000", "#330000");
					break;
				}
				
				case 1:		// partial (unmanned)
				{
					sprintf(Buffer, "Partial Control");
					oi->setLEDColour("#FFA500", "#332200");
					break;
				}
				
				case 2:		// partial (manned)
				{
					sprintf(Buffer, "Partial Control");
					oi->setLEDColour("#FFA500", "#332200");
					break;
				}
				
				case 3:		// full control
				{
					sprintf(Buffer, "Full Control");
					oi->setLEDColour("#00FF00", "#003300");
					break;
				}	
			}
			
			oi->setText(Buffer);
			oi->draw(d, w, double_buffer);
			
			oi = (kspLEDIndicator *)objects["vesselSituation"]; 
			oi->activate();
			switch (data->vesselSituation)
			{
				case 1:		// landed
				{
					sprintf(Buffer, "Landed");
					oi->setLEDColour("#00FF00", "#003300");
					break;
				}
				
				case 2:		// splashed down
				{
					sprintf(Buffer, "Splashed Down");
					oi->setLEDColour("#0000FF", "#000033");
					break;
				}
				
				case 4:		// pre-launch
				{
					sprintf(Buffer, "Pre-Launch");
					oi->setLEDColour("#FFFF00", "#333300");
					break;
				}
				
				case 8:		// flying
				{
					sprintf(Buffer, "In Flight");
					oi->setLEDColour("#00FFFF", "#003333");
					break;
				}
				
				case 16:	// sub orbital
				{
					sprintf(Buffer, "Sub Orbital");
					oi->setLEDColour("#FFA500", "#332200");
					break;
				}
				
				case 32:	// orbiting
				{
					sprintf(Buffer, "Orbiting");
					oi->setLEDColour("#00FF00", "#003300");
					break;
				}
				
				case 64:	// escaping
				{
					sprintf(Buffer, "Escaping");
					oi->setLEDColour("#00FF00", "#003300");
					break;
				}
				
				case 128:	// docked
				{
					sprintf(Buffer, "Docked");
					oi->setLEDColour("#00FF00", "#003300");
					break;
				}
				
				default:	// unknown or other
				{
					break;
				}
			}
			oi->setText(Buffer);
			oi->draw(d, w, double_buffer);
			
			o = objects["currentTWIndex"]; 
			sprintf(Buffer, "%d", data->currentTWIndex);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			o = objects["crewCapacity"]; 
			sprintf(Buffer, "%d", data->crewCapacity);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			o = objects["crewCount"]; 
			sprintf(Buffer, "%d", data->crewCount);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			kspBarIndicator *ob = (kspBarIndicator *) objects["commNet"];
			if (data->commNetSignalStrenghPercentage >= 50)
			{
				// good signal
				ob->setBarColour("#00AA00","#003300");
			}
			else if (data->commNetSignalStrenghPercentage >= 20)
			{
				// moderate signal
				ob->setBarColour("#AAAA00","#333300");
			}
			else if (data->commNetSignalStrenghPercentage >= 1)
			{
				// poor signal
				ob->setBarColour("#FFA500", "#332200");
			}
			else
			{
				// no signal
				ob->setBarColour("#FF0000","#330000");
			}
			ob->setValue(data->commNetSignalStrenghPercentage);
			ob->draw(d, w, double_buffer);
			
			o = objects["currentStage"]; 
			sprintf(Buffer, "%d", data->currentStage);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			o = objects["vesselType"];
			switch (data->vesselType)
			{
				case 0:			// debris
				{
					sprintf(Buffer, "Debris");
					break;
				}
				case 1:			// space object
				{
					sprintf(Buffer, "Space Object");
					break;
				}
				case 2:			// unknown
				{
					sprintf(Buffer, "Unknown");
					break;
				}
				case 3:			// Probe
				{
					sprintf(Buffer, "Probe");
					break;
				}
				case 4:			// relay
				{
					sprintf(Buffer, "Relay");
					break;
				}
				case 5:			// rover
				{
					sprintf(Buffer, "Rover");
					break;
				}
				case 6:			// lander
				{
					sprintf(Buffer, "Lander");
					break;
				}
				case 7:			// ship
				{	
					sprintf(Buffer, "Spaceship");
					break;
				}
				case 8:			// plane
				{
					sprintf(Buffer, "Aeroplane");
					break;
				}
				case 9:			// station
				{
					sprintf(Buffer, "Space Station");
					break;
				}
				case 10:		// base
				{
					sprintf(Buffer, "Base");
					break;
				}
				case 11:		// EVA
				{
					sprintf(Buffer, "EVA Kerbal");
					break;
				}
				case 12:		// flag
				{
					sprintf(Buffer, "Flag");
					break;
				}
				case 13:		// deployed science controller
				{
					sprintf(Buffer, "Science Controller");
					break;
				}
				case 14:		// deployed science part
				{
					sprintf(Buffer, "Science Part");
					break;
				}
				case 15:		// dropped part
				{
					sprintf(Buffer, "Dropped Part");
					break;
				}
				case 16:		// deployed ground part
				{
					sprintf(Buffer, "Deployed Part");
					break;
				}
				default:		// no idea
				{
					sprintf(Buffer, "????");
					break;
				}
			} 
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			XClearArea(d, w, 0, 0, 1000, 500, True);
			
			break;
		}
		
		case ATMO_CONDITIONS_MESSAGE:
		{
			atmoConditionsMessage *data = (atmoConditionsMessage *)msg->msgData;
			
			kspObject *o;
			char Buffer[40];
			
			o = objects["hasAtmosphere"]; 	(data->hasAtmosphere()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["hasOxygen"]; 	(data->hasOxygen()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["isVesselInAtmosphere"]; 	(data->isVesselInAtmosphere()) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			
			o = objects["airDensity"]; 
			sprintf(Buffer, "%0.2f", data->airDensity);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			o = objects["temperature"]; 
			sprintf(Buffer, "%0.2f", data->temperature);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			o = objects["pressure"]; 
			sprintf(Buffer, "%0.2f", data->pressure);
			o->setText(Buffer);
			o->draw(d, w, double_buffer);
			
			break;
		}
		
		case ACTIONSTATUS_MESSAGE:
		{
			//printf("ACTIONSTATUS_MESSAGE ");
			uint8_t data = msg->msgData[0];
			ActionGroupCurrentStatus = data;	// store this
			
			//printf("\t0x%02X", data);
			
			kspObject *o;
			
			o = objects["stage_action"]; 	(data & STAGE_ACTION) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["gear_action"]; 	(data & GEAR_ACTION) ? o->activate() : o->deactivate();		o->draw(d, w, double_buffer);
			o = objects["light_action"]; 	(data & LIGHT_ACTION) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["rcs_action"]; 		(data & RCS_ACTION) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["sas_action"]; 		(data & SAS_ACTION) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["brakes_action"]; 	(data & BRAKES_ACTION) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			o = objects["abort_action"]; 	(data & ABORT_ACTION) ? o->activate() : o->deactivate();	o->draw(d, w, double_buffer);
			
			XClearArea(d, w, 0, 0, 1000, 500, True);
			
			break;
		}
		
		case SCENE_CHANGE_MESSAGE:
		{
			
			break;
		}
		
		case SOI_MESSAGE:
		{
			char myBuffer[60];
			XSetForeground(d, gc, white_col.pixel);
			XFillRectangle(d, double_buffer, gc, 10, 105, 200, 20);
			
			char SoIBuffer[40] = {0};
			for (uint8_t cntr = 0; cntr < msg->msgSize; cntr++)
			{
				SoIBuffer[cntr] = msg->msgData[cntr];
			}			
	
			kspObject *o = objects["SoI"];
			sprintf(myBuffer, "%s", SoIBuffer);
			o->setText(myBuffer);
			o->draw(d, w, double_buffer);
			
			XClearArea(d, w, 0, 0, 1000, 500, True);
			break;
		}
	}
		
	return;
}

void myHandshakeHandler(KSPMessage *msg)
{
	mySimpit.registerChannel(ALTITUDE_MESSAGE);
	mySimpit.registerChannel(VELOCITY_MESSAGE);
	mySimpit.registerChannel(AIRSPEED_MESSAGE);
	mySimpit.registerChannel(FLIGHT_STATUS_MESSAGE);
	mySimpit.registerChannel(ATMO_CONDITIONS_MESSAGE);
	mySimpit.registerChannel(ACTIONSTATUS_MESSAGE);
	mySimpit.registerChannel(SCENE_CHANGE_MESSAGE);
	mySimpit.registerChannel(SOI_MESSAGE);
	return;
}

// a button has been clicked
void onClick(int ID)
{
	switch (ID)
	{
		// action group buttons
		case GEAR_BUTTON:	
		{
			(ActionGroupCurrentStatus & GEAR_ACTION) ? mySimpit.deactivateAction(GEAR_ACTION) : mySimpit.activateAction(GEAR_ACTION);
			break;
		}
		
		case LIGHT_BUTTON:	
		{
			(ActionGroupCurrentStatus & LIGHT_ACTION) ? mySimpit.deactivateAction(LIGHT_ACTION) : mySimpit.activateAction(LIGHT_ACTION);
			break;
		}
		
		case RCS_BUTTON:	
		{
			(ActionGroupCurrentStatus & RCS_ACTION) ? mySimpit.deactivateAction(RCS_ACTION) : mySimpit.activateAction(RCS_ACTION);
			break;
		}
		
		case SAS_BUTTON:	
		{
			(ActionGroupCurrentStatus & SAS_ACTION) ? mySimpit.deactivateAction(SAS_ACTION) : mySimpit.activateAction(SAS_ACTION);
			break;
		}
		
		case BRAKE_BUTTON:	
		{
			(ActionGroupCurrentStatus & BRAKES_ACTION) ? mySimpit.deactivateAction(BRAKES_ACTION) : mySimpit.activateAction(BRAKES_ACTION);
			break;
		}
	}
	
	return;
}
