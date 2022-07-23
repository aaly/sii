#include "service_manager.hpp"
#include <fstream>
#include <iostream>
#include <thread>

namespace SII {
       std::vector<size_t> get_cpu_times() {
        std::ifstream proc_stat("/proc/stat");
        proc_stat.ignore(5, ' '); // Skip the 'cpu' prefix.
        std::vector<size_t> times;
        for (size_t time; proc_stat >> time; times.push_back(time)) {
            ;
        }
          return times;
       }
  
    bool get_cpu_times(size_t &idle_time, size_t &total_time) {
          const std::vector<size_t> cpu_times = get_cpu_times();
          if (cpu_times.size() < 4)
              return false;
          idle_time = cpu_times[3];
          total_time = std::accumulate(cpu_times.begin(), cpu_times.end(), 0);
          return true;
      }


    serviceManager::serviceManager(const std::string& config_path) {
        std::cerr << "opening config:" << config_path << std::endl;
        std::ifstream json_config_stream(config_path);
        nlohmann::json config_json;
        json_config_stream >> config_json;
        for(const auto& service: config_json["services"]) {
            std::cerr << "Name:" << service["name"] << std::endl;
            m_services.emplace(service["name"],  SII::Service(service["name"], service["exe"]));
        }

        m_eventHandlers["run"] = [&](const json message_json) {
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
            m_threadPool.setRouterAddress(router_address);
        }

        printf("Openning session...\n");
        m_zenoh_session = zn_open(config);
        if (m_zenoh_session == 0)
        {
            throw std::runtime_error("Unable to open session!\n");
        }

        // Start the read session session lease loops
        znp_start_read_task(m_zenoh_session);
        znp_start_lease_task(m_zenoh_session);
    
        printf("Declaring Subscriber on '%s'...\n", topic_path.c_str());
        zn_subscriber_t *sub = zn_declare_subscriber(m_zenoh_session, zn_rname(topic_path.c_str()), zn_subinfo_default(), dataHandler, this);
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
        zn_close(m_zenoh_session);
    
        return 0;
    }
} //namespace SII

