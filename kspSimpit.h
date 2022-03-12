/* kspSimpit.h
 * KSP simpit header file for Raspberry Pi
 * 
 */ 
 
#ifndef KSPSIMPIT_H

#define KSPSIMPIT_H



#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART

#define byte uint8_t
uint8_t bitRead(uint8_t x, uint8_t n);

#include "KerbalSimpitMessageTypes.h"		// simpit file
#include "PayloadStructs.h"					// simpit file


const char KERBALSIMPIT_VERSION[] = "1.3.0";

struct KSPMessage
{
	uint8_t msgSize = 0;
	uint8_t msgType = 0;
	uint8_t msgData[40] = {0};
};

class KSPSimpit  
{
	public:
		KSPSimpit(void);		// constructor
		~KSPSimpit(void);		// destructor
		uint8_t begin(void);	// returns 0 on success
		void handshake(void);	// send the handshake message
		void registerChannel(uint8_t channelID);
		
		void loop(void);
		
		void(*onMessage)(KSPMessage *) = NULL;
		void(*onHandshake)(KSPMessage *) = NULL;
		
		void activateAction(uint8_t Action);
		void deactivateAction(uint8_t Action);
		
		
		
		bool isHandshake = false;
	
	private:
		void _send(int fd, uint8_t messageType, uint8_t msg[], uint8_t msgSize);
		void _myHandler(void);	// internal message handler, manages handshake and cast off
		
		int _fd;
		uint8_t _currentState = 0;
		KSPMessage myMsg;
		uint8_t _byteCntr = 0;
		
		void _printMessage(void);
		
};

#endif
