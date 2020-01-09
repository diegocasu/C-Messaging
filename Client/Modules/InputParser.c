
/***** Pulizia buffer di ingresso *****/

void cleanInputBuffer(){

	int c ;
	while ( ( c = getchar() ) != '\n' && c != EOF ) { }  // Scarta anche \n, in quanto lo preleva prima del test di uguaglianza

}



/****************                  *****************
***************** GESTIONE COMANDI *****************
*****************				   ****************/


/***** Estrae comando da input utente *****/

int extractCommand( char* inputCommand, char* availableCommands[], int dim ){

	char directive[MAX_COMMAND_LENGTH + 1];
	int i;
	
	for( i = 0 ; i < dim ; i++ ){
		int compare;
		int length = strlen( availableCommands[i] ); 

		strncpy( directive, inputCommand, length ); 
		directive[length] = '\0';
	    
		compare = strcmp( availableCommands[i], directive );
			
		if ( compare == 0 ){ 
			
			 if ( strcmp( directive, "!send" ) == 0 || strcmp( directive, "!register" ) == 0 ){ // Direttiva complessa: estrazione nome utente
			 	  int startPoint = strlen(directive);				
				  int k = 0; // Usato per controllare spazi bianchi tra comando e username
				  			 	  
				  while( isspace(inputCommand[startPoint+k]))
						 k++;

				  if ( k == 0 )  // Comando e username devono essere separati da uno spazio
					   return FALSE;					
	
				  if ( k > 1 )  // Username non può iniziare con uno spazio bianco
					  return FALSE;

			 	  strncpy( MyStatus.UserCommand.textCommand, directive, sizeof(directive) );
			 	  strncpy( MyStatus.UserCommand.username, inputCommand+startPoint+1, MAX_USERNAME_LENGTH);
				  
				  strcpy( MyStatus.UserCommand.listenIP, "vuoto" );  // Nell'invio del comando si usa la conversione con sprintf/sscanf: i campi sono
				  MyStatus.UserCommand.listenPort = 0;				 // riempiti per evitare problemi nella conversione con stringhe vuote. Nel caso di
																	 // !register, i dati di ascolto sono aggiunti durante l'esecuzione del comando

			      return TRUE ;
			 }				 	

			// Siamo nel caso di direttiva semplice

			if ( strcmp(directive,inputCommand) == 0 ){	// Sono ammesse solamente direttive semplici senza argomenti
			     strncpy( MyStatus.UserCommand.textCommand, directive, sizeof(directive) );
				
				 strcpy( MyStatus.UserCommand.username, "vuoto" );  // Nell'invio del comando si usa la conversione con sprintf/sscanf: i campi sono
				 strcpy( MyStatus.UserCommand.listenIP, "vuoto" );  // riempiti per evitare problemi nella conversione con stringhe vuote
				 MyStatus.UserCommand.listenPort = 0;
				 
				return TRUE;
			}
			else // Non sono ammesse direttive semplici con argomenti
				 break; // La prima parte dell'input non può avere altre corrispondenze, quindi esco direttamente dal for
	    
		}

	}

	return FALSE;

}


/***** Interprete dei comandi *****/

void commandParser(){

	char* availableCommands[] = { "!register","!deregister","!who","!send","!help","!quit" };
	char inputCommand[MAX_COMMAND_LENGTH + MAX_USERNAME_LENGTH + 2];  // 2 = un whitespace + un terminatore di stringa
	
	int commandFound = FALSE;
	int dim = sizeof(availableCommands)/sizeof(availableCommands[0]);
	
	printf("Inserire un comando:\n");

	
	while (1){		
		if ( MyStatus.currentState == ONLINE )
		 	printf( "%s# ", MyStatus.currentUsername );
	
		memset( &MyStatus.UserCommand, 0, sizeof(MyStatus.UserCommand) );

		fgets( inputCommand, MAX_COMMAND_LENGTH + MAX_USERNAME_LENGTH + 2, stdin );  // fgets legge X-1 caratteri, dove X è il secondo argomento

		if ( inputCommand[strlen(inputCommand)-1] != '\n' )
			 cleanInputBuffer();   // pulizia buffer di ingresso

		inputCommand[strcspn(inputCommand, "\n")] = '\0';   // Rimozione dell'eventuale carattere \n ; strcspn restituisce posizione del primo \n

		commandFound = extractCommand( inputCommand, availableCommands, dim );
		
		if ( commandFound == TRUE ){
			 printf("\n");
			 return;
		 }

		 printf("Comando non valido. Inserire un comando valido\n");		 	
	}

}


/****************************************************           
*****************************************************
*****************************************************/



/****************                   *****************
***************** GESTIONE MESSAGGI *****************
*****************				    ****************/

void getUserMessage( char* userMessage ){

	char buffer[MAX_MESSAGE_LENGTH + 1];
	int charactersRemaining = MAX_MESSAGE_LENGTH ;

	printf("Inserire il messaggio (massimo %d caratteri e terminato da una linea singola \'.\') \n", MAX_MESSAGE_LENGTH );
	printf("Nel caso il messaggio sia più lungo di %d caratteri, ", MAX_MESSAGE_LENGTH);
	printf("sarà troncato in automatico e non verrà richiesto il terminatore\n");
	printf( "%s# ", MyStatus.currentUsername );

	while (1){			
			memset( buffer, 0, sizeof(buffer) );
			
			fgets( buffer, charactersRemaining + 1, stdin );

			if ( buffer[strlen(buffer)-1] != '\n' )
			     cleanInputBuffer();   // pulizia buffer di ingresso

			if ( strcmp( buffer, MESSAGE_TERMINATOR ) == 0 )
				 break;

			strcat( userMessage, buffer );
			charactersRemaining = charactersRemaining - strlen(buffer);	

			if ( charactersRemaining == 0 )
				 break;
	}

	userMessage[MAX_MESSAGE_LENGTH] = '\0';

}

