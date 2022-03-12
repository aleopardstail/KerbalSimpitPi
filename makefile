CC=g++
CFLAGS=-Wall
LIBS=-L/usr/X11R6/lib -lX11

ksptest: ksptest.o kspSimpit.o kspObject.o
	$(CC) $(CFLAGS) ksptest.o kspSimpit.o kspObject.o  $(LIBS) -o ksptest

ksptest.o:  ksptest.cpp kspSimpit.h kspSimpit.cpp kspObject.h kspObject.cpp
	$(CC) $(CFLAGS) -c ksptest.cpp -o ksptest.o
	
kspSimpit.o: kspSimpit.cpp kspSimpit.h PayloadStructs.h KerbalSimpitMessageTypes.h
	$(CC) $(CFLAGS) -c kspSimpit.cpp -o kspSimpit.o
	
kspObject.o: kspObject.cpp kspObject.h
	$(CC) $(CFLAGS) -c kspObject.cpp -o kspObject.o
