
/***** Verifica stato dell'utente a cui si vuole inviare messaggio *****/

void setSendReply( char* replyMessage, int* userDescriptorIndex ){
	
	int i;

	strcpy( replyMessage, SEND_USER_NOTFOUND );

	for ( i = 0 ; i < MyStatus.nextRecord ; i++ ){
		  if ( strcmp( MyStatus.UserCommand.username, MyStatus.myUsers[i].registeredUsername ) == 0 ){
			   *userDescriptorIndex = i ;
			   
			   if ( MyStatus.myUsers[i].connectionState == ONLINE ){		   
					strcpy( replyMessage, SEND_USER_ONLINE );
				    printf("L'utente richiesto è online\n");
					break;
			   }
			
			   // Utente è offline
			   if ( MyStatus.myUsers[i].nextMessage == MAX_MSG_QUEUE_LENGTH ){
					strcpy( replyMessage, SEND_LIMIT_REACHED );
					printf("Casella di posta dell'utente piena\n");	
					break;		
				}
		
			   strcpy( replyMessage, SEND_USER_OFFLINE );
			   printf("L'utente richiesto è offline\n");
			   break;
		  }
	}

	if ( strcmp( replyMessage, SEND_USER_NOTFOUND ) == 0 )
		 printf("L'utente richiesto non è registrato\n");

}



/***** Comando !send *****/

void commandSend( int socketID, fd_set* masterSet ){

	char replyMessage[SEND_MSG_LENGTH + 1];
	char clientIP[16];
	int clientPort;
	uint16_t messageLength_network;
	int messageLength_host;
	int userIndex;

	int error;


	searchClientDetails( socketID, clientIP, &clientPort, sizeof(clientIP) );
	printf("Esecuzione del comando %s %s richiesto dal client %s, porta %d \n", MyStatus.UserCommand.textCommand, 
																			    MyStatus.UserCommand.username, 
																		        clientIP, clientPort );

	// Ricerca di un utente con lo stesso nome
	setSendReply( replyMessage, &userIndex );
	
	// Invio del risultato
	error = send( socketID, replyMessage, SEND_MSG_LENGTH, MSG_NOSIGNAL ); // MSG_NOSIGNAL --> send() resituisce -1 se socket è stato chiuso 
	if ( error == SOCKET_SYSCALL_ERROR ){
	     releaseClient( socketID, masterSet, error );
		 return;
	}



	// Messaggistica offline
	if ( strcmp( replyMessage, SEND_USER_OFFLINE ) == 0 ){
		 int nextMessageIndex = MyStatus.myUsers[userIndex].nextMessage;
	 	 int usernameLength_host;
		 uint16_t usernameLength_network;


		 // Ricezione dello username: dimensione + testo		
		 error = recv( socketID, &usernameLength_network , sizeof(uint16_t), MSG_WAITALL );

		 if ( error == DROPPED_CONNECTION || error == SOCKET_SYSCALL_ERROR ){	 
		 	  releaseClient( socketID, masterSet, error );
			  return;
		 }
		 
		 usernameLength_host = ntohs(usernameLength_network);
		 error = recv( socketID, MyStatus.myUsers[userIndex].messageQueue[nextMessageIndex].sender, usernameLength_host, MSG_WAITALL );

		 if ( error == DROPPED_CONNECTION || error == SOCKET_SYSCALL_ERROR ){	 
		 	releaseClient( socketID, masterSet, error );
			return;
		 }

		 MyStatus.myUsers[userIndex].messageQueue[nextMessageIndex].sender[usernameLength_host] = '\0';
	
		 // Ricezione del messaggio: dimensione + testo
	     error = recv( socketID, &messageLength_network , sizeof(uint16_t), MSG_WAITALL );

		 if ( error == DROPPED_CONNECTION || error == SOCKET_SYSCALL_ERROR ){	 
		 	releaseClient( socketID, masterSet, error );
			return;
		 }
		 
		 messageLength_host = ntohs(messageLength_network);
		 error = recv( socketID, MyStatus.myUsers[userIndex].messageQueue[nextMessageIndex].message, messageLength_host, MSG_WAITALL );

		 if ( error == DROPPED_CONNECTION || error == SOCKET_SYSCALL_ERROR ){	 
		 	releaseClient( socketID, masterSet, error );
			return;
		 }

		 MyStatus.myUsers[userIndex].messageQueue[nextMessageIndex].message[messageLength_host] = '\0';
		 MyStatus.myUsers[userIndex].nextMessage++ ;

		 printf("Messaggio offline ricevuto correttamente\n");
		 return ;
	}
	

	// Messaggistica istantanea
	if ( strcmp( replyMessage, SEND_USER_ONLINE ) == 0 ){
		
		uint16_t addressLength_network;
		int addressLength_host;
		char* userInfo;
				 
		addressLength_host = snprintf( NULL, 0, "%s %d", MyStatus.myUsers[userIndex].registeredIP, MyStatus.myUsers[userIndex].registeredPort );
		addressLength_network = htons(addressLength_host);
		userInfo = malloc(addressLength_host + 1);

		sprintf( userInfo, "%s %d", MyStatus.myUsers[userIndex].registeredIP, MyStatus.myUsers[userIndex].registeredPort );

		error = send( socketID, &addressLength_network, sizeof(uint16_t), MSG_NOSIGNAL ); 
		if ( error == SOCKET_SYSCALL_ERROR ){
			free(userInfo);
	    	releaseClient( socketID, masterSet, error );
		    return;
	   }

	   error = send( socketID, userInfo, addressLength_host, MSG_NOSIGNAL ); 
		if ( error == SOCKET_SYSCALL_ERROR ){
			free(userInfo);
	    	releaseClient( socketID, masterSet, error );
		    return;
	   }

	
	   printf("Dati di ascolto %s %d dell'utente %s richiesto inviati correttamente\n", MyStatus.myUsers[userIndex].registeredIP,
																						MyStatus.myUsers[userIndex].registeredPort,
																						MyStatus.UserCommand.username );
	   free(userInfo);


	}


}
