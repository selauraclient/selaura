#pragma once
#include <memory>
#include <string>
#include <vector>

#include "command.hpp"

namespace selaura {
    using command_types = std::tuple<

    >;

    struct parsed_command {
        std::string verb;
        std::vector<std::string> arguments;
    };

    struct command_manager {
        command_manager();

        template <typename command_t, typename... args_t>
        auto& register_cmd(args_t... args) {
            return commands.emplace_back(std::make_unique<command_t>(args...));
        }

        [[nodiscard]] std::vector<std::shared_ptr<command>> get_commands() const {
            return commands;
        }

        parsed_command parse_chat_message(std::string message, std::string prefix = ".");
    private:
        std::vector<std::shared_ptr<command>> commands{};
        command_types default_commands;
    };
};