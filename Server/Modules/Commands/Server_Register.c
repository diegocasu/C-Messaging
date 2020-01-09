
/***** Registrazione utente *****/

void registerNewUser( char* clientIP, int clientPort ){

	MyStatus.myUsers[MyStatus.nextRecord].connectionState = ONLINE;	
	strcpy( MyStatus.myUsers[MyStatus.nextRecord].registeredUsername, MyStatus.UserCommand.username );
		
	strcpy( MyStatus.myUsers[MyStatus.nextRecord].registeredIP, MyStatus.UserCommand.listenIP );
	MyStatus.myUsers[MyStatus.nextRecord].registeredPort = MyStatus.UserCommand.listenPort;
		
	strcpy( MyStatus.myUsers[MyStatus.nextRecord].connectedIP, clientIP );
	MyStatus.myUsers[MyStatus.nextRecord].connectedPort = clientPort;

	MyStatus.myUsers[MyStatus.nextRecord].nextMessage = 0;
	
	MyStatus.nextRecord++;

}



/***** Riconnessione utente *****/

void reconnectUser( int index, char* clientIP, int clientPort ){

	MyStatus.myUsers[index].connectionState = ONLINE;
	
	strcpy( MyStatus.myUsers[index].registeredIP, MyStatus.UserCommand.listenIP );
	MyStatus.myUsers[index].registeredPort = MyStatus.UserCommand.listenPort;

	strcpy( MyStatus.myUsers[index].connectedIP, clientIP );
	MyStatus.myUsers[index].connectedPort = clientPort;

}



/***** Calcolo della lunghezza della stringa di messaggi da inviare al client *****/

int offlineMessageStringSize(int userIndex){

	int i;	
	int length = 0;

	if ( MyStatus.myUsers[userIndex].nextMessage == 0 ){
		 length = strlen("Non hai ricevuto messaggi offline\n\n");	
		 return length;	
	}


	for ( i = 0 ; i < MyStatus.myUsers[userIndex].nextMessage ; i++ ){
		  length += strlen( "\nMessaggio offline inviato da " );
		  length += strlen( MyStatus.myUsers[userIndex].messageQueue[i].sender );
		  length += strlen( ":\n" );
		  length += strlen( MyStatus.myUsers[userIndex].messageQueue[i].message );
		  length += strlen( "\n" );
	}

	return length;

}


/***** Creazione stringa di messaggi ricevuti offline *****/

void createOfflineMessageString( char* userMessages, int userIndex ){

	int i;

	if ( MyStatus.myUsers[userIndex].nextMessage == 0 ){
		 strcat( userMessages, "Non hai ricevuto messaggi offline\n\n");	
		 return;
	}


	for ( i = 0 ; i < MyStatus.myUsers[userIndex].nextMessage ; i++ ){
		  strcat( userMessages, "\nMessaggio offline inviato da " );
		  strcat( userMessages, MyStatus.myUsers[userIndex].messageQueue[i].sender );
		  strcat( userMessages, ":\n" );
		  strcat( userMessages, MyStatus.myUsers[userIndex].messageQueue[i].message );
		  strcat( userMessages, "\n" );
	}

}




/****************          *****************
***************** COMANDO  *****************
*****************		   ****************/


void commandRegister( int socketID, fd_set* masterSet ){

	char replyMessage[REGISTER_MSG_LENGTH + 1];
	char clientIP[16];
	int clientPort;
	int userFound = FALSE;

	char* userMessages;
	int  messagesLength_host;
	uint16_t messagesLength_network;	

	int i;
	int error;


	searchClientDetails( socketID, clientIP, &clientPort, sizeof(clientIP) );
	printf("Esecuzione del comando %s %s richiesto dal client %s, porta %d \n", MyStatus.UserCommand.textCommand, 
																			    MyStatus.UserCommand.username, 
																		        clientIP, clientPort );
	
	// Ricerca di un utente con lo stesso nome
	for ( i = 0 ; i < MyStatus.nextRecord ; i++ ){
		  if ( strcmp( MyStatus.UserCommand.username, MyStatus.myUsers[i].registeredUsername ) == 0 ){
			   userFound = TRUE;
			   break;
		  }
	}

	// Nome utente non registrato
	if ( userFound == FALSE ){

		if ( MyStatus.nextRecord == MAX_REGISTERED_USERS ){  	// Massimo numero di utenti raggiunto		 
			  printf("Impossibile eseguire !register: massimo numero di utenti registrabili raggiunto\n");
			  strcpy( replyMessage, REGISTER_LIMIT_REACHED );
		}
	    else{  // Registrazione utente
			  registerNewUser( clientIP, clientPort );
			  printf("Registrazione dell'utente %s effettuata con successo\n",MyStatus.UserCommand.username);
			  strcpy( replyMessage, REGISTER_SUCCESS );
		}
		
		error = send( socketID, replyMessage, REGISTER_MSG_LENGTH, MSG_NOSIGNAL ); // MSG_NOSIGNAL --> send() resituisce -1 se socket è stato chiuso 
		if ( error == SOCKET_SYSCALL_ERROR )
		     releaseClient( socketID, masterSet, error );
		return;
	}

	// Nome utente già registrato e online in questo momento
	if ( MyStatus.myUsers[i].connectionState == ONLINE ){
	 
		 printf("Impossibile registrare utente %s: l'utente è già online\n",MyStatus.UserCommand.username);
		 strcpy( replyMessage, REGISTER_ALREADY_ONLINE );
		
		 error = send( socketID, replyMessage, REGISTER_MSG_LENGTH, MSG_NOSIGNAL );
	 	 if ( error == SOCKET_SYSCALL_ERROR )
			  releaseClient( socketID, masterSet, error );
		return;
	}


	
	// Nome utente già registrato, ma offline in questo momento --> Riconnessione utente
	printf("Riconnessione dell'utente %s effettuata con successo\n", MyStatus.UserCommand.username );
	reconnectUser( i, clientIP,  clientPort ); 
	strcpy( replyMessage, REGISTER_RECONNECTION );
	
	error = send( socketID, replyMessage, REGISTER_MSG_LENGTH, MSG_NOSIGNAL );
	if ( error == SOCKET_SYSCALL_ERROR ){
	      releaseClient( socketID, masterSet, error );
		  return;
	}

	// Invio di eventuali messaggi ricevuti offline	
	messagesLength_host = offlineMessageStringSize(i);
	messagesLength_network = htons(messagesLength_host);

	userMessages = malloc( messagesLength_host + 1);
	memset( userMessages, 0, messagesLength_host+1 );

	createOfflineMessageString( userMessages, i );

	
	error = send( socketID, &messagesLength_network, sizeof(uint16_t), MSG_NOSIGNAL );
	if ( error == SOCKET_SYSCALL_ERROR ){
		  free(userMessages);
	      releaseClient( socketID, masterSet, error );
		  return;
	}
	
	error = send( socketID, userMessages, messagesLength_host, MSG_NOSIGNAL );
	if ( error == SOCKET_SYSCALL_ERROR ){
		  free(userMessages);
	      releaseClient( socketID, masterSet, error );
		  return;
	}


	MyStatus.myUsers[i].nextMessage = 0;
	printf("Invio dei messaggi ricevuti offline eseguito con successo\n");	
	free(userMessages);

	return;
	
}

