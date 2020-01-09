
/***** Ricezione di un comando dal client *****/

int receiveCommand( int socketID, fd_set* masterSet ){

	char* commandReceived;
	uint16_t commandLength_network;
	int commandLength_host;
	int error;

	memset( &MyStatus.UserCommand, 0, sizeof(MyStatus.UserCommand) );

	// Ricezione della dimensione della struttura rappresentante il comando utente
	error = recv( socketID, &commandLength_network, sizeof(uint16_t), MSG_WAITALL );  
		
	// Client si è disconnesso senza usare !quit oppure errore nella recv()
	if ( error == DROPPED_CONNECTION || error == SOCKET_SYSCALL_ERROR ){	 
		 releaseClient( socketID, masterSet, error );
		 return FALSE;
	}

	commandLength_host = ntohs(commandLength_network);
	commandReceived = malloc(commandLength_host + 1);   // Allocazione spazio per la ricezione della struttura
	
	// Ricezione della struttura rappresentante il comando utente
	error = recv( socketID, commandReceived, commandLength_host, MSG_WAITALL ); 
	
	// Client si è disconnesso senza usare !quit oppure errore nella recv()
	if ( error == DROPPED_CONNECTION || error == SOCKET_SYSCALL_ERROR ){	 
		 releaseClient( socketID, masterSet, error );
		 free(commandReceived);
		 return FALSE;
	}
	
	*(commandReceived+commandLength_host) = '\0';

	// Conversione della struttura. Il quarto formato serve a gestire username con spazi all'interno
	sscanf( commandReceived, "%s %s %d %[^0]s", MyStatus.UserCommand.textCommand, MyStatus.UserCommand.listenIP, 
										        &MyStatus.UserCommand.listenPort, MyStatus.UserCommand.username ); 
	
	free(commandReceived);

	return TRUE;

}
