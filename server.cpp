#include "config.h"
#include "logger.hpp"
#include "parser.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;

void handleClient(int readyFd, int epollfd, queue<string> &peerMessages,
                  queue<string> &discoveryServerMessages) {
  char clientMessage[1024] = {0};

  // TODO : Handle partial reads for logical parsing
  int recvValue = recv(readyFd, clientMessage, 1023, 0);

  if (recvValue <= 0) {
    close(readyFd);
    epoll_ctl(epollfd, EPOLL_CTL_DEL, readyFd, nullptr);
    return;
  }

  if (recvValue > 0 && recvValue < 1024)
    clientMessage[recvValue] = '\0';
  else
    clientMessage[1023] = '\0';

  logger("clientMessage : ", clientMessage);

  string serverReply;
  unordered_map<string, string> messageData;
  parseMessage(clientMessage, messageData);

  auto action = messageData.find("action");

  if (action->second == "peer") {
    // Peer Message
    auto user = messageData.find("username");
    auto message = messageData.find("message");
    string msg = user->second + " : " + message->second;
    peerMessages.push(msg);
  } else if (action->second == "discovery") {
    // Message from discovery server
    auto message = messageData.find("message");
    string msg = "discovery : " + message->second;
    discoveryServerMessages.push(msg);
  }
}

void server(queue<string> &peerMessages,
            queue<string> &discoveryServerMessages) {
  cout << "Starting server" << endl;
  // Socket
  int socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  cout << "Created socket with descriptor : " << socketDescriptor << endl;

  // Bind
  struct sockaddr_in add;
  add.sin_family = AF_INET;
  add.sin_port = htons(Config::SELF_TCP_SERVER_PORT);
  add.sin_addr.s_addr = INADDR_ANY;

  int bindResult = bind(socketDescriptor, (struct sockaddr *)&add, sizeof(add));
  cout << "bindResult : " << bindResult << endl;
  if (bindResult != 0) {
    throw "Error while binding to socket";
  }

  int yes = 1;
  if (setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
                 sizeof(yes)) < 0) {
    cout << "setsockopt(SO_REUSEADDR) failed" << endl;
  }

  // Listen
  int listenResult = listen(socketDescriptor, 100);
  cout << "listenResult : " << listenResult << endl;
  if (listenResult != 0) {
    throw "Error while listening on socket";
  }

  // Accept & Process Clients
  // E-Poll
  int maxConnections = 1000;
  struct epoll_event ev;
  vector<struct epoll_event> events(maxConnections);

  // Create E-Poll instance
  int epollfd = epoll_create1(0);
  if (epollfd == -1) {
    perror("Epoll error");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN;
  ev.data.fd = socketDescriptor;

  // Add socketDescriptor for monitoring
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketDescriptor, &ev) == -1) {
    perror("Epoll socketDescriptor error");
    exit(EXIT_FAILURE);
  }

  while (1) {
    int readyFds = epoll_wait(epollfd, &events[0], maxConnections, -1);
    if (readyFds == -1) {
      perror("epoll_wait error");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < readyFds; i++) {
      if (events[i].data.fd == socketDescriptor) {
        // Accept Client Connection
        int clientDescriptor = accept(socketDescriptor, NULL, NULL);
        if (clientDescriptor < 0) {
          cout << "client accept error : " << clientDescriptor << endl;
          continue;
        }
        cout << "Got client with descriptor : " << clientDescriptor << endl;

        ev.events = EPOLLIN;
        ev.data.fd = clientDescriptor;

        int clientAdd =
            epoll_ctl(epollfd, EPOLL_CTL_ADD, clientDescriptor, &ev);

        if (clientAdd == -1) {
          perror("epoll_ctl client add error");
          close(clientDescriptor);
        }
      } else {
        handleClient(events[i].data.fd, epollfd, peerMessages,
                     discoveryServerMessages);
      }
    }
  }
}
