#ifndef SERVER
#define SERVER

#include <queue>
#include <string>
void server(std::queue<std::string> &peerMessages,
            std::queue<std::string> &discoveryServerMessages);

#endif
