#include "kspSimpit.h"

uint8_t bitRead(uint8_t x, uint8_t n)
{
	return (x & (1 << n)) ? 1 : 0;
}

KSPSimpit::KSPSimpit(void)
{
	// constructor, set up any memory etc we need
	return;
}

KSPSimpit::~KSPSimpit(void)
{
	// destrouctor, clean up behind us
	
	close(_fd);
	
	return;
}

uint8_t KSPSimpit::begin(void)
{
	// initialisation routine
	
	//printf("Pi 4B KSP Comms Test\n");
	_fd = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NONBLOCK);
	
	if (_fd == -1)
	{
		printf("unable to open serial port\n\n");
		return 1;
	}
	
	struct termios options;
	tcgetattr(_fd, &options);
	
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
		
	// output options
	options.c_cflag = B115200 | CLOCAL | CREAD;		//<Set baud rate
	// 8N1
	options.c_cflag &= ~PARENB;		// disable partity bit
	options.c_cflag &= ~CSTOPB;		// disable 2 stop bits so we only have 1
	options.c_cflag &= ~CSIZE;		// clear bit mask
	options.c_cflag |= CS8;			// set bit mask for 8 data bits
	
	options.c_iflag = IGNPAR;
	options.c_oflag &= ~OPOST;		// select raw output //= 0;
	options.c_cflag &= ~CRTSCTS;	// disable hardware flow control
	
	options.c_cc[VTIME] = 0;		// no inter character timer
	options.c_cc[VMIN] = 1;			// return after each character
	
	// input options
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON); 	
	
	tcflush(_fd, TCIFLUSH);
	tcsetattr(_fd, TCSANOW, &options);
	
	return 0;
}

void KSPSimpit::handshake(void) 
{
	// send the handshake, seperate function in case we want to do it again
	uint8_t _outboundBuffer[40] = {0};
	
	_outboundBuffer[0] = 0x00;
	unsigned int i;
	for (i=0; i<sizeof(KERBALSIMPIT_VERSION); i++)
	{
		_outboundBuffer[i+1] = KERBALSIMPIT_VERSION[i];
	}
	i += 1;
	
	_send(_fd, 0x00, _outboundBuffer, i);
	return;
}

void KSPSimpit::registerChannel(uint8_t channelID)
{
	_send(_fd, REGISTER_MESSAGE, &channelID, 1);
	return;
}

void KSPSimpit::loop(void)
{
	// loop routine
	
	// here we check to see whats come back on the serial port, if anything
	// first, we need to see if there are characters to read, and if so, one by one, read them
	unsigned char myChar = 0x00;
	int R = read(_fd, &myChar, 1);
	if (R == 1)
	{
		// we have read a character!
		//printf("\n%c\t0x%02X", myChar, myChar);
		
		switch(_currentState)
		{
			case 0:			// looking for 0xAA, the start of the header
			{
				if (myChar == 0xAA)
				{
					_currentState = 1;	// move on, otherwise stay where we are
					//printf("\tH1");
				}
				break;
			}
			
			case 1:			// look of 0x50, the end of the header
			{
				if (myChar == 0x50)
				{
					_currentState = 2;	// move on
					//printf("\tH2");
				}
				else
				{
					_currentState = 0;	// go back to looking for a valid header
				}
				break;
			}
			
			case 2:			// get message size
			{
				myMsg.msgSize = myChar;
				_byteCntr = 0;
				_currentState = 3;
				//printf("\tsize (%d bytes)", myMsg.msgSize);
				break;
			}
			
			case 3:			// get message type
			{
				myMsg.msgType = myChar;
				_currentState = 4;
				//printf("\ttype (%d)", myMsg.msgType);
				break;
			}
			
			case 4:			// the message data, we need to read the appropriate number of bytes
			{
				if (_byteCntr < myMsg.msgSize)
				{
					//printf("\t%d", _byteCntr);
					
					// read into the byte array
					myMsg.msgData[_byteCntr] = myChar;
					
					//printf("\t(0x%02X)", myMsg.msgData[_byteCntr]);
					
					_byteCntr++;
				}
				
				if (_byteCntr == myMsg.msgSize)
				{
					// we have loaded the message!
					
					//printf("\nloop: Message!");
					_myHandler();
					
					_currentState = 0;
				}
				
				break;
			}
		}
	}
	
	
	return;
}

void KSPSimpit::activateAction(uint8_t Action)
{
	_send(_fd, AGACTIVATE_MESSAGE, &Action, 1);
	return;
}
		
void KSPSimpit::deactivateAction(uint8_t Action)
{
	_send(_fd, AGDEACTIVATE_MESSAGE, &Action, 1);
	return;
}

void KSPSimpit::_send(int fd, uint8_t msgType, uint8_t msg[], uint8_t msgSize)
{
	uint8_t _hdr[4] = {0xAA, 0x50, 0x00, 0x00};
	_hdr[2] = msgSize;
	_hdr[3] = msgType;
	
	write(fd, _hdr, 4);
	write(fd, msg, msgSize);
	
	return;
}

void KSPSimpit::_myHandler(void)
{
	// check to see if this is a handshake message, type 0, first data byte = 1
	if ((myMsg.msgType == 0) & (myMsg.msgData[0] == 1))
	{
		//printf("\n_myHandler: Handshake!");
		//_printMessage();
		
		myMsg.msgData[0] = 0x02;	// respond
		_send(_fd, myMsg.msgType, myMsg.msgData, myMsg.msgSize);
		
		isHandshake = true;
		
		if (onHandshake != NULL)
		{
			(*onHandshake)(&myMsg);
		}
	}
	
	//printf("\n_myHandler: message!");
	fflush(stdout);
	if (onMessage != NULL)
	{
		//printf(" *");
		_printMessage();
		(*onMessage)(&myMsg);
	}
	
	return;
}

void KSPSimpit::_printMessage(void)
{
	printf("\nmsgType: %d\t msgSize: %d\tmsgData: ", myMsg.msgType, myMsg.msgSize);
	for (uint8_t cntr = 0; cntr < myMsg.msgSize; cntr++)
	{
		printf("%02X ", myMsg.msgData[cntr]);
	}
	fflush(stdout);
	
	return;
}
