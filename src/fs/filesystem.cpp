#include "butler/fs/filesystem.hpp"
#include "butler/fs/filesystem_ops.hpp"

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
