#include "client.hpp"
#include "server.hpp"
#include <iostream>

int main() {
  using namespace std;
  cout << "Please select run mode : " << endl;
  cout << "Server : 1" << endl;
  cout << "Client : 2" << endl;
  int mode;
  cin >> mode;
  cout << "Selected Mode : " << mode << endl;
  if (mode == 1) {
    cout << "Initializing server mode ..." << endl;
    server();
  } else if (mode == 2) {
    cout << "Initializing client mode ..." << endl;
    client();
  }
}
