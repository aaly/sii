#ifndef COMMON_HPP
#define COMMON_HPP
#include <string>
#include <iostream>

// IF LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
// END IF LINUX

namespace SII {
    const std::string getHomeDirectory(); 
} //namespace SII
#endif //COMMON_HPP
