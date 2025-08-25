#ifndef PARSER
#define PARSER

#include <string>
#include <unordered_map>

void parseMessage(const char *message,
                  std::unordered_map<std::string, std::string> &messageData);

#endif
