
/***** Comando !deregister *****/

void commandDeregister( int socketID ){

	char clientIP[16];
	int clientPort;
		
	int i;
	int j;

	
	searchClientDetails( socketID, clientIP, &clientPort, sizeof(clientIP) );
	printf("Esecuzione del comando %s %s richiesto dal client %s, porta %d \n", MyStatus.UserCommand.textCommand, 
																			    MyStatus.UserCommand.username, 
																		        clientIP, clientPort );

	for ( i = 0 ; i < MyStatus.nextRecord ; i++ ){
		  if ( strcmp( MyStatus.UserCommand.username, MyStatus.myUsers[i].registeredUsername ) == 0 ){
			
			  // Rialloco descrittori (sposto ciascun descrittore indietro di una posizione)
			  if ( i != MyStatus.nextRecord-1 ){
			  		for ( j = i+1 ; j < MyStatus.nextRecord ; j++ )
						  MyStatus.myUsers[j-1] = MyStatus.myUsers[j];	
			  }
			  MyStatus.nextRecord--;
		
          }			 
	}
	
	printf("Deregistrazione dell'utente %s effettuata con successo\n", MyStatus.UserCommand.username );

}


