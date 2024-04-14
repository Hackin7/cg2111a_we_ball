
/*

	Alex Serial Routines to the Arduino

	*/


// Consolidate settings here


/* --- Serial ------------------------------------------------------ */

// Prototype for sendNetworkData
void sendNetworkData(const char *, int);


// For Other Handling
void sendMessage(const char *buf, int len){
	char data[60];
  printf("Send Message from RPi: %s\n", buf);
	data[0] = NET_MESSAGE_PACKET;
	memcpy(&data[1], buf, len); // I am a secure coder
  data[len] = '\0'; // I am very secure trust me
	sendNetworkData(data, sizeof(data));
}

//void sendMessage(const char *buf){sendMessage(buf, sizeof(buf));}


void handleErrorResponse(TPacket *packet)
{
	printf("UART ERROR: %d\n", packet->command);
	char buffer[2];
	buffer[0] = NET_ERROR_PACKET;
	buffer[1] = packet->command;
	sendNetworkData(buffer, sizeof(buffer));
}

void handleMessage(TPacket *packet)
{
	char data[60];
	printf("UART MESSAGE PACKET: %s\n", packet->data);
	data[0] = NET_MESSAGE_PACKET;
	memcpy(&data[1], packet->data, sizeof(packet->data));
  data[sizeof(packet->data)] = '\0';
	sendNetworkData(data, sizeof(data));
}

void handleStatus(TPacket *packet)
{
	char data[65];
	printf("UART STATUS PACKET\n");
	data[0] = NET_STATUS_PACKET;
	memcpy(&data[1], packet->params, sizeof(packet->params));
	sendNetworkData(data, sizeof(data));
}

void handleResponse(TPacket *packet)
{
	// The response code is stored in command
	switch(packet->command)
	{
		case RESP_OK:
			char resp[2];
			printf("Command OK\n");
			resp[0] = NET_ERROR_PACKET;
			resp[1] = RESP_OK;
			sendNetworkData(resp, sizeof(resp));
		  break;

		case RESP_STATUS:
			handleStatus(packet);
		  break;

		default:
		  printf("Boo\n");
	}
}


void handleUARTPacket(TPacket *packet)
{
	switch(packet->packetType)
	{
		case PACKET_TYPE_COMMAND:
				// Only we send command packets, so ignore
			break;

		case PACKET_TYPE_RESPONSE:
				handleResponse(packet);
			break;

		case PACKET_TYPE_ERROR:
				handleErrorResponse(packet);
			break;

		case PACKET_TYPE_MESSAGE:
				handleMessage(packet);
			break;
	}
}


void uartSendPacket(TPacket *packet)
{
	char buffer[PACKET_SIZE];
	int len = serialize(buffer, packet, sizeof(TPacket));

	serialWrite(buffer, len);
}

void handleError(TResult error)
{
	switch(error)
	{
		case PACKET_BAD:
			printf("ERROR: Bad Magic Number\n");
      sendMessage("ERROR: Bad Magic Number", 24);
			break;

		case PACKET_CHECKSUM_BAD:
			printf("ERROR: Bad checksum\n");
      sendMessage("ERROR: Bad checksum", 20);
			break;

		default:
			printf("ERROR: UNKNOWN ERROR\n");
      sendMessage("ERROR: UNKNOWN ERROR", 21);
	}
}

void *uartReceiveThread(void *p)
{
	char buffer[PACKET_SIZE];
	int len;
	TPacket packet;
	TResult result;
	int counter=0;

	while(1)
	{
		len = serialRead(buffer);
		counter+=len;
		if(len > 0)
		{
			result = deserialize(buffer, len, &packet);

			if(result == PACKET_OK)
			{
				counter=0;
				handleUARTPacket(&packet);
			}
			else 
				if(result != PACKET_INCOMPLETE)
				{
					printf("PACKET ERROR\n");
					handleError(result);
				} // result
		} // len > 0
	} // while
}

void serialInit(){
  startSerial(PORT_NAME, BAUD_RATE, 8, 'N', 1, 5);
}

void sendHello()
{
	// Send a hello packet
	TPacket helloPacket;

	helloPacket.packetType = PACKET_TYPE_HELLO;
	uartSendPacket(&helloPacket);
}
