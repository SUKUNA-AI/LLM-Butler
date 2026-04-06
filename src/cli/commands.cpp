#include "butler/cli/commands.hpp"
#include "butler/fs/filesystem.hpp"
#include "butler/fs/filesystem_ops.hpp"

#include <iostream>
#include <string_view>
#include <system_error>

namespace butler::cli {

int run_init()
{
    auto res = butler::fs::ops::check_main_directories();

    if (!res.result) {
        std::cerr << res.message;
        return 1;
    }

    std::cout << res.message;
    return 0;
}

int run_status()
{
    auto res = butler::fs::ops::ensure_butler_inizialization();

    if (!res.result) {
        std::cerr << res.message;
        return 1;
    }

    std::cout << res.message;
    return 0;
}

void print_unknown_command(std::string_view command)
{
    std::cerr << "Unknown command: " << command << "\n";
    std::cerr << "Use 'butler --help' or 'butler help' to see available commands\n";
}

void print_help()
{
    std::cout << "\n"
                 "╔══════════════════════════════════════════════════════════╗\n"
                 "║                    GPU BUTLER v0.1                       ║\n"
                 "║         Local-first runtime manager for LLM backends     ║\n"
                 "╚══════════════════════════════════════════════════════════╝\n"
                 "\n"
                 "DESCRIPTION:\n"
                 "  Butler is a tool for managing and monitoring local LLM\n"
                 "  backends. It allows you to easily initialize workspaces\n"
                 "  and verify their base directory structure.\n"
                 "\n"
                 "USAGE:\n"
                 "  butler [command] [options]\n"
                 "\n"
                 "COMMANDS:\n"
                 "  init              Initialize a Butler workspace\n"
                 "                    Creates the root and base directories\n"
                 "                    in ~/Butler\n"
                 "\n"
                 "  status            Check Butler workspace status\n"
                 "                    Verifies root, logs, artifacts,\n"
                 "                    and runtime directories\n"
                 "\n"
                 "  help, --help      Show help message\n"
                 "                    Displays information about available\n"
                 "                    commands and their purposes\n"
                 "\n"
                 "EXAMPLES:\n"
                 "  ./build/butler init       # Create a new workspace\n"
                 "  ./build/butler status     # Check status\n"
                 "  ./build/butler --help     # Show help message\n"
                 "\n"
                 "MORE INFO:\n"
                 "  Visit https://github.com/SUKUNA-AI/LLM-Butler for documentation\n"
                 "\n";
}

int handle_commands(int argc, char* argv[])
{
    if (argc <= 1) {
        print_help();
        return 0;
    }

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

    print_unknown_command(command);
    return 1;
}
} // namespace butler::cli
