
void executeCommand( int socketID, fd_set* masterSet ){

	if ( strcmp( MyStatus.UserCommand.textCommand, "!register" ) == 0 ){
		 commandRegister( socketID, masterSet ); 
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!deregister" ) == 0 ){
		 commandDeregister(socketID);
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!who" ) == 0 ){
		 commandWho( socketID, masterSet );
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!quit")== 0 ){
		 commandQuit( socketID, masterSet );
		 return;
	}

	if ( strcmp( MyStatus.UserCommand.textCommand, "!send" ) == 0 ){
		 commandSend( socketID, masterSet );
		 return;
	}

}
