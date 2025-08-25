#include <cstring>
#include <string>
#include <unordered_map>

using namespace std;

void parseMessage(const char *message, unordered_map<string, string> &messageData) {
  // Message format
  // action:key:value:key:value...
  int count = 0;
  string temp;
  string key;
  int len = strlen(message);
  for (int i = 0; message[i] != '\0'; i++) {
    if ((strncmp(&message[i], ":", 1) == 0) || (i == len - 1)) {
      if (count == 0) {
        // action
        messageData["action"] = temp;
      } else if (count % 2 == 0) {
        // value
        if (i == len - 1) {
          temp += message[i];
        }
        messageData[key] = temp;
        key = "";
      } else {
        // key
        key = temp;
      }

      count++;
      temp = "";
    } else {
      temp += message[i];
    }
  }
}
