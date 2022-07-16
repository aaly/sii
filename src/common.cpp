#include "common.hpp"

namespace SII {
    const std::string getHomeDirectory() {
        struct passwd *pw = getpwuid(getuid());
          return std::string(pw->pw_dir);
      }
}

