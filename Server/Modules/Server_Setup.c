
/****************                 *****************
***************** GESTIONE ERRORI *****************
*****************				  ****************/

void checkSetupError( int error, char* errorMessage, int valueCheck ){

	if ( error == valueCheck ){	

		 closeSockets();	

		 if ( valueCheck == ATOI_CONVERSION_FAIL ){
		 	  printf("%s\n",errorMessage);
			  exit(-1);
		 }
 
		 perror(errorMessage);
		 exit(-1) ;	
	}
}

/****************************************************           
*****************************************************
*****************************************************/



/***** Imposta indirizzo e porta del server *****/

void setServerAddress( int argc, char* argv[] ){

	int serverPort;

	printf("Inizializzazione server\n");

	if ( argc != SERVER_START_INPUT + 1 ){
		printf( "%s\n", "Impossibile avviare il server. Riprovare rispettando la seguente sintassi:" );
		printf( "%s\n", "./msg_server <Porta server>" );
		exit(1);
	}

	memset( &MyStatus.MyAddress , 0, sizeof(MyStatus.MyAddress) );

	serverPort = atoi(argv[1]);
	checkSetupError( serverPort, "Impossibile avviare il server: la porta fornita non è un numero valido", ATOI_CONVERSION_FAIL );

	MyStatus.MyAddress.sin_family = AF_INET;
	MyStatus.MyAddress.sin_port = htons(serverPort);
	MyStatus.MyAddress.sin_addr.s_addr = INADDR_ANY;

}



/***** Inizializza socket di benvenuto *****/

void createHelloSocket(){

	int error;
	int serverPort;
	
	MyStatus.helloSocket = socket( AF_INET, SOCK_STREAM, 0 );
	checkSetupError( MyStatus.helloSocket, "Impossibile creare socket di benvenuto: ", SOCKET_SYSCALL_ERROR );

	error = bind( MyStatus.helloSocket, (struct sockaddr*)&MyStatus.MyAddress, sizeof(MyStatus.MyAddress) );
	checkSetupError( error, "Impossibile eseguire bind() sul socket di benvenuto: ", SOCKET_SYSCALL_ERROR );

	error = listen( MyStatus.helloSocket, LISTEN_BACKLOG );
	checkSetupError( error, "Impossibile eseguire listen() sul socket di benvenuto: ", SOCKET_SYSCALL_ERROR );

	serverPort = ntohs(MyStatus.MyAddress.sin_port);
	printf("Socket di benvenuto correttamente inizializzato, in ascolto sulla porta %d \n", serverPort);

}



/***** Inizializza set di socket per la select() *****/

void initializeSet( fd_set* masterSet, fd_set* readSet, int* setMax ){
	
	FD_ZERO(masterSet);		// Pulizia
	FD_ZERO(readSet);  		// Pulizia

	FD_SET( MyStatus.helloSocket, masterSet );
	*setMax = MyStatus.helloSocket;

}



/***** Esecuzione della select() con controllo degli errori *****/

void startSelect( int setMax, fd_set* readSet ){

	int error;

	error = select( setMax+1, readSet, NULL, NULL, NULL );
	checkSetupError( error, "Impossibile eseguire select(): ", SOCKET_SYSCALL_ERROR );

}
