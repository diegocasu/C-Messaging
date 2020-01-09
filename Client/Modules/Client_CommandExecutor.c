

/***** Funzione principale che decide quale comando eseguire *****/

void executeCommand(){

	if ( strcmp( MyStatus.UserCommand.textCommand, "!help" ) == 0 ){
		 commandHelp();
		 return;
	}
	
	if ( strcmp( MyStatus.UserCommand.textCommand, "!register" ) == 0 ){
	     commandRegister();
		 return;
	}
	
	if ( strcmp( MyStatus.UserCommand.textCommand, "!deregister" ) == 0 ){
	     commandDeregister();
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!who" ) == 0 ){
	     commandWho();
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!quit" ) == 0 ){
	     commandQuit();
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!send" ) == 0 ){
	     commandSend();
		 return;
	}

}
