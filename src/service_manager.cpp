#include "service_manager.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>

namespace SII {
    serviceManager::serviceManager(const std::string& config_path) {
        std::ifstream json_config_stream(config_path);
        nlohmann::json config_json;
        json_config_stream >> config_json;
        for(const auto& service: config_json["services"]) {
            std::cerr << "Name:" << service["name"] << std::endl;
        }

    }

    serviceManager::~serviceManager(){
    }
} //namespace SII

