#include "butler/cli/commands.hpp"
#include "butler/fs/filesystem.hpp"

#include <iostream>
#include <string_view>
#include <system_error>

namespace butler::cli {
namespace {

bool print_directory_error(std::string_view action, const butler::fs::Path& path, const std::error_code& ec)
{
    std::cerr << butler::fs::format_error(action, path, ec) << '\n';
    return false;
}

bool ensure_directory_ready(const butler::fs::Path& path, std::string_view label)
{
    std::error_code ec;
    const auto status = butler::fs::get_directory_status(path, ec);

    if (ec) {
        return print_directory_error("Inspect directory", path, ec);
    }

    if (status == butler::fs::DirectoryStatus::directory) {
        std::cout << label << " already exists: " << path.string() << '\n';
        return true;
    }

    if (status == butler::fs::DirectoryStatus::not_directory) {
        std::cerr << label << " exists but is not a directory: "
                  << path.string() << '\n';
        return false;
    }

    const bool created = butler::fs::create_directories(path, ec);
    if (ec) {
        return print_directory_error("Create directory", path, ec);
    }

    if (!created) {
        std::cerr << "Could not create directory: " << path.string() << '\n';
        return false;
    }

    const auto verified_status = butler::fs::get_directory_status(path, ec);
    if (ec) {
        return print_directory_error("Verify directory", path, ec);
    }

    if (verified_status != butler::fs::DirectoryStatus::directory) {
        std::cerr << "Directory was not created correctly: "
                  << path.string() << '\n';
        return false;
    }

    std::cout << "Created " << label << ": " << path.string() << '\n';
    return true;
}

bool report_directory_status(std::string_view label, const butler::fs::Path& path)
{
    std::error_code ec;
    const auto status = butler::fs::get_directory_status(path, ec);

    if (ec) {
        print_directory_error("Inspect directory", path, ec);
        return false;
    }

    if (status == butler::fs::DirectoryStatus::directory) {
        std::cout << "  - " << label << ": initialized (" << path.string() << ")\n";
        return true;
    }

    if (status == butler::fs::DirectoryStatus::not_directory) {
        std::cout << "  - " << label << ": invalid, path is not a directory ("
                  << path.string() << ")\n";
        return false;
    }

    std::cout << "  - " << label << ": missing (" << path.string() << ")\n";
    return false;
}

} // namespace

int run_init()
{
    std::error_code ec;

    const auto root = butler::fs::root_dir(ec);

    if (ec || root.empty()) {
        std::cerr << "Could not resolve Butler root directory\n";
        return 1;
    }

    if (!ensure_directory_ready(root, "Butler root directory")) {
        return 1;
    }

    const auto logs = butler::fs::logs_dir(ec);
    if (ec || logs.empty()) {
        std::cerr << "Could not resolve Butler logs directory\n";
        return 1;
    }

    const auto artifacts = butler::fs::artifacts_dir(ec);
    if (ec || artifacts.empty()) {
        std::cerr << "Could not resolve Butler artifacts directory\n";
        return 1;
    }

    const auto runtime = butler::fs::runtime_dir(ec);
    if (ec || runtime.empty()) {
        std::cerr << "Could not resolve Butler runtime directory\n";
        return 1;
    }

    bool all_ready = true;
    all_ready = ensure_directory_ready(logs, "logs directory") && all_ready;
    all_ready = ensure_directory_ready(artifacts, "artifacts directory") && all_ready;
    all_ready = ensure_directory_ready(runtime, "runtime directory") && all_ready;

    if (!all_ready) {
        std::cerr << "Butler initialization failed\n";
        return 1;
    }

    std::cout << "Butler is initialized\n";
    return 0;
}

int run_status()
{
    std::error_code ec;
    const auto root = butler::fs::root_dir(ec);

    if (ec || root.empty()) {
        std::cerr << "Could not resolve Butler root directory\n";
        return 1;
    }

    const auto logs = butler::fs::logs_dir(ec);
    if (ec || logs.empty()) {
        std::cerr << "Could not resolve Butler logs directory\n";
        return 1;
    }

    const auto artifacts = butler::fs::artifacts_dir(ec);
    if (ec || artifacts.empty()) {
        std::cerr << "Could not resolve Butler artifacts directory\n";
        return 1;
    }

    const auto runtime = butler::fs::runtime_dir(ec);
    if (ec || runtime.empty()) {
        std::cerr << "Could not resolve Butler runtime directory\n";
        return 1;
    }

    std::cout << "Butler workspace: " << root.string() << '\n';

    bool initialized = true;
    initialized = report_directory_status("root", root) && initialized;
    initialized = report_directory_status("logs", logs) && initialized;
    initialized = report_directory_status("artifacts", artifacts) && initialized;
    initialized = report_directory_status("runtime", runtime) && initialized;

    if (initialized) {
        std::cout << "Butler is initialized\n";
    } else {
        std::cout << "Butler is not initialized\n";
    }

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
