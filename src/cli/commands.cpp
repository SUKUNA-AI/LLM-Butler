#include "butler/cli/commands.hpp"
#include <iostream>
#include <string_view>

namespace butler::cli {

int run_init()
{
    // Заглушка
    // создать рабочую директорию Butler
    // создать подпапки
    // создать config
    std::cout << "init не реализован\n";

    return 0;
}

int run_status()
{
    // Заглушка
    // Реализовать потом:
    // проверка инициализации
    // существование директорий
    // чтение конфига

    std::cout << "run_status не реализован\n";

    return 0;
}

void print_unknow_command(std::string_view command)
{
    std::cerr << "Unknow command: " << command << "\n";
    std::cerr << "Use 'butler --help' or 'butler help' to see available commands\n";
}

void print_help()
{
    // help look pretier
    std::cout << "\n"
                 "╔══════════════════════════════════════════════════════════╗\n"
                 "║                    GPU BUTLER v0.1                       ║\n"
                 "║         Local-first runtime manager for LLM backends     ║\n"
                 "╚══════════════════════════════════════════════════════════╝\n"
                 "\n"
                 "DESCRIPTION:\n"
                 "  Butler is a tool for managing and monitoring local LLM\n"
                 "  backends. It allows you to easily initialize workspaces,\n"
                 "  track status, and manage configurations.\n"
                 "\n"
                 "USAGE:\n"
                 "  butler [command] [options]\n"
                 "\n"
                 "COMMANDS:\n"
                 "  init              Initialize a new Butler workspace\n"
                 "                    Creates the necessary folder structure\n"
                 "                    and default configuration files\n"
                 "\n"
                 "  status            Show current Butler status\n"
                 "                    Displays information about the workspace,\n"
                 "                    active backends, and resource usage\n"
                 "\n"
                 "  help, --help      Show help message\n"
                 "                    Displays information about available\n"
                 "                    commands and their purposes\n"
                 "\n"
                 "EXAMPLES:\n"
                 "  butler init       # Create a new workspace\n"
                 "  butler status     # Check status\n"
                 "  butler --help     # Show help message\n"
                 "\n"
                 "MORE INFO:\n"
                 "  Visit https://github.com/SUKUNA-AI/LLM-Butler for documentation\n"
                 "\n";
}

int handle_commands(int argc, char* argv[])
{
    // пользователь запустил программу без команд
    if (argc <= 1) {
        print_help();
        return 0;
    }

    // argv[1] - это адрес по которому лежит команда
    const std::string_view command { argv[1] };

    if (command == "--help" || command == "help") {
        print_help();
        return 0;
    }

    if (command == "init") {
        return run_init();
    }

    if (command == "status") {
        return run_status();
    }

    print_unknow_command(command);
    return 1;
}
} // namespace butler::cli
