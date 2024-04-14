#include "make_tls_server.h"
#include "tls_common_lib.h"
#include "netconstants.h"
#include "constants.h"
#include "packet.h"
#include "serial.h"
#include "serialize.h"

/* TODO: Set PORT_NAME to the port name of your Arduino */
#define PORT_NAME			"/dev/ttyS0"
/* END TODO */

// Check and ensure that this baud rate is the same
// as what is used in Alex.ino
#define BAUD_RATE			B9600

// TLS Port Number
#define SERVER_PORT			5001

/* TODO: #define constants for the  filenames for Alex's private key, certificate, CA certificate name,
        and the Common Name for your laptop */


#define PORTNUM 5001
#define KEY_FNAME   "keys/alex.key"
#define CERT_FNAME  "keys/alex.crt"
#define CA_CERT_FNAME   "keys/signing.pem"
#define CLIENT_NAME     "laptop.cg2111a.we.ball.com"
/* END TODO */

// Our network buffer consists of 1 byte of packet type, and 128 bytes of data
#define BUF_LEN				129

// This variable shows whether a network connection is active
// We will also use this variable to prevent the server from serving
// more than one connection, to keep connection management simple.

static volatile int networkActive;

// This variable is used by sendNetworkData to send back responses
// to the TLS connection.  It is sent by handleNetworkData

static void *tls_conn = NULL;

#include "alex_uart.cpp"




void handleCommand(void *conn, const char *buffer)
{
	// The first byte contains the command
	char cmd = buffer[1];
	uint32_t cmdParam[2];

	// Copy over the parameters.
	memcpy(cmdParam, &buffer[2], sizeof(cmdParam));

	TPacket commandPacket;

	commandPacket.packetType = PACKET_TYPE_COMMAND;
	commandPacket.params[0] = cmdParam[0];
	commandPacket.params[1] = cmdParam[1];

	printf("COMMAND RECEIVED: %c %d %d\n", cmd, cmdParam[0], cmdParam[1]);
	
	switch(cmd)
	{
		case 'f':
		case 'F':
			commandPacket.command = COMMAND_FORWARD;
			uartSendPacket(&commandPacket);
			break;

		case 'b':
		case 'B':
			commandPacket.command = COMMAND_REVERSE;
			uartSendPacket(&commandPacket);
			break;

		case 'l':
		case 'L':
			commandPacket.command = COMMAND_TURN_LEFT;
			uartSendPacket(&commandPacket);
			break;

		case 'r':
		case 'R':
			commandPacket.command = COMMAND_TURN_RIGHT;
			uartSendPacket(&commandPacket);
			break;

		case 's':
		case 'S':
			commandPacket.command = COMMAND_STOP;
			uartSendPacket(&commandPacket);
			break;

		case 'c':
		case 'C':
			commandPacket.command = COMMAND_CLEAR_STATS;
			commandPacket.params[0] = 0;
			uartSendPacket(&commandPacket);
			break;

		case 'g':
		case 'G':
			commandPacket.command = COMMAND_GET_STATS;
			uartSendPacket(&commandPacket);
			break;
    
    // Kill Command
    case 'k':
      system("raspi-gpio set 23 op");
      system("raspi-gpio set 23 dh");
      break;
    case 'K':
      system("raspi-gpio set 23 op");
      system("raspi-gpio set 23 dl");
			printf("Unreset Arduino\n");
      endSerial();
      sleep(5);
      // restart Serial
      serialInit();
			printf("DONE. Sending HELLO to Arduino\n");
    	sendHello();
    	printf("DONE.\n");
      break;
		default:
			printf("Bad command\n");

	}
}

#include "alex_tls.cpp"



int main()
{
	// Start the uartReceiveThread. The network thread is started by
    // createServer

	pthread_t serThread;

	printf("\nALEX REMOTE SUBSYSTEM\n\n");

	printf("Opening Serial Port\n");
	// Open the serial port
	//startSerial(PORT_NAME, BAUD_RATE, 8, 'N', 1, 5);
  serialInit();
	printf("Done. Waiting 3 seconds for Arduino to reboot\n");
	sleep(3);

	printf("DONE. Starting Serial Listener\n");
	pthread_create(&serThread, NULL, uartReceiveThread, NULL);

    printf("Starting Alex Server\n");

    networkActive = 1;

    /* TODO: Call createServer with the necessary parameters to do client authentication and to send
        Alex's certificate. Use the #define names you defined earlier  */

    createServer(KEY_FNAME, CERT_FNAME, PORTNUM, &worker, CA_CERT_FNAME, CLIENT_NAME, 1);
    /* TODO END */

	printf("DONE. Sending HELLO to Arduino\n");
	sendHello();
	printf("DONE.\n");


    // Loop while the server is active
    while(server_is_running());
}
