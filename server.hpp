#ifndef SERVER
#define SERVER

#include <mutex>
#include <queue>
#include <string>

void server(std::queue<std::string> &peerMessages,
            std::queue<std::string> &discoveryServerMessages,
            std::mutex &messagesMutex);

#endif
