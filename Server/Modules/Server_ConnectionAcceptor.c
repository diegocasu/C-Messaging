
/***** Accettazione connessione client *****/

void acceptClientConnection( fd_set* masterSet, int* setMax ){
			
	char isAccepted[CONNECTION_MSG_LENGTH + 1];

	int connectionLimitExceeded = MAX_CLIENT_CONNECTED - MyStatus.nextDescriptor;
	int newSocket;
	int error;

	struct sockaddr_in ClientAddressCopy;
	char clientIP[16];
	int clientPort;
	int clientLength = sizeof(ClientAddressCopy);
 


	newSocket = accept( MyStatus.helloSocket, (struct sockaddr*)&ClientAddressCopy, (socklen_t*)&clientLength );	

	if ( newSocket ==  SOCKET_SYSCALL_ERROR ){
		 printf("Connessione rifiutata --- Errore nell'esecuzione di accept()\n" );
		 return;
	}

	clientPort = ntohs(ClientAddressCopy.sin_port);
	inet_ntop( AF_INET, &(ClientAddressCopy.sin_addr), clientIP, sizeof(clientIP) );	

	
	// Numero massimo di connessioni raggiunto	
	
	if ( connectionLimitExceeded == 0 ){
		 strcpy( isAccepted, CONNECTION_ERROR_MSG );
		 error = send( newSocket, isAccepted, CONNECTION_MSG_LENGTH, MSG_NOSIGNAL );
		
		 if ( error == SOCKET_SYSCALL_ERROR )
			  printf("Impossibile eseguire handshake di connessione con il client %s, porta %d\n", clientIP, clientPort );

		 printf("Connessione rifiutata --- Client %s, porta %d. Limite di connessioni raggiunto\n", clientIP, clientPort );
		 close(newSocket);
		 return;
	}

	// Memorizzazione della nuova connessione

	strcpy( isAccepted, CONNECTION_ACK_MSG );
	error = send( newSocket, isAccepted, CONNECTION_MSG_LENGTH, MSG_NOSIGNAL );

	if ( error == SOCKET_SYSCALL_ERROR ){
		 printf("Connessione rifiutata --- Client %s, porta %d. Impossibile eseguire handshake di connessione\n", clientIP, clientPort );
		 close(newSocket);
		 return;
	}
	

	FD_SET( newSocket, masterSet);
	MyStatus.myConnections[MyStatus.nextDescriptor].socketID = newSocket;
	MyStatus.myConnections[MyStatus.nextDescriptor].ClientAddress = ClientAddressCopy;
	MyStatus.nextDescriptor++;

	if ( (*setMax) < newSocket )
		  *setMax = newSocket;

	printf("Connessione eseguita con successo --- Client %s, porta %d \n", clientIP, clientPort );
	
}



