#include "service_manager.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>


#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "zenoh-pico.h"
}

namespace SII {
    serviceManager::serviceManager(const std::string& config_path) {
        std::ifstream json_config_stream(config_path);
        nlohmann::json config_json;
        json_config_stream >> config_json;
        for(const auto& service: config_json["services"]) {
            std::cerr << "Name:" << service["name"] << std::endl;
            m_services.emplace(service["name"],  SII::Service(service["name"]));
        }
    }

    serviceManager::~serviceManager(){
    }

    int serviceManager::Run(const std::string& router_address) {

        zn_properties_t *config = ::zn_config_default();
        if (router_address.size())
        {
            zn_properties_insert(config, ZN_CONFIG_PEER_KEY, z_string_make(router_address.c_str()));
        }

        zn_properties_insert(config, ZN_CONFIG_USER_KEY, z_string_make("user"));
        zn_properties_insert(config, ZN_CONFIG_PASSWORD_KEY, z_string_make("password"));

        printf("Openning session...\n");
        zn_session_t *s = zn_open(config);
        if (s == 0)
        {
            printf("Unable to open session!\n");
            exit(-1);
        }

        zn_properties_t *ps = zn_info(s);
        z_string_t prop = zn_properties_get(ps, ZN_INFO_PID_KEY);
        printf("info_pid : %.*s\n", (int)prop.len, prop.val);

        prop = zn_properties_get(ps, ZN_INFO_ROUTER_PID_KEY);
        printf("info_router_pid : %.*s\n", (int)prop.len, prop.val);

        zn_properties_free(ps);
        zn_close(s);
    
        return 0;
    }
} //namespace SII

