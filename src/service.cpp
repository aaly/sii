#include "service.hpp"
#include <memory>
#include <array>
#include <stdexcept>
#include <iostream>
#include <thread>

namespace SII {

    Service::Service(const std::string& name, const std::string& exe) : name(name), exe(exe) {
        
    }

    Service::Service() {
    }

    Service::~Service() {
    }

    Service& Service::operator=(const Service& copy) {
        name = copy.name;
        exe = copy.exe;
        return *this;
    }

    Service::Service(const Service& copy) : name(copy.name), exe(copy.exe){
    }       
    

    Service::Service(Service&& s) {
        name = std::move(s.name);
        exe = std::move(s.exe);
    }


    std::optional<std::string> Service::Execute(const std::vector<std::string>& parameters) {
        std::optional<std::string> result = std::nullopt;
        std::array<char, 128> buffer;
        std::string result_str;
        std::cerr << "Executing:" << exe << " From:" << std::this_thread::get_id() << std::endl;
        /*std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(exe.c_str(), nullptr), pclose);

        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result_str += buffer.data();
        }*/

        result_str = Command::exec(exe).output;
        std::cerr << "result:" << result_str << std::endl;

        result = result_str;

        return result;
    }
} //namespace SII

