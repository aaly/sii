#ifndef SERVICE_HPP
#define SERVICE_HPP
#include <string>

namespace SII {
    class Service{
        public:
            Service(const std::string& name);
        private:
            std::string name;
    };
} //namespace SII
#endif //SERVICE_HPP
