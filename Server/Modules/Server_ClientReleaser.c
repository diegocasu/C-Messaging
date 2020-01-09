
/***** Utente disconnesso con !quit o caduto improvvisamente deve essere messo offline *****/

void setUserOffline(int socketID){

	char clientIP[16];
	int clientPort;
	int i;
	
	for ( i = 0 ; i < MyStatus.nextDescriptor ; i++ ){
		 if ( MyStatus.myConnections[i].socketID == socketID ){
			
			  clientPort = ntohs(MyStatus.myConnections[i].ClientAddress.sin_port);
			  inet_ntop( AF_INET, &(MyStatus.myConnections[i].ClientAddress.sin_addr), clientIP, sizeof(clientIP) );
		 }
	}

	for ( i = 0 ;  i < MyStatus.nextRecord ; i++ ){
		  if ( MyStatus.myUsers[i].connectionState == ONLINE && 
			   ( strcmp( MyStatus.myUsers[i].connectedIP, clientIP ) == 0 ) &&
			   MyStatus.myUsers[i].connectedPort == clientPort ){
			   
			   MyStatus.myUsers[i].connectionState = OFFLINE;
		  }
	}
	
}



/***** Rimozione descrittore *****/

void removeDescriptor( int socketID, int errorCode ){
	
	char clientIP[16];
	int clientPort;
	int i;
	int j;

	for ( i = 0 ; i < MyStatus.nextDescriptor ; i++ ){
		 if ( MyStatus.myConnections[i].socketID == socketID ){
			
			  // Recupero informazioni da stampare a video
			  clientPort = ntohs(MyStatus.myConnections[i].ClientAddress.sin_port);
			  inet_ntop( AF_INET, &(MyStatus.myConnections[i].ClientAddress.sin_addr), clientIP, sizeof(clientIP) );	

			  // Rialloco descrittori (sposto ciascun descrittore indietro di una posizione)
			  if ( i != MyStatus.nextDescriptor-1 ){
			  		for ( j = i+1 ; j < MyStatus.nextDescriptor ; j++ )
						MyStatus.myConnections[j-1] = MyStatus.myConnections[j];	
			  }
			  MyStatus.nextDescriptor--;
		}
	}

	if ( errorCode == DROPPED_CONNECTION ){
		printf("Disconnessione improvvisa --- Client %s, porta %d. Il comando !quit non è stato eseguito\n", clientIP, clientPort );
		return;
	}
	
	if ( errorCode == SOCKET_SYSCALL_ERROR ){
		printf("Disconnessione forzata --- Client %s, porta %d:", clientIP, clientPort );
		perror(" ");
	}
	

	if ( errorCode == NORMAL_QUIT )
		printf("Disconnessione eseguita --- Client %s, porta %d. \n", clientIP, clientPort );

}



/***** Cancellazione strutture client disconnesso *****/

void releaseClient( int socketID, fd_set* masterSet, int errorCode ){

	setUserOffline(socketID);	
	removeDescriptor( socketID, errorCode );   // Rimozione dell'entrata nella struttura MyStatus.myConnections	
	FD_CLR( socketID, masterSet );			  // Rimozione socket dal set di lettura
	close(socketID);				         // Chiusura socket

}

