
/***** Comando !deregister *****/

void commandDeregister(){

	if ( MyStatus.currentState == OFFLINE ){
		 printf("Impossibile eseguire !deregister: non si è ancora effettuata una registrazione\n");
		 return;
	}


	// Aggiunta username corrente
	strcpy( MyStatus.UserCommand.username, MyStatus.currentUsername ) ;

	// Invio del comando al server
	sendCommand();

	MyStatus.currentState = OFFLINE;
	printf("Deregistrazione dell'utente %s effettuata con successo\n", MyStatus.currentUsername);
		

}
