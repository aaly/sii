#include "service_manager.hpp"
#include <deque>
#include <filesystem>

std::deque<std::string> default_config_paths = {
    "sii.config.json", "/usr/share/sii/sii.config.json"
};

int main() {
    std::string selected_config_path;

    try {
        default_config_paths.push_front(SII::getHomeDirectory()+ "/.sii.config.json");
        std::cerr << "home path:" << default_config_paths[0] << std::endl;

        for(const auto& path: default_config_paths) {
            if(std::filesystem::exists(path)) {
                selected_config_path = path;
                break;
            }
        }

        if(selected_config_path.size()) {
            SII::serviceManager sManager(selected_config_path);
//            sManager.Run("137.204.57.224");
              sManager.Run();
              std::cerr << "Done running" << std::endl;
        }
        else {
            throw std::runtime_error("Could not find any config file");
        }
    }
    catch (std::exception& e) {
        std::cerr << "exception in main(): " << e.what() << std::endl;
    }

    return 0;;
}
