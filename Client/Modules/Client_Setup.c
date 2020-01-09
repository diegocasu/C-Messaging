
/****************                 *****************
***************** GESTIONE ERRORI *****************
*****************				  ****************/


/***** Gestore errori *****/

void checkSetupError( int error, char* errorMessage, int valueCheck ){

	if ( error == valueCheck ){
	
	 	closeSockets();

		 // inet_pton(..) e atoi(..) non modificano errno in caso di errore
		if ( valueCheck == INET_PTON_ERROR_ADDRESS || valueCheck == ATOI_CONVERSION_FAIL ){  
			 printf("%s\n",errorMessage);
			 exit(-1);	// Non chiamo kill(...), perchè in questi due casi non è stata ancora eseguita fork()
		}
		 
		 perror(errorMessage);
		 kill( MyStatus.myPid, SIGTERM );		 
	     exit(-1) ;
	
	}

}

/****************************************************           
*****************************************************
*****************************************************/



/***** Gestisce indirizzi forniti in input dal client *****/

void setClientAddresses( int argc, char* argv[] ){
	
	int myPort;
	int serverPort;
	char* myIP;
	char* serverIP;

	int error;	


	if ( argc != INPUT_ADDRESSES + 1 ){
		printf( "%s\n", "Impossibile avviare il client. Riprovare rispettando la seguente sintassi:" );
		printf( "%s\n", "./msg_client <IP locale> <porta locale> <IP server> <porta server>" );
		exit(1);
	}

	memset( &MyStatus.MyAddress, 0, sizeof(MyStatus.MyAddress) );
	memset( &MyStatus.ServerAddress, 0, sizeof(MyStatus.ServerAddress) );

	myIP = argv[1];
	myPort = atoi(argv[2]);
	serverIP = argv[3];
	serverPort = atoi(argv[4]);

	checkSetupError( myPort, "Impossibile avviare il client: la porta locale fornita non è un numero", ATOI_CONVERSION_FAIL );
	checkSetupError( serverPort, "Impossibile avviare il client: la porta server fornita non è un numero", ATOI_CONVERSION_FAIL );
		
	MyStatus.MyAddress.sin_family = AF_INET;
	MyStatus.MyAddress.sin_port = htons(myPort);
	error = inet_pton( AF_INET, myIP, &(MyStatus.MyAddress.sin_addr) ) ;
	checkSetupError( error, "Indirizzo IP locale fornito non valido", INET_PTON_ERROR_ADDRESS );	

	MyStatus.ServerAddress.sin_family = AF_INET; 
	MyStatus.ServerAddress.sin_port = htons(serverPort);
	error = inet_pton( AF_INET, serverIP, &(MyStatus.ServerAddress.sin_addr) ) ;
	checkSetupError( error, "Indirizzo IP server fornito non valido", INET_PTON_ERROR_ADDRESS );

}



/***** Creazione di un socket TCP *****/

void createSocket_TCP(){
	
	MyStatus.serverCommunication = socket( AF_INET, SOCK_STREAM, 0 );	
	checkSetupError( MyStatus.serverCommunication, "Impossibile creare socket per la comunicazione con il server: ", SOCKET_SYSCALL_ERROR );

}



/***** Inizializza connessione con il server  *****/

void connectToServer(){
	
	char connectedIP[16];
	int connectedPort;	
	char isAccepted[CONNECTION_MSG_LENGTH + 1];
	int error;

	error = connect( MyStatus.serverCommunication, (struct sockaddr*)&MyStatus.ServerAddress, sizeof(MyStatus.ServerAddress) );
	checkSetupError( error, "Impossibile stabilire una connessione con il server", SOCKET_SYSCALL_ERROR );	

	error = recv( MyStatus.serverCommunication, isAccepted, CONNECTION_MSG_LENGTH, MSG_WAITALL );
	isAccepted[CONNECTION_MSG_LENGTH] = '\0';
	checkSetupError( error, "Impossibile stabilire una connessione con il server", SOCKET_SYSCALL_ERROR );
	checkSetupError( error, "Impossibile stabilire una connessione con il server", DROPPED_CONNECTION );

	if ( strcmp(isAccepted,CONNECTION_ERROR_MSG) == 0 ){
		 printf("Connessione rifiutata dal server: numero massimo di connessioni contemporanee raggiunto\n");
		 closeSockets();
		 kill( MyStatus.myPid, SIGTERM );
		 exit(1);	
	}

	connectedPort = ntohs(MyStatus.ServerAddress.sin_port);
	inet_ntop( AF_INET, &(MyStatus.ServerAddress.sin_addr), connectedIP, sizeof(connectedIP) );	 
	
	printf("Connessione stabilita con %s, ", connectedIP );
	printf("porta %d \n\n", connectedPort );
	
}



/***** Stampa elenco dei comandi disponibili *****/

void printAvailableCommands(){

	printf("Sono disponibili i seguenti comandi:\n");

	printf("!register <NomeUtente> : registra il client presso il server\n"
	"!deregister : annulla la registrazione del client presso il server\n"
	"!who : mostra l'elenco degli utenti registrati e il loro stato\n"
	"!send <NomeUtente> : invia un messaggio all'utente scelto\n"
	"!help : mostra l'elenco dei comandi disponibili\n"
	"!quit : disconnette il client dal server ed esce\n"
	);
	
	printf("\n");

}
