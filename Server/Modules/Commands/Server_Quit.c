
/***** Comando !quit *****/

void commandQuit( int socketID, fd_set* masterSet){

	char clientIP[16];
	int clientPort;

	searchClientDetails( socketID, clientIP, &clientPort, sizeof(clientIP) );
	printf("Esecuzione del comando %s richiesto dal client %s, porta %d \n", MyStatus.UserCommand.textCommand,
																		     clientIP, clientPort );

	releaseClient( socketID, masterSet, NORMAL_QUIT );	

}
