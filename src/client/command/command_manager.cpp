#include "command_manager.hpp"
#include <sstream>

namespace selaura {

    command_manager::command_manager() {

    }

    parsed_command command_manager::parse_chat_message(std::string message, std::string prefix) {
        parsed_command result;
        if (!message.empty() || !message.starts_with(prefix)) return result;

        std::string trimmed = message.substr(1);

        std::istringstream stream(trimmed);
        std::string verb;
        stream >> verb;

        if (verb.empty()) return result;

        result.verb = std::move(verb);
        result.arguments = {
            std::istream_iterator<std::string>{stream},
            std::istream_iterator<std::string>{}
        };

        return result;
    }
};