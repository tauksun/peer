#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void client() {
  using namespace std;
  cout << "Initializing client..." << endl;

  const char *serverIP = "127.0.0.1";
  const int serverPort = 4345;

  // Create a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Establish a TCP connection with another server
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(serverPort);
  if (inet_pton(AF_INET, serverIP, &server.sin_addr) == -1) {
    perror("Error inet_pton");
    exit(EXIT_FAILURE);
  }

  int connectRet = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
  if (connectRet == -1) {
    perror("connection error");
    exit(EXIT_FAILURE);
  }

  // Read from terminal & send to server
  cout << " Starting read-write loop" << endl;

  std::string line;
  while (std::getline(std::cin, line)) {

    // Write & Read
    const char *message = line.c_str();
    if (strlen(message) == 0) {
      cout << "No message" << endl;
      continue;
    }
    cout << "Writing to server : " << message << endl;
    int bytes = write(sockfd, message, strlen(message));
    if (bytes == -1) {
      perror("Write failed");
      exit(EXIT_FAILURE);
    }

    char serverReply[1023] = {0};
    int recvBytes = recv(sockfd, &serverReply, 1023, 0);
    if (recvBytes <= 0) {
      cout << "recvBytes is less than 0, recvBytes : " << recvBytes << endl;
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    cout << "serverReply : " << serverReply << endl << endl;
  }
}
