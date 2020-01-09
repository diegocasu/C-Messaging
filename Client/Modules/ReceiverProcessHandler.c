
/****************                 *****************
***************** GESTIONE ERRORI *****************
*****************				  ****************/


/***** Errore in ricezione *****/

int checkReceiverProcessError(int error){

	if ( error == DROPPED_CONNECTION ){
		 printf("Connessione con l'utente non più attiva: ricezione del messaggio istantaneo fallita \n");
		 return FALSE;
	}
	
	if ( error == SOCKET_SYSCALL_ERROR ){
		 perror("Impossibile ricevere messaggio istantaneo: ");
		 return FALSE;
	}

	return TRUE;

}



/***** Errore nell'inizializzazione del socket UDP *****/

void checkSetupReceiverError( int error, char* errorMessage, int valueCheck ){

	if ( error == valueCheck ){
		pid_t myFather = getppid();
	
	 	closeSockets();
		perror(errorMessage);

		kill( myFather, SIGTERM );	 
	    exit(-1) ;

	}

}


/****************************************************           
*****************************************************
*****************************************************/



/***** Creazione socket UDP *****/

void createSocket_UDP(){
	int error;

	MyStatus.userCommunication = socket( AF_INET, SOCK_DGRAM, 0 ); 
	checkSetupReceiverError( MyStatus.userCommunication, "Impossibile creare socket per la comunicazione con un utente registrato: ", SOCKET_SYSCALL_ERROR );

	error = bind( MyStatus.userCommunication, (struct sockaddr*)&MyStatus.MyAddress, sizeof(MyStatus.MyAddress) );
	checkSetupReceiverError( error, "Impossibile eseguire bind() sul socket per la comunicazione con un utente registrato: ", SOCKET_SYSCALL_ERROR );

}



/***** Ricezione di un messaggio: username + testo *****/

int receiveSender(char** usernameSender){

	int usernameLength_host;
	uint16_t usernameLength_network;
	int error;
	int success;


	error = recvfrom( MyStatus.userCommunication, &usernameLength_network, sizeof(uint16_t), MSG_WAITALL, NULL, NULL );
	success = checkReceiverProcessError(error);

	if ( success == FALSE )
		 return FALSE;

	usernameLength_host = ntohs(usernameLength_network);
	*usernameSender = malloc(usernameLength_host + 1);			

	error = recvfrom( MyStatus.userCommunication, *usernameSender, usernameLength_host, MSG_WAITALL, NULL, NULL );
	success = checkReceiverProcessError(error);

	if ( success == FALSE ){
		 free(*usernameSender);
		 return FALSE;
	}

	*(*usernameSender + usernameLength_host) = '\0';

	return TRUE;

}



int receiveMessage( char** usernameSender, char** receivedMessage ){

	int messageLength_host;
	uint16_t messageLength_network;
	int error;
	int success;

	error = recvfrom( MyStatus.userCommunication, &messageLength_network, sizeof(uint16_t), MSG_WAITALL, NULL, NULL );
	success = checkReceiverProcessError(error);

	if ( success == FALSE ){
		 free(usernameSender);
		 return FALSE;
	}

	messageLength_host = ntohs(messageLength_network);
	*receivedMessage = malloc(messageLength_host + 1);
		
	error = recvfrom( MyStatus.userCommunication, *receivedMessage, messageLength_host, MSG_WAITALL, NULL, NULL );
	success = checkReceiverProcessError(error);

	if ( success == FALSE ){
		 free(*usernameSender);
		 free(*receivedMessage);
		 return FALSE;
	}

	*(*receivedMessage + messageLength_host) = '\0';

	return TRUE;

}



