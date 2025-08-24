#include "config.h"
#include "connect.hpp"
#include "logger.hpp"
#include "parser.hpp"
#include "readAndSendMessage.hpp"
#include "retry.hpp"
#include "server.hpp"
#include <arpa/inet.h>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

using namespace std;
void registerUserNameAndFetchPasskey(int discoverySocketFd, string &user,
                                     string &passkey) {

  string username("");
  cout << "Enter username : ";
  cin >> username;
  username = username.substr(0, Config::MAX_USERNAME_LENGTH);
  string payload = "register:username:" + username;

  int usernameBytesWritten = retry(
      write, [](int bytes) { return bytes > 0; }, Config::DEFAULT_RETRY,
      discoverySocketFd, payload.c_str(), strlen(payload.c_str()));

  if (usernameBytesWritten == -1) {
    perror("Unable to register user");
    exit(EXIT_FAILURE);
  }

  // Wait for username registeration
  char userRegisterationResponse[100] = {0};
  int recvBytes = recv(discoverySocketFd, userRegisterationResponse, 100, 0);
  if (recvBytes <= 0) {
    cout << "Could not register username, please try again";
    return registerUserNameAndFetchPasskey(discoverySocketFd, user, passkey);
  }

  if (recvBytes > 0 && recvBytes < 1023) {
    unordered_map<string, string> messageData;
    parseMessage(userRegisterationResponse, messageData);
    auto action = messageData.find("action");

    if (action->second == "discovery") {
      auto message = messageData.find("message");
      if (message == messageData.end()) {
        return;
      }
      if (message->second == "exists") {
        cout << "Username already exists, Please choose another" << endl;
        return registerUserNameAndFetchPasskey(discoverySocketFd, user,
                                               passkey);
      }

      if (message->second == "created") {
        auto passkeyData = messageData.find("passkey");
        if (passkeyData == messageData.end()) {
          return;
        }
        passkey = passkeyData->second;
        user = username;
      }
    }
  }

  cout << "Registration successful" << endl;
  cout << "Username : " << user << endl;
  cout << "Passkey : " << passkey << endl;
  return;
}

void peer() {
  logger("Starting peer...");

  // Establish connection with Discovery server
  int discoverySocketFd = retry(
      establishConnection, [](int fd) { return fd > -1; },
      Config::DEFAULT_RETRY, Config::DISCOVERY_SERVER_IP,
      Config::DISCOVERY_SERVER_PORT);

  if (discoverySocketFd == -1) {
    perror("Error while establishing connection with discovery server");
    exit(EXIT_FAILURE);
  }

  // Block here to register username & fetch passkey
  string user;
  string passkey;
  registerUserNameAndFetchPasskey(discoverySocketFd, user, passkey);
  cout << "Please share your username & passkey to another peer for "
          "establishing connection."
       << endl;

  // Start TCP server in another thread, while managing Chat in main
  // thread.

  mutex messagesMutex;
  condition_variable wakeup;
  queue<string> peerMessages;
  queue<string> discoveryServerMessages;
  thread serve(server, ref(peerMessages), ref(discoveryServerMessages),
               ref(messagesMutex), ref(wakeup));
  thread inputAndSendMessage(takeInput);

  while (1) {

    unique_lock<mutex> lock(messagesMutex);
    wakeup.wait(lock, [&peerMessages]() { return !peerMessages.empty(); });

    while (!peerMessages.empty()) {
      // Read messages from queue
      string msg = peerMessages.front();
      peerMessages.pop();
      cout << "@" << msg << endl;
    }

    lock.unlock();
  }

  serve.join();
  inputAndSendMessage.join();
}
