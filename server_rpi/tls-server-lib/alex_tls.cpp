/* --- Network ------------------------------------------------------ */

/*

	Alex Network Routines

	*/

void sendNetworkData(const char *data, int len)
{
	// Send only if network is active
	if(networkActive)
	{
        // Use this to store the number of bytes actually written to the TLS connection.
        int c;

		printf("WRITING TO CLIENT\n");
 
        if(tls_conn != NULL) {
            /* TODO: Implement SSL write here to write data to the network. Note that
              handleNetworkData should already have set tls_conn to point to the TLS
              connection we want to write to. */
            c = sslWrite(tls_conn, data, len);
            if(c < 0) {
                perror("Error writing to network: " );
            }
            /* END TODO */
        }

		// Network is still active if we can write more then 0 bytes.
		networkActive = (c > 0);
	}
}


void handleNetworkData(void *conn, const char *buffer, int len)
{
    /* Note: A problem with our design is that we actually get data to be written
        to the SSL network from the serial port. I.e. we send a command to the Arduino,
        get back a status, then write to the TLS connection.  So we do a hack:
        we assume that whatever we get back from the Arduino is meant for the most
        recent client, so we just simply store conn, which contains the TLS
        connection, in a global variable called tls_conn */

        tls_conn = conn; // This is used by sendNetworkData

  printf("received network data, %.*s\n",len,buffer);
	if(buffer[0] == NET_COMMAND_PACKET)
		handleCommand(conn, buffer);
}

void *worker(void *conn)
{
	int len;

	char buffer[BUF_LEN];
	
  printf("Worker thread started\n");
  networkActive = 1;
	while(networkActive)
	{
    printf("Doing SSL Read\n");
		/* TODO: Implement SSL read into buffer */
    len = sslRead(conn, buffer, sizeof(buffer));
    
		/* END TODO */
		// As long as we are getting data, network is active
		networkActive=(len > 0);
    printf("sslReaded?\n");
		if(len > 0){
      printf("received network data\n");
			handleNetworkData(conn, buffer, len);
		}else{
			if(len < 0)
				perror("ERROR READING NETWORK: ");
    }
	}

  printf("exiting\n");
    // Reset tls_conn to NULL.
    tls_conn = NULL;
    EXIT_THREAD(conn);
    endSerial();
}

