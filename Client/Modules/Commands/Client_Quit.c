
/***** Comando !quit *****/

void commandQuit(){

	// Invio del comando al server
	sendCommand();

	closeSockets();
	kill( MyStatus.myPid, SIGTERM );

	printf("Chiusura dell'applicazione\n");
	exit(0);

}
