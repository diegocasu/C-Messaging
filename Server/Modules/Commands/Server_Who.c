
/***** Calcolo della dimensione della stringa con elenco degli utenti *****/

int userListStringSize(){

	int i;	
	int length = 0;

	if ( MyStatus.nextRecord == 0 ){
		 length = strlen("Non sono presenti utenti registrati\n");	
		 return length;	
	}


	for ( i = 0 ; i < MyStatus.nextRecord ; i++ ){
		  length += strlen(MyStatus.myUsers[i].registeredUsername);

		  if ( MyStatus.myUsers[i].connectionState == ONLINE )
			   length += strlen( " (ONLINE)\n" );
		  else
			   length += strlen( " (OFFLINE)\n" );
	}

	return length;

}



/***** Calcolo della stringa con elenco degli utenti *****/

void createUserListString(char* userList){
	
	int i;

	if ( MyStatus.nextRecord == 0 ){
		 strcat( userList, "Non sono presenti utenti registrati\n");	
		 return;
	}


	for ( i = 0 ; i < MyStatus.nextRecord ; i++ ){
		  strcat( userList, MyStatus.myUsers[i].registeredUsername );
		  		
		  if ( MyStatus.myUsers[i].connectionState == ONLINE )
			   strcat( userList, " (ONLINE)\n" );
		  else
			   strcat( userList, " (OFFLINE)\n" );
	}

}



/***** Comando !who *****/

void commandWho( int socketID, fd_set* masterSet ){

	char clientIP[16];
	int clientPort;

	uint16_t sendLength_network;
	int sendLength_host;
	char* userList;

	int error;	


	searchClientDetails( socketID, clientIP, &clientPort, sizeof(clientIP) );
	printf("Esecuzione del comando %s richiesto dal client %s, porta %d \n", MyStatus.UserCommand.textCommand, 
																		     clientIP, clientPort );

	sendLength_host = userListStringSize();
	userList = malloc(sendLength_host + 1);
	createUserListString(userList);
	
	*(userList+sendLength_host) = '\0';  // Per sicurezza


	// Invio della dimensione
	sendLength_network = htons(sendLength_host);
	error = send( socketID, &sendLength_network, sizeof(uint16_t), MSG_NOSIGNAL );

	if ( error == SOCKET_SYSCALL_ERROR ){	 
		 releaseClient( socketID, masterSet, error );
		 free(userList);
		 return;
	}

	// Invio della stringa	
	error = send( socketID, userList, sendLength_host, MSG_NOSIGNAL );

	if ( error == SOCKET_SYSCALL_ERROR ){	 
		 releaseClient( socketID, masterSet, error );
		 free(userList);
		 return;
	}


	free(userList);
	printf("Invio della lista di utenti registrati effettuata con successo\n");

}
