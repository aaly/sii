#ifndef SERVICE_MANAGER_HPP
#define SERVICE_MANAGER_HPP
#include <string>
#include "common.hpp"

namespace SII {
    class serviceManager{
        public:
            serviceManager(const std::string& config_path);
           ~serviceManager(); 
        private:
    };
} //namespace SII
#endif //SERVICE_MANAGER_HPP
