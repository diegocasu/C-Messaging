
/***** Controllo errori *****/

void checkCommandRegisterError( int error, char* messagesList ){

	if ( error == DROPPED_CONNECTION ){
		 printf("Connessione con il server non più attiva\n");
		 printf("Chiusura dell'applicazione\n'");
		 closeSockets();

		 if ( messagesList != NULL )
			  free(messagesList);

		 kill( MyStatus.myPid, SIGTERM );
		 exit(1);
	}


	if ( error == SOCKET_SYSCALL_ERROR ){
		 perror("Impossibile eseguire comando: ");
		 closeSockets();
	
		 if ( messagesList != NULL )
			  free(messagesList);
		 
		 kill( MyStatus.myPid, SIGTERM );
		 exit(-1);
	}
	
}


/***** Comando !register *****/

void commandRegister(){

	char replyMessage[REGISTER_MSG_LENGTH + 1];
	int messagesLength_host;
	int messagesLength_network;
	char* messagesList;

	int error;


	if ( MyStatus.currentState == ONLINE ){
		 printf("Impossibile effettuare !register: si è già registrati come %s\n", MyStatus.currentUsername );
		 return;
	}
	

	// Inserimento di IP e porta di ascolto UDP da inviare al server
	MyStatus.UserCommand.listenPort = ntohs(MyStatus.MyAddress.sin_port);
	inet_ntop( AF_INET, &(MyStatus.MyAddress.sin_addr), MyStatus.UserCommand.listenIP, sizeof(MyStatus.UserCommand.listenIP) );
	
	// Invio del comando al server
	sendCommand();

	// Ricezione risposta
	error = recv( MyStatus.serverCommunication, replyMessage, REGISTER_MSG_LENGTH, MSG_WAITALL );
	checkCommandRegisterError( error, NULL );
	replyMessage[REGISTER_MSG_LENGTH] = '\0';

 
	if ( strcmp( replyMessage, REGISTER_LIMIT_REACHED ) == 0 ){
		 printf("Impossibile effettuare la registrazione: numero massimo di utenti registrati sul server raggiunto\n");
		 return;
	}
		
	if ( strcmp( replyMessage, REGISTER_ALREADY_ONLINE ) == 0 ){
		 printf("Impossibile effettuare la registrazione: un utente con lo stesso nome è online\n");
		 return;
	}

	if ( strcmp( replyMessage, REGISTER_SUCCESS ) == 0 ){
		 MyStatus.currentState = ONLINE;
		 strcpy( MyStatus.currentUsername, MyStatus.UserCommand.username ); 
		 printf("Registrazione effettuata con successo come %s\n", MyStatus.currentUsername );
		 return;
	}


	if ( strcmp( replyMessage,REGISTER_RECONNECTION ) == 0 ){
		 MyStatus.currentState = ONLINE;
		 strcpy( MyStatus.currentUsername, MyStatus.UserCommand.username ); 
		 printf("Riconnessione effettuata con successo come %s\n", MyStatus.currentUsername );
		 
		 // Ricezione messaggi offline: dimensione + testo
		 error = recv( MyStatus.serverCommunication, &messagesLength_network, sizeof(uint16_t), MSG_WAITALL );
	     checkCommandRegisterError( error, NULL );

		 messagesLength_host = ntohs(messagesLength_network);
		 messagesList = malloc(messagesLength_host + 1);
		 
		 error = recv( MyStatus.serverCommunication, messagesList, messagesLength_host, MSG_WAITALL );
		 checkCommandRegisterError( error, messagesList );

		 *( messagesList + messagesLength_host ) = '\0';

		 printf("%s", messagesList);
		 free(messagesList);
			
		return;
	}


}
