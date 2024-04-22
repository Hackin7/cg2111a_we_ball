
// Routines to create a TLS client
#include "make_tls_client.h"

// Network packet types
#include "netconstants.h"

// Packet types, error codes, etc.
#include "constants.h"

// Tells us that the network is running.
static volatile int networkActive=0;

// DEBUG Prints
//static volatile int debug_print_tls = false;
#include<chrono> 
#include<thread>


void handleError(const char *buffer)
{
	switch(buffer[1])
	{
		case RESP_OK:
			printf("Command / Status OK\n");
			break;

		case RESP_BAD_PACKET:
			printf("BAD MAGIC NUMBER FROM ARDUINO\n");
			break;

		case RESP_BAD_CHECKSUM:
			printf("BAD CHECKSUM FROM ARDUINO\n");
			break;

		case RESP_BAD_COMMAND:
			printf("PI SENT BAD COMMAND TO ARDUINO\n");
			break;

		case RESP_BAD_RESPONSE:
			printf("PI GOT BAD RESPONSE FROM ARDUINO\n");
			break;

		default:
			printf("PI IS CONFUSED!\n");
	}
}

void handleStatus(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));

	printf("\n ------- ALEX STATUS REPORT ------- \n\n");
	printf("Left Forward Ticks:\t\t%d\n", data[0]);
	printf("Right Forward Ticks:\t\t%d\n", data[1]);
	printf("Left Reverse Ticks:\t\t%d\n", data[2]);
	printf("Right Reverse Ticks:\t\t%d\n", data[3]);
	printf("Left Forward Ticks Turns:\t%d\n", data[4]);
	printf("Right Forward Ticks Turns:\t%d\n", data[5]);
	printf("Left Reverse Ticks Turns:\t%d\n", data[6]);
	printf("Right Reverse Ticks Turns:\t%d\n", data[7]);
	printf("Forward Distance:\t\t%d\n", data[8]);
	printf("Reverse Distance:\t\t%d\n", data[9]);
	printf("---------------------------------------\n\n");
	printf("Red Frequency:\t\t%d\n", data[10]);
	printf("Green Frequency:\t%d\n", data[11]);
	printf("Blue Frequency:\t\t%d\n", data[12]);
	printf("Ultrasonic:\t\t%d %f\n", data[13], (double)data[13] * 0.034/2);
	printf("\n---------------------------------------\n\n");
}

void handleMessage(const char *buffer)
{
	printf("MESSAGE FROM ALEX: %s\n", &buffer[1]);
}

void handleCommand(const char *buffer)
{
	// We don't do anything because we issue commands
	// but we don't get them. Put this here
	// for future expansion
}

void handleNetwork(const char *buffer, int len)
{
	// The first byte is the packet type
	int type = buffer[0];

	switch(type)
	{
		case NET_ERROR_PACKET:
		handleError(buffer);
		break;

		case NET_STATUS_PACKET:
		handleStatus(buffer);
		break;

		case NET_MESSAGE_PACKET:
		handleMessage(buffer);
		break;

		case NET_COMMAND_PACKET:
		handleCommand(buffer);
		break;
	}
}

void sendData(void *conn, const char *buffer, int len)
{
	int c;
	printf("sendData() - SENDING %d BYTES DATA\n", len);
	int buflen = sizeof(buffer);
	if(networkActive)
	{
		/* TODO: Insert SSL write here to write buffer to network */
		c = sslWrite(conn, buffer, sizeof(buffer));
        printf("sendData() - read %d bytes from server.\n", c);
		//printf("sendDataReceive: %s\n", buffer); 
		/* END TODO */	
		networkActive = (c > 0);
	}
}

void *readerThread(void *conn)
{
	char buffer[128];
	int len;

	while(networkActive)
	{
		/* TODO: Insert SSL read here into buffer */
		len = sslRead(conn, buffer, sizeof(buffer));

        if(len < 0) {
            perror("readerThread() - Error reading socket: ");
        }

        if(len > 0) {
            printf("readerThread() - Received: \"%s\"\n", buffer);
        }
		/* END TODO */

		networkActive = ( len > 0);

		if(networkActive)
			handleNetwork(buffer, len);
	}

	printf("readerThread() - Exiting network listener thread\n");
    
    /* TODO: Stop the client loop and call EXIT_THREAD */
    stopClient();
    EXIT_THREAD(conn);
    /* END TODO */

    return NULL;
}

void flushInput()
{
	char c;

	while((c = getchar()) != '\n' && c != EOF);
}

void getParams(int32_t *params)
{
	printf("Enter distance/angle in cm/degrees (e.g. 50) and power in %% (e.g. 75) separated by space.\n");
	printf("E.g. 50 75 means go at 50 cm at 75%% power for forward/backward, or 50 degrees left or right turn at 75%%  power\n");
	scanf("%d %d", &params[0], &params[1]);
	flushInput();
}


/* --- Keyboard Code --------------------------------------------------------------------- */
#include <iostream>
#include <termios.h>
#include <unistd.h>
using namespace std;

int getCommand(void *conn);

void move(void *conn, char ch, int distance, int speed){
	char buffer[10];
	int32_t params[2];

	//getParams(params);
	params[0] = distance;
	params[1] = speed;

	buffer[0] = NET_COMMAND_PACKET;
	buffer[1] = ch;
	memcpy(&buffer[2], params, sizeof(params));
	sendData(conn, buffer, sizeof(buffer));
}

void move(void *conn, char ch){
	move(conn, ch, 0, 0);
}
/* --- Angle Code ------------------------------------------------------------------------ */
int angleConversion(int speed, int angle){
	// batt level around 8.8V
	float rateOfRotation = 0; // time / 180degrees
	if (speed >= 100){
		rateOfRotation = 3500 / 360; // 3.5 seconds
	} else if (speed >= 90){
		rateOfRotation = 3500 / 360; // 3.5 seconds
	} else if (speed >= 80){
		rateOfRotation = 4000 / 360; // 4 seconds
	} else if (speed >= 70){
		rateOfRotation = 5000 / 360; // 5 seconds
	} else if (speed >= 60){
		rateOfRotation = 6000 / 360; // 6 seconds
	} else if (speed >= 0){
		rateOfRotation = 20000 / 360; // 20 seconds
	}
	int output = rateOfRotation * angle;
	printf("Distance Calculated: %f * %d = %d\n", rateOfRotation, angle, output);
	return output;
}
void approxAngle(void *conn, char ch){
	int32_t params[2];
	int32_t angle=0;
	int32_t speed=0;
	printf("Enter approx angle + speed\n");
	scanf("%d %d", &angle, &speed);
	flushInput();

	move(conn, ch, angleConversion(speed, angle)+1000, speed); // + 1000 to use timer logic
}
void distanceMove(void *conn, char ch){
	int32_t params[2];
	int32_t dist=0;
	int32_t speed=0;
	printf("Enter approx dist + speed\n");
	scanf("%d %d", &dist, &speed);
	move(conn, ch, dist, speed); // + 1000 to use timer logic
	flushInput();
}


void printCommands(){
	printf("\n\n");
	printf("### Commands ##################################################\n");
	printf("wasd, q=stop\n");
	printf("p=changeparams, c=command)\n");
	printf("k=kill, K=unkill, l=clearSerializeBuffer\n");
	printf("x=exit\n");
	printf("---------------------------------------------------------------\n");
}
void printState(int speeds[4]){
	printCommands();
	printf("Current Speeds: f=%d b=%d l=%d r=%d\n", speeds[0], speeds[1], speeds[2], speeds[3]);
	printf("###############################################################\n");
}
void *keyboardControlThread(void *conn) {
	// Get terminal settings
    termios oldTermios, newTermios;
    tcgetattr(STDIN_FILENO, &oldTermios);

    // Set terminal to raw mode (no buffering)
    newTermios = oldTermios;
    newTermios.c_lflag &= ~ICANON;
    newTermios.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

	int speeds[4] = {40, 40, 50, 50};
	bool exit=false;
	char ch;

	printState(speeds);
    while (!exit) {
        ch = getchar();
		printf("---------------------------------------------------------------\n");
		printf("keyboardControlThread() - char = '%c'\n", ch);
        switch (ch) {
			case '\n':
				printState(speeds);
				break;
			// Movement ////////////////////////////////////////////////
            case 'w':
                move(conn, 'f', 0, speeds[0]);
				break;
            case 'a':
                move(conn, 'l', 0, speeds[2]);
				break;

            case 's':
                move(conn, 'b', 0, speeds[1]);
				break;

            case 'd':
                move(conn, 'r', 0, speeds[3]);
				break;
            case 'x':
                // Exit the program
                cout << "Exiting..." << endl;
				exit=true;
                break;
			// timed movement ///////////////////////////////////////////////////
			case 'W':
				tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);// restore terminal settings
				distanceMove(conn, 'f');
    			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
				break;
			case 'S':
				tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);// restore terminal settings
				distanceMove(conn, 'b');
    			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
				break;
			case 'A':
				tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);// restore terminal settings
				approxAngle(conn, 'l');
    			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
				break;
			case 'D':
				tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);// restore terminal settings
				approxAngle(conn, 'r');
    			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
				break;
			// Parameters ///////////////////////////////////////////////////
            case 'c':
				tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);// restore terminal settings
				getCommand(conn);
    			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
                break;
            case 'p': // Change Params
				tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);// restore terminal settings
				printf("Key in new speeds f_b_l_r:\n");
				printf("Current Speeds: f=%d b=%d l=%d r=%d\n", speeds[0], speeds[1], speeds[2], speeds[3]);
				scanf("%d %d %d %d", &speeds[0], &speeds[1], &speeds[2], &speeds[3]);
				flushInput();
    			tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
                break;
			// Kills ///////////////////////////////////////////////////////
			case 'k':
			case 'K':
				move(conn, ch);
				break;
			case 'l':
                move(conn, 'a');
				break;
			// Get commands ///////////////////////////////////////////////
			case 'g':
			case 'G':
                move(conn, 'g');
				break;
            default:
                move(conn, 's');
				break;
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

	// Restore terminal settings on exit
	tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
	/* Stop the client loop and call EXIT_THREAD */
    //stopClient();
    EXIT_THREAD(conn);
    return NULL;
}

/* --------------------------------------------------------------------------------------- */

int getCommand(void *conn){
	char ch;
	printf("Command (f=forward, b=reverse, l=turn left, r=turn right, s=stop, c=clear stats, g=get stats q=exit)\n");
	scanf("%c", &ch);

	// Purge extraneous characters from input stream
	flushInput();

	char buffer[10];
	int32_t params[2];

	buffer[0] = NET_COMMAND_PACKET;
	switch(ch)
	{
		case 'f':
		case 'F':
		case 'b':
		case 'B':
		case 'l':
		case 'L':
		case 'r':
		case 'R':
			getParams(params);
			buffer[1] = ch;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		case 's':
		case 'S':
		case 'c':
		case 'C':
		case 'g':
		case 'G':
			params[0]=0;
			params[1]=0;
			memcpy(&buffer[2], params, sizeof(params));
			buffer[1] = ch;
			sendData(conn, buffer, sizeof(buffer));
			break;
		case 'q':
		case 'Q':
			//quit=1;
			return 1;
			break;
		default:
			printf("getCommand() - BAD COMMAND\n");
	}
	return 0;
}

// Old Code
void *writerThread(void *conn)
{
	int quit=0;

	while(!quit)
	{
		quit = getCommand(conn);
	}

	printf("Exiting keyboard thread\n");

    /* TODO: Stop the client loop and call EXIT_THREAD */
    stopClient();
    EXIT_THREAD(conn);
    /* END TODO */

    return NULL;
}

/* TODO: #define filenames for the client private key, certificatea,
   CA filename, etc. that you need to create a client */
#define SERVER_NAME "192.168.255.99"
#define CA_CERT_FNAME "keys/signing.pem"
#define PORT_NUM 5001
#define CLIENT_CERT_FNAME "keys/laptop.crt"
#define CLIENT_KEY_FNAME "keys/laptop.key"
#define SERVER_NAME_ON_CERT "alex.cg2111a.we.ball.com"
/* END TODO */

void connectToServer(const char *serverName, int portNum)
{
    /* TODO: Create a new client */
	//createClient(serverName, portNum, 1, CA_CERT_FNAME, SERVER_NAME_ON_CERT, 1, CLIENT_CERT_FNAME, CLIENT_KEY_FNAME, readerThread, writerThread);
	createClient(serverName, portNum, 1, CA_CERT_FNAME, SERVER_NAME_ON_CERT, 1, CLIENT_CERT_FNAME, CLIENT_KEY_FNAME, readerThread, keyboardControlThread);
    /* END TODO */
}

int main(int ac, char **av)
{
	if(ac != 3)
	{
		fprintf(stderr, "\n\n%s <IP address> <Port Number>\n\n", av[0]);
		exit(-1);
	}

    networkActive = 1;
    connectToServer(av[1], atoi(av[2]));

    /* TODO: Add in while loop to prevent main from exiting while the
    client loop is running */
    while(client_is_running());

    /* END TODO */
	printf("\nMAIN exiting\n\n");
}
