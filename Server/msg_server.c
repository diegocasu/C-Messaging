
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "./Modules/Server_Constants.c"
#include "./Modules/Server_Setup.c"
#include "./Modules/Server_ConnectionAcceptor.c"
#include "./Modules/Server_ClientReleaser.c"
#include "./Modules/Server_CommandReceiver.c"
#include "./Modules/Commands/Server_Register.c"
#include "./Modules/Commands/Server_Deregister.c"
#include "./Modules/Commands/Server_Who.c"
#include "./Modules/Commands/Server_Quit.c"
#include "./Modules/Commands/Server_Send.c"
#include "./Modules/Server_CommandExecutor.c"



int main( int argc, char* argv[] ) {
										
	fd_set masterSet;   // Set di lettura per select() 
	fd_set readSet;	    // Set di lavoro in lettura per select()
	int setMax;			// Variabile che tiene traccia dell'identificatore più alto del set
	int execute;		// Flag che indica se comando è da eseguire o meno
	int i;


	setServerAddress( argc, argv );
	createHelloSocket();
	initializeSet( &masterSet, &readSet, &setMax );	

	while(1){
		readSet = masterSet;					
		startSelect( setMax, &readSet );		
			
		// Da questo punto in poi almeno un socket è pronto	in lettura			

		for( i = 0 ; i <= setMax ; i++ ){
			if ( FD_ISSET(i,&readSet) ){
	
				if ( i == MyStatus.helloSocket )
					  acceptClientConnection( &masterSet, &setMax );
				else{
					  execute = receiveCommand( i, &masterSet );						 
					  if ( execute == TRUE )
						   executeCommand( i, &masterSet );			
				}
			
			}
		}


	
	}

	return 0 ;

}
