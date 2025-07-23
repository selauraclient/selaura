#pragma once
#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <print>

#include "command.hpp"
#include "impl/Help.hpp"
#include "impl/Enable.hpp"
#include "impl/Disable.hpp"

namespace selaura {

    constexpr std::array<selaura::command, 3> default_commands = {
        command{ "help", "Lists all available commands", &selaura::commands::help },
        command{ "enable", "Enable a module", &selaura::commands::enable },
        command{ "disable", "Disable a module", &selaura::commands::disable }
    };

    inline std::vector<command> command_list;

    struct command_handler {
        void register_command(command cmd) {
            command_list.push_back(std::move(cmd));
        }

        void init_cmd() {
            command_list.insert(command_list.end(), default_commands.begin(), default_commands.end());

            std::string line;
            while (true) {
                std::print("> ");
                if (!std::getline(std::cin, line)) break;

                std::istringstream iss(line);
                std::string input_command;
                iss >> input_command;

                if (input_command == "exit") break;

                bool found = false;
                for (const auto& cmd : command_list) {
                    if (cmd.name == input_command) {
                        cmd.handler(iss);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    std::println("Unknown command: {}", input_command);
                }
            }
        }
    };

}