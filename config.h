#ifndef CONSTANTS
#define CONSTANTS
#include <string>

namespace Config {
using namespace std;
constexpr int SELF_TCP_SERVER_PORT = 4347;
constexpr const char *DISCOVERY_SERVER_IP = "127.0.0.1";
constexpr int DISCOVERY_SERVER_PORT = 4345;
constexpr int DEFAULT_RETRY = 3;
constexpr bool ENABLE_LOGS = true;
constexpr int MAX_USERNAME_LENGTH = 20;
constexpr const char *USERNAME_EXISTS = "exists";
} // namespace Config

#endif
