

/***** Gestore errore send() *****/

void checkSendCommandError( int error, char* convertedStruct ){

	if ( error == SOCKET_SYSCALL_ERROR ){
		 if ( errno == EPIPE ){		// Socket di connessione con il server è stato chiuso
			  printf("Connessione con il server non più attiva\n");
			  printf("Chiusura dell'applicazione\n'");
		 }
		 else
			perror("Errore nell'invio del comando al server: ");
		
		closeSockets();
		free(convertedStruct);
		kill( MyStatus.myPid, SIGTERM );
	    exit(-1);	 
	}

}


/***** Funzione per l'invio del comando utente al server *****/

void sendCommand(){

	uint16_t sendLength_network;
	int sendLength_host;
	char* convertedStruct;
	int error;

	
	// Calcolo quanto occupa la struttura una volta convertita in stringa	
	sendLength_host = snprintf( NULL, 0, "%s %s %d %s", MyStatus.UserCommand.textCommand, MyStatus.UserCommand.listenIP, 
											 			MyStatus.UserCommand.listenPort, MyStatus.UserCommand.username ); 
	sendLength_network = htons(sendLength_host);

	// Alloco memoria per contenere struttura convertita in stringa
	convertedStruct = malloc(sendLength_host + 1); 

	// Conversione 
	sprintf( convertedStruct, "%s %s %d %s", MyStatus.UserCommand.textCommand, MyStatus.UserCommand.listenIP, 
											 MyStatus.UserCommand.listenPort, MyStatus.UserCommand.username );
	
	// Invio della dimensione della struttura convertita. MSG_NOSIGNAL evita che SO uccida il processo 
	// quando send() è invocata su socket chiuso dal server: ora send() restituirà -1 e permetterà il test manuale   
	error = send( MyStatus.serverCommunication, &sendLength_network, sizeof(uint16_t), MSG_NOSIGNAL ) ;
	checkSendCommandError( error, convertedStruct );

	// Invio della struttura convertita
	error = send( MyStatus.serverCommunication, convertedStruct, sendLength_host , MSG_NOSIGNAL );
	checkSendCommandError( error, convertedStruct );

	free(convertedStruct);

}
