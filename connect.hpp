#ifndef CONNECTION
#define CONNECTION

/**
 * @description : establishes TCP connection with serverIP & port
 * @returns : file descriptor of socket
 * */
int establishConnection(const char *serverIP, int port);

#endif
