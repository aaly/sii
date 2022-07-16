#include "service_manager.hpp"
#include <deque>
#include <filesystem>

std::deque<std::string> default_config_paths = {"sii.config.json", "/usr/share/sii/sii.config.json"};

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

        SII::serviceManager sManager(selected_config_path);
    }
    catch (std::exception&) {
        std::cerr << "exception in main()" << std::endl;
}
    return 0;;
}
