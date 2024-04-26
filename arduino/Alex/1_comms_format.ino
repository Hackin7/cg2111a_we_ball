/*
 * 
 * Alex Communication Routines.
 * 
 */

void dbprintf(char *format, ...) {
  va_list args;
  char buffer[128];
  va_start(args, format);
  vsprintf(buffer, format, args);
  sendMessage(buffer);
} 

/* --------------------------------------------------------------------------------*/
void sendStatus()
{
  // Implement code to send back a packet containing key
  // information like leftTicks, rightTicks, leftRevs, rightRevs
  // forwardDist and reverseDist
  // Use the params array to store this information, and set the
  // packetType and command files accordingly, then use sendResponse
  // to send out the packet. See sendMessage on how to use sendResponse.
  //

  TPacket statusPacket;
  statusPacket.packetType = PACKET_TYPE_RESPONSE;
  statusPacket.command = RESP_STATUS;
  statusPacket.params[0] = leftForwardTicks;
  statusPacket.params[1] = rightForwardTicks;
  statusPacket.params[2] = leftReverseTicks;
  statusPacket.params[3] = rightReverseTicks;
  statusPacket.params[4] = leftForwardTicksTurns;
  statusPacket.params[5] = rightForwardTicksTurns;
  statusPacket.params[6] = leftReverseTicksTurns;
  statusPacket.params[7] = rightReverseTicksTurns;
  statusPacket.params[8] = forwardDist;
  statusPacket.params[9] = reverseDist;
  // Stuff
  statusPacket.params[10] = redFrequency;
  statusPacket.params[11] = blueFrequency;
  statusPacket.params[12] = greenFrequency;
  statusPacket.params[13] = (uint32_t)ultDuration;
  statusPacket.params[14] = (uint32_t)colourEnum;
  // Packet up till 15
  sendResponse(&statusPacket);

}

void colourStatus()
{
	TPacket colourPacket;
	colourPacket.packetType = PACKET_TYPE_RESPONSE;
	colourPacket.command = RESP_STATUS;
	colourPacket.params[0] = redFrequency;
	colourPacket.params[1] = redColor;
	colourPacket.params[2] = blueFrequency;
	colourPacket.params[3] = blueColor;
	colourPacket.params[4] = greenFrequency;
	colourPacket.params[5] = greenColor;
	// insert dbprintf //
	dbprintf("Red frequency: %d, Red Colour: %d\n", redFrequency, redColor);
	dbprintf("Blue frequency: %d, Blue Colour: %d\n", blueFrequency, blueColor);
	dbprintf("Green frequency: %d, Green Colour: %d\n", greenFrequency, greenColor);
	sendResponse(&colourPacket);
}

void sendMessage(const char *message)
{
  // Sends text messages back to the Pi. Useful
  // for debugging.
  
  TPacket messagePacket;
  messagePacket.packetType=PACKET_TYPE_MESSAGE;
  strncpy(messagePacket.data, message, MAX_STR_LEN);
  sendResponse(&messagePacket);
}

void sendBadPacket()
{
  // Tell the Pi that it sent us a packet with a bad
  // magic number.
  
  TPacket badPacket;
  badPacket.packetType = PACKET_TYPE_ERROR;
  badPacket.command = RESP_BAD_PACKET;
  sendResponse(&badPacket);
  
}

void sendBadChecksum()
{
  // Tell the Pi that it sent us a packet with a bad
  // checksum.
  
  TPacket badChecksum;
  badChecksum.packetType = PACKET_TYPE_ERROR;
  badChecksum.command = RESP_BAD_CHECKSUM;
  sendResponse(&badChecksum);  
}

void sendBadCommand()
{
  // Tell the Pi that we don't understand its
  // command sent to us.
  
  TPacket badCommand;
  badCommand.packetType=PACKET_TYPE_ERROR;
  badCommand.command=RESP_BAD_COMMAND;
  sendResponse(&badCommand);
}

void sendBadResponse()
{
  TPacket badResponse;
  badResponse.packetType = PACKET_TYPE_ERROR;
  badResponse.command = RESP_BAD_RESPONSE;
  sendResponse(&badResponse);
}

void sendOK()
{
  TPacket okPacket;
  okPacket.packetType = PACKET_TYPE_RESPONSE;
  okPacket.command = RESP_OK;
  sendResponse(&okPacket);  
}

void sendResponse(TPacket *packet)
{
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;

  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}

/* --------------------------------------------------------------------------------*/
void handleCommand(TPacket *command)
{
  //lcd.clear();
  lcd.setCursor(9, 0);
  //lcd.print("Command: ");  
  
  //lcd.println("G="+String(greenFrequency)+" ");
  //lcd.setCursor(0, 1);  
  //lcd.print("B="+String(blueFrequency));
  
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        lcd.print("Forward"); 
        forward((float) command->params[0], (float) command->params[1]);
        break;
    case COMMAND_REVERSE:
        sendOK();
        lcd.print("Reverse"); 
        backward((float) command->params[0], (float) command->params[1]);
        break;
    case COMMAND_TURN_LEFT:
        sendOK();
        lcd.print("Left"); 
        left((float) command->params[0], (float) command->params[1]);
        break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        lcd.print("Right"); 
	      right((float) command->params[0], (float) command->params[1]);
        break;
    case COMMAND_STOP:
        sendOK();
        lcd.print("Stop"); 
        stop();
        break;
    case COMMAND_GET_STATS:
        sendOK();
        
        colourSense();
  	    ultrasonicSensor();
        sendStatus();
        
        //sendOK();
        //break;
        //case COMMAND_GET_COLOUR:
  	    //colourStatus();
        
        break;
    case COMMAND_CLEAR_STATS:
        sendOK();
        clearOneCounter(command->params[0]);
        break;
    
    /*
     * Implement code for other commands here.
     * 
     */
        
    default:
        lcd.print("how like tis"); 
        sendBadCommand();
  }
}

void waitForHello()
{
  int exit=0;

  while(!exit)
  {
    TPacket hello;
    TResult result;
    
    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if(result == PACKET_OK)
    {
      if(hello.packetType == PACKET_TYPE_HELLO)
      {
     

        sendOK();
        exit=1;
      }
      else
        sendBadResponse();
    }
    else
      if(result == PACKET_BAD)
      {
        sendBadPacket();
      }
      else
        if(result == PACKET_CHECKSUM_BAD)
          sendBadChecksum();
  } // !exit
}


// Not even running
void handlePacket(TPacket *packet)
{ 
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}
