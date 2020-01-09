
/***** COSTANTI ERRORI *****/

#define SOCKET_SYSCALL_ERROR     -1     // Valore restituito da una primitiva socket nel caso in cui non abbia successo
#define ATOI_CONVERSION_FAIL      0	    // Valore restituito da atoi(...) nel caso la stringa da convertire non sia un numero
#define NOT_SET_SOCKET			-10	    // Indica che l'intero non è un identificatore di socket valido
#define DROPPED_CONNECTION		  0		// Valore restituito da recv() quando socket da cui leggere è stato chiuso
#define NORMAL_QUIT				200		// Valore passato a clientRelease() quando utente si disconnette correttamente


/***** COSTANTI SERVER *****/

#define SERVER_START_INPUT	      1	    // Numero di argomenti da specificare al lancio del programma server
#define LISTEN_BACKLOG			  10	// Dimensione della coda di backlog in listen()
#define MAX_CLIENT_CONNECTED      3	    /* Numero di client che possono essere connessi al server contemporaneamente	
			   							   Usato per determinare massimo numero di socket aperti */
#define MAX_COMMAND_LENGTH        11    /* Massima lunghezza di un comando che un utente può immettere (escluso terminatore di stringa). 
										   In questo caso 11 è la lunghezza di !deregister */
#define MAX_USERNAME_LENGTH		  20	// Lunghezza massima nome utente (escluso terminatore di stringa)
#define MAX_MESSAGE_LENGTH		  100   // Lunghezza massima di un messaggio che può essere inviato ad un altro utente
#define MAX_MSG_QUEUE_LENGTH	  3     // Numero massimo di messaggi che possono essere mantenuti in memoria nel caso l'utente sia offline
#define MAX_REGISTERED_USERS	  3		// Numero massimo di utenti registrati che il server può supportare

#define OFFLINE					  0		// Stato utente offline
#define ONLINE					  1		// Stato utente online

#define FALSE					  0
#define TRUE					  1

/***** COSTANTI PROTOCOLLO *****/

#define CONNECTION_MSG_LENGTH  	  7		  	  // Lunghezza del messaggio di risposta inviato dopo accept()
#define CONNECTION_ACK_MSG		 "CONN_OK"	 // Notifica di connessione avvenuta da inviare al client
#define CONNECTION_ERROR_MSG	 "CONN_ER"	// Notifica di limite utenti connessi al server raggiunta 

#define REGISTER_MSG_LENGTH		  7	    		// Lunghezza del messaggio di risposta a !register
#define REGISTER_LIMIT_REACHED	 "REG_LIM"     // Notifica di massimo numero di utenti registrabili raggiunto
#define REGISTER_ALREADY_ONLINE  "REG_ONL"    // Notifica di utente già online
#define REGISTER_SUCCESS		 "REG_SUC"	 //	Notifica di registrazione effettuata con successo
#define REGISTER_RECONNECTION	 "REG_REC"	// Notifica di riconnessione effettuata con successo

#define SEND_MSG_LENGTH		  	  8         	//  Lunghezza del messaggio di risposta a !send
#define SEND_USER_NOTFOUND		 "SEND_NOT"    //  Notifica di nome utente inesistente
#define SEND_USER_OFFLINE		 "SEND_OFF"   //  Notifica di utente offline
#define SEND_USER_ONLINE		 "SEND_ONL"  //  Notifica di utente online
#define SEND_LIMIT_REACHED		 "SEND_LIM" //  L'utente è offline ed il server non può memorizzare messaggio



/***** STRUTTURE *****/


struct OfflineMessage{
	
	char message[MAX_MESSAGE_LENGTH +1];
	char sender[MAX_USERNAME_LENGTH +1];

};


struct RegisteredUser{

	int connectionState;	// Online-Offline 
	char registeredUsername[MAX_USERNAME_LENGTH + 1]; 
	
	char registeredIP[16];	// Necessario per messaggistica istantanea
	int  registeredPort;	// Necessario per messaggistica istantanea
	
	char connectedIP[16];	// Necessari per mettere utente offline; indirizzo IP e porta 
	int  connectedPort;		// con cui utente si è registrato o ha fatto log in
			
	struct OfflineMessage messageQueue[MAX_MSG_QUEUE_LENGTH];	// Casella di postra per messaggistica offline
	int nextMessage;

};


struct ParsedCommand{

	char textCommand[MAX_COMMAND_LENGTH + 1];
	char username[MAX_USERNAME_LENGTH + 1]; 

	char listenIP[16];	// Indirizzo IP e porta UDP per messaggistica istantanea. I campi vengono riempiti
	int  listenPort;    // e inviati al server durante l'esecuzione di !register

};


struct ConnectionDescriptor{
	
	int socketID ;
	struct sockaddr_in ClientAddress;

};



/***** DESCRITTORE DEL SERVER *****/


struct ServerStatus{

	int helloSocket;				 // Socket di benvenuto
	struct sockaddr_in MyAddress;	//  Indirizzo del server

	struct ConnectionDescriptor myConnections[MAX_CLIENT_CONNECTED]; // Lista dei socket aperti con i client
	int nextDescriptor;												 // Variabile che tiene traccia del numero di client attualmente connessi

	struct ParsedCommand UserCommand;

	struct RegisteredUser myUsers[MAX_REGISTERED_USERS];	// Lista degli utenti registrati
	int nextRecord;
	
};


// Stato del server

struct ServerStatus MyStatus = { .helloSocket = NOT_SET_SOCKET,
								 .nextDescriptor = 0,
								 .nextRecord = 0 
							   };





/***** FUNZIONI DI UTILITA' *****/


/***** Chiusura socket *****/

void closeSockets(){
	
	int i;	

	if ( MyStatus.helloSocket != NOT_SET_SOCKET )
		 close(MyStatus.helloSocket);

	if ( MyStatus.nextDescriptor > 0)
		 for( i = 0 ; i < MyStatus.nextDescriptor ; i++ )
			 close( MyStatus.myConnections[i].socketID );

}



/***** Funzione per estrarre indirizzo IP e porta di un client dal suo descrittore *****/

void searchClientDetails( int socketID, char* clientIP, int* clientPort, int dim ){

	int i;

	for ( i = 0 ; i < MyStatus.nextDescriptor ; i++ ){
		 if ( MyStatus.myConnections[i].socketID == socketID ){
	  
			  *clientPort = ntohs(MyStatus.myConnections[i].ClientAddress.sin_port);
			  inet_ntop( AF_INET, &(MyStatus.myConnections[i].ClientAddress.sin_addr), clientIP, dim );
		}
	}

}

