#include "service_manager.hpp"
#include <fstream>
#include <iostream>


#include <stdio.h>
#include <stdlib.h>
#include <thread>
namespace SII {
    serviceManager::serviceManager(const std::string& config_path) {
        std::ifstream json_config_stream(config_path);
        nlohmann::json config_json;
        json_config_stream >> config_json;
        for(const auto& service: config_json["services"]) {
            std::cerr << "Name:" << service["name"] << std::endl;
            m_services.emplace(service["name"],  SII::Service(service["name"], service["exe"]));
        }

        m_eventHandlers["run"] = [&](const json message_json) {
            std::cerr << "in call backkkkkkk" << std::endl;
            const std::string service_name = message_json["name"];
            if(m_services.count(service_name)) {
                 const std::vector<std::string> service_parameters = message_json["parameters"];
                 auto service = m_services[service_name];
                 service.Execute(service_parameters);
            }
            else {
                std::cerr << "Received unknown service: " << service_name << std::endl;
            }
        };

    }

    serviceManager::~serviceManager(){
    }

    void serviceManager::dataHandler(const zn_sample_t *sample, const void *arg)
    {
        if(arg != nullptr && sample != nullptr) {
            SII::serviceManager* sManager = (SII::serviceManager*)(arg);
            const std::string path((const char*)(sample->key.val), sample->key.len);
            const std::string message((const char*)(sample->value.val), sample->value.len);
            std::cerr << "ID: " << std::this_thread::get_id() << std::endl;
            std::cerr << "RECIEVED MESSAGE:" << message << std::endl;
            auto message_json = nlohmann::json::parse(message);
            sManager->Process(message_json);
        }
    }

    void serviceManager::Process(const json& message_json) {
        const std::string command = message_json["command"];
        std::cerr << "COMMAND:" << command << std::endl;
        if(m_eventHandlers.count(command)) {
            m_threadPool.queueWork(m_eventHandlers[command], message_json);
        }
        else {
            std::cerr << "Received unknown command: " << command << std::endl;
        }
    }

    int serviceManager::Run(const std::string& router_address, const std::string& topic_path) {
        zn_properties_t *config = zn_config_default();
        if(router_address.size()) {
            zn_properties_insert(config, ZN_CONFIG_PEER_KEY, z_string_make(router_address.c_str()));
        }

        printf("Openning session...\n");
        zn_session_t *s = zn_open(config);
        if (s == 0)
        {
            throw std::runtime_error("Unable to open session!\n");
        }

        // Start the read session session lease loops
        znp_start_read_task(s);
        znp_start_lease_task(s);
    
        printf("Declaring Subscriber on '%s'...\n", topic_path.c_str());
        zn_subscriber_t *sub = zn_declare_subscriber(s, zn_rname(topic_path.c_str()), zn_subinfo_default(), dataHandler, this);
        if (sub == 0)
        {
            throw std::runtime_error("Unable to declare subscriber.\n");
        }

        char c = 0;
        while (c != 'q')
        {
            c = fgetc(stdin);
        }
    
        zn_undeclare_subscriber(sub);
        zn_close(s);
    
        return 0;
    }
} //namespace SII

