#ifndef SERVER
#define SERVER

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

void server(std::queue<std::string> &peerMessages,
            std::queue<std::string> &discoveryServerMessages,
            std::mutex &messagesMutex, std::condition_variable &wakeup);

#endif
