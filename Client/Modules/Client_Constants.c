
/***** COSTANTI ERRORI *****/

#define SOCKET_SYSCALL_ERROR     -1     // Valore restituito da una primitiva socket nel caso in cui non abbia successo
#define INET_PTON_ERROR_ADDRESS   0     // Valore restituito da inet_pton(...) nel caso in cui l'indirizzo IP fornito non sia valido
#define ATOI_CONVERSION_FAIL      0	    // Valore restituito da atoi(...) nel caso la stringa da convertire non sia un numero
#define NOT_SET_SOCKET			-10	    // Indica che l'intero non è un identificatore di socket valido
#define DROPPED_CONNECTION		  0		// Valore restituito da recv() quando socket da cui leggere è stato chiuso


/***** COSTANTI CLIENT *****/

#define INPUT_ADDRESSES 		  4      // Numero di argomenti da fornire al lancio del client da linea di comando
#define MAX_COMMAND_LENGTH        11     /* Massima lunghezza di un comando che un utente può immettere (escluso terminatore di stringa). 
										   In questo caso 11 è la lunghezza di !deregister */
#define MAX_USERNAME_LENGTH		  20	 // Lunghezza massima nome utente (escluso terminatore di stringa)
#define MAX_MESSAGE_LENGTH		  100	 // Lunghezza massima di un messaggio che può essere inviato ad un altro utente
#define MESSAGE_TERMINATOR		  ".\n"  // Terminatore di messaggio

#define FALSE					  0
#define TRUE					  1

#define OFFLINE					  0		// Stato utente offline
#define ONLINE					  1		// Stato utente online


/***** COSTANTI PROTOCOLLO *****/

#define CONNECTION_MSG_LENGTH  	  7		  	  // Lunghezza del messaggio di risposta inviato dopo accept()
#define CONNECTION_ACK_MSG		 "CONN_OK"	 // Notifica di avvenuta connessione al server
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

struct ParsedCommand{

	char textCommand[MAX_COMMAND_LENGTH + 1];	// Direttiva (ex. !send, !register, etc)
	char username[MAX_USERNAME_LENGTH + 1]; 	// Eventuale username immesso nel comando (o implicito, come in !quit e !deregister)
	
	char listenIP[16];	// Indirizzo IP e porta UDP per messaggistica istantanea. I campi vengono riempiti e
	int  listenPort;    // inviati al server durante l'esecuzione di !register

};



struct ClientStatus{

	pid_t myPid;  // Identificatore del processo

	int serverCommunication;  // Socket TCP per la comunicazione con il server
	int userCommunication;   // Socket UDP per la comunicazione con un altro client
	
	struct sockaddr_in MyAddress;		// Struttura contente indirizzo (UDP) di ascolto
	struct sockaddr_in ServerAddress;	// Struttura contenente indirizzo (TCP) del server

	struct ParsedCommand UserCommand;	// Comando digitato dall'utente

	int currentState;	// Online-Offline
	char currentUsername[MAX_USERNAME_LENGTH + 1];	// Username corrente: significativo solo se si è effettuata !register

};


struct ClientStatus MyStatus = { .serverCommunication = NOT_SET_SOCKET,
								 .userCommunication = NOT_SET_SOCKET,
								 .currentState = OFFLINE
							   };



/***** FUNZIONI *****/



/***** Chiusura socket client *****/

void closeSockets(){
	
	if ( MyStatus.serverCommunication != NOT_SET_SOCKET )
		 close(MyStatus.serverCommunication);

	if ( MyStatus.userCommunication != NOT_SET_SOCKET )
		 close(MyStatus.userCommunication);

}


