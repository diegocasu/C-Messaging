
/***** Handler SIGTERM *****/

void CleanExit(int sig){

	closeSockets();
	exit(0);
	
}

