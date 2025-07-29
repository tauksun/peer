#ifndef LOGGER
#define LOGGER

#include "config.h"
#include <iostream>

template <typename... Args>

void logger(Args... args) {
  using namespace std;
  if (!Config::ENABLE_LOGS) {
    return;
  }

  // Un-pack parameters
  (cout << ... << args) << endl;

  return;
}

#endif
