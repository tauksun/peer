#include "logger.hpp"
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

static void connectToPeer(string peer) {};
static void listPeers() {};

static void sendMessage(string msg) {
}

// -------- //
// Protocol //
// -------- //

// To connect with another peer
// >> User types - "connect:peerNameHere:password:passwordHere"
// To send message to already connected peer
// >> User types - "peerNameHere:messageHere"
// To list all connected peers
// >> User types - "list"

void takeInput() {
  while (1) {
    string input;
    cout << "\\<^_^>/  ";
    getline(cin, input);

    // Extract the user intent from first word &
    // execute accordingly
  }
}
