# KerbalSimpitPi
KerbalSimpit Mod for the Raspberry Pi

very rough code to connect a Raspberry Pi to the Kerbal Space Programme Mod "Kerbal Simpit Revamped". 
this requires a Serial to USB interface connected to the GPIO header for the first serial port

this has been built on a Pi4 1GB and as written is aimed at a 1024x600 touch screen, the interface works fine with a mouse and larger monitor though

several KSP simpit library files are utilised here, though slightly out of date, and slightly modified (the #include <arduino.h> line commented out)

to build type "make", to run type ./ksptest

the interesting stuff is in kspSimpit.h and kpsSimpit.cpp which defines the objects
