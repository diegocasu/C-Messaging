
/****************                 *****************
***************** GESTIONE ERRORI *****************
*****************				  ****************/


/***** Controllo errori recv() *****/

void checkCommandSendError_Receive( int error, char* userInfo ){

	if ( error == DROPPED_CONNECTION ){
		 printf("Connessione con il server non più attiva\n");
		 printf("Chiusura dell'applicazione\n'");
		 closeSockets();

		 if ( userInfo != NULL )
			  free(userInfo);			
	
		 kill( MyStatus.myPid, SIGTERM );
		 exit(1);
	}
	
	if ( error == SOCKET_SYSCALL_ERROR ){
		 perror("Impossibile eseguire comando: ");
		 closeSockets();

		 if ( userInfo != NULL )
			  free(userInfo);

		 kill( MyStatus.myPid, SIGTERM );
		 exit(-1);
	}


}


/***** Controllo errori recv() *****/

void checkCommandSendError_Send( int error, int* supportSocket ){

	if ( error == SOCKET_SYSCALL_ERROR ){
		 if ( errno == EPIPE ){		// Socket di connessione con il server è stato chiuso
			  printf("Connessione con il server non più attiva\n");
			  printf("Chiusura dell'applicazione\n'");
		 }
		 else
			perror("Errore nell'invio: ");
		
		kill( MyStatus.myPid, SIGTERM );
		closeSockets();
		
		if ( supportSocket != NULL )
    		close(*supportSocket);	

		exit(-1);	 
	}
	

}



/****************************************************           
*****************************************************
*****************************************************/



/***** Invio del messaggio al server (messaggistica offline) *****/

void sendMessageToServer( int messageLength_host, uint16_t messageLength_network, 
						  int usernameLength_host, uint16_t usernameLength_network, 
						  char* userMessage )
{

	int error;

	// Invio del nome utente (mittente): dimensione + testo
	error = send( MyStatus.serverCommunication, &usernameLength_network, sizeof(uint16_t), MSG_NOSIGNAL );		 	
	checkCommandSendError_Send(error,NULL);

	error = send( MyStatus.serverCommunication, MyStatus.currentUsername, usernameLength_host, MSG_NOSIGNAL );
	checkCommandSendError_Send(error,NULL);


	// Invio del messaggio: dimensione + testo
	error = send( MyStatus.serverCommunication, &messageLength_network, sizeof(uint16_t), MSG_NOSIGNAL );		 	
	checkCommandSendError_Send(error,NULL);

	error = send( MyStatus.serverCommunication, userMessage, messageLength_host, MSG_NOSIGNAL );
	checkCommandSendError_Send(error,NULL);
	
	printf("Messaggio offline inviato correttamente al server\n");
	return;

}



/***** Invio del messaggio ad un altro utente online (messaggistica istantanea) *****/

void receiveRecipientAddress( struct sockaddr_in* RecipientAddress ){

	uint16_t addressLength_network;
	int addressLength_host;
	char* userInfo; 	
	char  recipientIP[16];
	int   recipientPort;

	int error;

	error = recv( MyStatus.serverCommunication, &addressLength_network, sizeof(uint16_t), MSG_WAITALL );
	checkCommandSendError_Receive( error, NULL );

	addressLength_host = ntohs(addressLength_network);
	userInfo = malloc(addressLength_host + 1);
	
	error = recv( MyStatus.serverCommunication, userInfo, addressLength_host, MSG_WAITALL );
	checkCommandSendError_Receive( error, userInfo );
	*(userInfo + addressLength_host) = '\0';

	sscanf( userInfo, "%s %d", recipientIP, &recipientPort );
	free(userInfo);

	RecipientAddress->sin_family = AF_INET;
	RecipientAddress->sin_port = htons(recipientPort);
	inet_pton( AF_INET, recipientIP, &(RecipientAddress->sin_addr) );

}



void createSupportSocket( int* supportSocket ){

	*supportSocket = socket( AF_INET, SOCK_DGRAM, 0);

	if ( *supportSocket == SOCKET_SYSCALL_ERROR ){
		 perror("Impossibile eseguire comando: ");
		 closeSockets();

		 kill( MyStatus.myPid, SIGTERM );
		 exit(-1);
	}

}



void sendMessageToRecipient( int messageLength_host, uint16_t messageLength_network, 
						     int usernameLength_host, uint16_t usernameLength_network, 
						     char* userMessage )
{

	int supportSocket;
	struct sockaddr_in RecipientAddress;
	int error;

	// Ricezione indirizzo UDP del destinatario
	receiveRecipientAddress( &RecipientAddress );
	createSupportSocket( &supportSocket );

		
	// Invio al destinatario: username
	error = sendto( supportSocket, &usernameLength_network, sizeof(uint16_t), MSG_NOSIGNAL,
			        (struct sockaddr*)&RecipientAddress, sizeof(RecipientAddress) );
	checkCommandSendError_Send(error,&supportSocket);

	error = sendto( supportSocket, MyStatus.currentUsername, usernameLength_host, MSG_NOSIGNAL,
				   (struct sockaddr*)&RecipientAddress, sizeof(RecipientAddress) );
	checkCommandSendError_Send(error,&supportSocket);

	// Invio al destinatario: messaggio
	error = sendto( supportSocket, &messageLength_network, sizeof(uint16_t), MSG_NOSIGNAL,
			        (struct sockaddr*)&RecipientAddress, sizeof(RecipientAddress) );
	checkCommandSendError_Send(error,&supportSocket);

	
	error = sendto( supportSocket, userMessage, messageLength_host, MSG_NOSIGNAL,
			        (struct sockaddr*)&RecipientAddress, sizeof(RecipientAddress) );
	checkCommandSendError_Send(error,&supportSocket);

	close(supportSocket);
	printf("Messaggio istantaneo inviato correttamente\n");
	return;	

}




/***** Comando !send *****/

void commandSend(){

	char replyMessage[SEND_MSG_LENGTH + 1];
	char userMessage[MAX_MESSAGE_LENGTH + 1];

	int  	 messageLength_host;
	uint16_t messageLength_network;
	int		 usernameLength_host;
  	uint16_t usernameLength_network;

	int error;


	if ( MyStatus.currentState == OFFLINE ){
		 printf("Impossibile eseguire !send: non si è ancora effettuata una registrazione\n");
		 return;
	}

	if ( strcmp( MyStatus.currentUsername, MyStatus.UserCommand.username ) == 0 ){
		 printf("Impossibile effettuare !send: l'utente a cui inviare il messaggio sei tu!\n");
		 return;
	}

	// Invio del comando al server
	sendCommand();

	// Ricezione della risposta
	error = recv( MyStatus.serverCommunication, replyMessage, SEND_MSG_LENGTH, MSG_WAITALL );
	checkCommandSendError_Receive( error, NULL );
 	replyMessage[SEND_MSG_LENGTH] = '\0';


	if ( strcmp( replyMessage, SEND_USER_NOTFOUND ) == 0 ){
		 printf("L'utente specificato non è registrato nel server\n");
		 return;
	}	

	if ( strcmp( replyMessage, SEND_LIMIT_REACHED ) == 0 ){
		 printf("L'utente specificato è attualmente offline e il server non può memorizzare messaggi a lui diretti. Riprovare più tardi\n");
		 return;
	}

	if ( strcmp( replyMessage, SEND_USER_OFFLINE ) == 0 )
		 printf("L'utente specificato è attualmente offline. Il messaggio sarà recapitato alla successiva riconnessione\n");
	if ( strcmp( replyMessage, SEND_USER_ONLINE ) == 0 )
		 printf("L'utente specificato è attualmente online. Il messaggio sarà recapitato istantaneamente\n");
	
	
	memset( userMessage, 0, sizeof(userMessage) );
	getUserMessage(userMessage);	// Richiede all'utente di immettere messaggio

	messageLength_host = strlen(userMessage);
	messageLength_network = htons(messageLength_host);
	usernameLength_host = strlen(MyStatus.currentUsername);
	usernameLength_network = htons(usernameLength_host);

	// Utente a cui inviare messaggio è offline
	if ( strcmp( replyMessage, SEND_USER_OFFLINE ) == 0 ){
		 sendMessageToServer( messageLength_host, messageLength_network, usernameLength_host, usernameLength_network, userMessage );
		 return;
	}


	// Utente a cui inviare messaggio è online
	if ( strcmp( replyMessage, SEND_USER_ONLINE ) == 0 ){
		 sendMessageToRecipient( messageLength_host, messageLength_network, usernameLength_host, usernameLength_network, userMessage );
		 return;	
	}


}
