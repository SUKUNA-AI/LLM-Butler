#include "butler/cli/commands.hpp"
#include "butler/fs/filesystem.hpp"

#include <iostream>
#include <string_view>
#include <system_error>

namespace butler::cli {

int run_init() {
    std::error_code ec;

    const auto root = butler::fs::root_dir(ec);

    if (ec || root.empty()) {
        std::cerr << "Could not resolve Butler root directory\n";
        return 1;
    }

    const bool exists = butler::fs::path_exists(root, ec);

    if (ec) {
        std::cerr << "Could not check Butler root directory: "
                  << root.string() << '\n';
        return 1;
    }

    if (exists) {
        std::cout << "Butler root directory already exists: "
                  << root.string() << '\n';
        return 1;
    }

    const bool created = butler::fs::create_directories(root, ec);

    if (ec || !created) {
        std::cerr << "Could not create Butler root directory: "
                  << root.string() << '\n';
        return 1;
    }

    std::cout << "Created Butler root directory: "
              << root.string() << '\n';

    const auto logs = butler::fs::logs_dir(ec);
    const auto artifacts = butler::fs::artifacts_dir(ec);
    const auto runtime = butler::fs::runtime_dir(ec);

    butler::fs::create_directories(logs, ec);
    butler::fs::create_directories(artifacts, ec);
    butler::fs::create_directories(runtime, ec);


    std::cout << "Created Butler base directories:\n"
              << "  - " << logs.string() << '\n'
              << "  - " << artifacts.string() << '\n'
              << "  - " << runtime.string() << '\n';

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
