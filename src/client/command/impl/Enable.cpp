#include "Enable.hpp"
#include "../command_handler.hpp"
#include "../../client.hpp"

namespace selaura::commands {
    void enable(std::istringstream& iss) {
        std::string module_search;
        iss >> module_search;

        if (!module_search.empty()) {
            auto& feature_manager = selaura::get()->get<selaura::feature_manager>();
            feature_manager.for_each([&module_search]<typename T>(std::shared_ptr<T>&f) {
                std::string module_name = selaura::feature_traits<T>::name.c_str();
                spdlog::info("Search: {}", module_name);
                if (module_name == module_search) {
                    std::println("Enabled module {}", module_name);
                    f->set_enabled(true);
                }
            });
        }
        else {
            std::println("Usage: enable <ModuleName>");
        }
    }
};