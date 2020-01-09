
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "./Modules/Client_Constants.c"
#include "./Modules/SignalHandler.c"
#include "./Modules/InputParser.c"
#include "./Modules/Client_Setup.c"
#include "./Modules/ReceiverProcessHandler.c"
#include "./Modules/Client_CommandSender.c"
#include "./Modules/Commands/Client_Register.c"
#include "./Modules/Commands/Client_Help.c"
#include "./Modules/Commands/Client_Deregister.c"
#include "./Modules/Commands/Client_Who.c"
#include "./Modules/Commands/Client_Quit.c"
#include "./Modules/Commands/Client_Send.c"
#include "./Modules/Client_CommandExecutor.c"



int main( int argc, char* argv[] ){

	signal( SIGTERM, CleanExit );
	

	setClientAddresses( argc, argv );
	MyStatus.myPid = fork();

	if ( MyStatus.myPid != 0 ){	 // Processo dedicato all'interazione con il server	
		createSocket_TCP();
		connectToServer();
		printAvailableCommands();

		while(1){
			commandParser();
			executeCommand();
		}
	}



	if ( MyStatus.myPid == 0 ){	 // Processo che attende la ricezione di messaggi istantanei

		 char* usernameSender ;
		 char* receivedMessage ;
		 int success;
			 
		 createSocket_UDP();

		 while(1){			
			success = receiveSender(&usernameSender);	 

			if ( success == FALSE )
				 continue;

			success = receiveMessage( &usernameSender, &receivedMessage );

			if ( success == FALSE )
				 continue;

			printf("\n\nMessaggio ricevuto da %s\n", usernameSender );
			printf("%s\n", receivedMessage );
			 
 			free(usernameSender);
			free(receivedMessage);
		 }

	}
	
	return 0 ;
	
}
