/*
 * Alex's setup and run codes
 * 
 */

void handleCommand(TPacket *command)
{
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        forward((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_REVERSE:
        backward((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_TURN_LEFT:
        left((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_TURN_RIGHT:
	    right((float) command->params[0], (float) command->params[1]);
        sendOK();
      break;
    case COMMAND_STOP:
        stop();
        sendOK();
      break;
    case COMMAND_GET_STATS:
        sendStatus();
        sendOK();
      break;
    case COMMAND_CLEAR_STATS:
        clearOneCounter(command->params[0]);
        sendOK();
      break;
    
    /*
     * Implement code for other commands here.
     * 
     */
        
    default:
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
