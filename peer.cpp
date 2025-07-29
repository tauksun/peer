#include "config.h"
#include "connect.hpp"
#include "logger.hpp"
#include "retry.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void registerSelf(int discoverySocketFd) {
  const char *registerSelf = "ping";
  int bytesWritten = retry(
      write, [](int bytes) { return bytes > 0; }, Config::DEFAULT_RETRY,
      discoverySocketFd, registerSelf, strlen(registerSelf));
  if (bytesWritten == -1) {
    perror("Error while registering to discovery server");
    exit(EXIT_FAILURE);
  }

  char discoveryServerPingReply[100] = {0};
  int recvBytes =
      recv(discoverySocketFd, discoveryServerPingReply, 99, 0);
  if (recvBytes == 0) {
    perror("Error during ping receive");
    exit(EXIT_FAILURE);
  }

  logger("Connection established with discovery server");
}

void registerUserNameAndFetchPasskey(int discoverySocketFd, char *user,
                                     char *passkey) {
  using namespace std;

  string username("");
  cout << "Enter username : ";
  cin >> username;
  username = username.substr(0, Config::MAX_USERNAME_LENGTH);

  string payload = "username:" + username;

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
    if (strncmp(userRegisterationResponse, Config::USERNAME_EXISTS,
                strlen(Config::USERNAME_EXISTS)) == 0) {
      userRegisterationResponse[recvBytes] = '\0';

      cout << "username : " << username
           << " already exists, please choose another." << endl;
      return registerUserNameAndFetchPasskey(discoverySocketFd, user, passkey);
    } else {
      user = (char *)username.c_str();
      passkey = userRegisterationResponse;
    }
  }

  cout << "Registration successful" << endl;
  cout << "Username : " << user << endl;
  cout << "Passkey : " << passkey << endl;
  return;
}

void peer() {
  using namespace std;
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

  // Register self
  registerSelf(discoverySocketFd);

  // Block here to register username & fetch passkey
  char user[20] = {0};
  char passkey[20] = {0};
  registerUserNameAndFetchPasskey(discoverySocketFd, user, passkey);

}
