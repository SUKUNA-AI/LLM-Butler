#include "butler/fs/filesystem.hpp"

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <system_error>

namespace butler::fs {

Path home_dir(std::error_code& ec)
{
    ec.clear();

    const char* home = std::getenv("HOME");

    if (home == nullptr || *home == '\0') {
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return { };
    }

    return Path(home);
}

Path root_dir(std::error_code& ec)
{
    ec.clear();

    const Path home = home_dir(ec);

    if (ec) {
        return { };
    }

    return home / "Butler";
}

bool path_exists(const Path& p, std::error_code& ec)
{
    ec.clear();

    return std::filesystem::exists(p, ec);
}

bool is_directory(const Path& p, std::error_code& ec)
{
    ec.clear();

    return std::filesystem::is_directory(p, ec);
}

bool create_directories(const Path& p, std::error_code& ec)
{
    ec.clear();

    return std::filesystem::create_directories(p, ec);
}

DirectoryStatus get_directory_status(const Path& p, std::error_code& ec)
{
    ec.clear();

    const bool exists = path_exists(p, ec);
    if (ec) {
        return DirectoryStatus::missing;
    }

    if (!exists) {
        return DirectoryStatus::missing;
    }

    const bool directory = butler::fs::is_directory(p, ec);
    if (ec) {
        return DirectoryStatus::missing;
    }

    return directory ? DirectoryStatus::directory : DirectoryStatus::not_directory;
}

Path logs_dir(std::error_code& ec)
{
    ec.clear();

    const auto root = root_dir(ec);

    if (ec) {
        return { };
    }

    return root / "logs";
}

Path artifacts_dir(std::error_code& ec)
{
    ec.clear();

    const auto root = root_dir(ec);

    if (ec) {
        return { };
    }

    return root / "artifacts";
}

Path runtime_dir(std::error_code& ec)
{
    ec.clear();

    const auto root = root_dir(ec);

    if (ec) {
        return { };
    }

    return root / "runtime";
}

std::string format_error(
    std::string_view action,
    const Path& p,
    const std::error_code& ec)
{
    std::ostringstream out;

    out << action << " failed";

    if (!p.empty()) {
        out << " for path: " << p.string();
    }

    if (ec) {
        out << " | error: " << ec.message();
        out << " (code: " << ec.value() << ")";
    }

    return out.str();
}
} // namespace butler::fs
