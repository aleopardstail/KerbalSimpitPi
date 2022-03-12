# KerbalSimpitPi
KerbalSimpit Mod for the Raspberry Pi

very rough code to connect a Raspberry Pi to the Kerbal Space Programme Mod "Kerbal Simpit Revamped". 
this requires a Serial to USB interface connected to the GPIO header for the first serial port

this has been built on a Pi4 1GB and as written is aimed at a 1024x600 touch screen, the interface works fine with a mouse and larger monitor though

several KSP simpit library files are utilised here, though slightly out of date, and slightly modified (the #include <arduino.h> line commented out)

to build type "make", to run type ./ksptest

the interesting stuff is in kspSimpit.h and kpsSimpit.cpp which defines the objects

ksptest.cpp has a badly commented example of how this works, essentially declare the object, probably global but doesn't have to be

KSPSimpite mySimpit;    // defaine object
mySimpit.begin();       // initialisation routine, sets up the serial port
mySimpit.onMessage = myMessageHandler;      // define callback for when a message arrives
mySimpit.onHandshake = myHandshakeHandler;  // define a callback for when a handshake is completed.

note unlike the Arduino code you do not register channels here, you do that in the handshake handler code

then in your applications primary loop, call mysimpit.loop();, this will process inbound stuff and call the message handlers

void myMessageHandler(KSPMessage *msg)
{
 switch (msg->msgType)
 {
  case ALTITUDE_MESSAGE:
  {....}
 }
 return; 
}

the handshake handler is similar

void myHandshakeHandler(KSPMessage *msg)
{
 mySimpit.registerChannel(ALTITUDE_MESSAGE);
 return;
}

the handshake message also goes to the main loop but a seperate callback is provided for clarity, otherwise fill your boots, there are some utility functions which can be seen in the example code
