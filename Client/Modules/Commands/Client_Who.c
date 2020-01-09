
/***** Controllo errori *****/


void checkCommandWhoError( int error, char* userList ){

	if ( error == DROPPED_CONNECTION ){	 
		 printf("Connessione con il server non più attiva\n");
		 printf("Chiusura dell'applicazione\n'");
		 closeSockets();
		 if ( userList != NULL )
			  free(userList);

		 kill( MyStatus.myPid, SIGTERM );
		 exit(1);
	}
	
	if ( error == SOCKET_SYSCALL_ERROR ){
		perror("Impossibile eseguire comando: ");
		closeSockets();
		if ( userList != NULL )
			  free(userList);

		kill( MyStatus.myPid, SIGTERM );
		exit(-1);
	}


}



/***** Comando !who *****/

void commandWho(){

	uint16_t userListLength_network;
	int userListLength_host;
	char* userList;
	int error;

	
	// Invio comando
	sendCommand();
	

	// Ricezione della dimensione dell'elenco utenti
	error = recv( MyStatus.serverCommunication, &userListLength_network, sizeof(uint16_t), MSG_WAITALL );
	checkCommandWhoError( error, NULL );

	
	userListLength_host = ntohs(userListLength_network);
	userList = malloc(userListLength_host + 1);	// Allocazione spazio per struttura da ricevere
	

	// Ricezione dell'elenco utenti
	error = recv( MyStatus.serverCommunication, userList , userListLength_host, MSG_WAITALL );
	checkCommandWhoError(error, userList);
	*(userList + userListLength_host) = '\0';

	printf("%s\n",userList);
	
	free(userList);

}
