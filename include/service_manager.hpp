#ifndef SERVICE_MANAGER_HPP
#define SERVICE_MANAGER_HPP
#include <string>
#include "common.hpp"
#include "service.hpp"
#include <unordered_map>

namespace SII {
    class serviceManager{
        public:
            serviceManager(const std::string& config_path);
           ~serviceManager(); 
        private:
           std::unordered_map<std::string, SII::Service> m_services;
    };
} //namespace SII
#endif //SERVICE_MANAGER_HPP
