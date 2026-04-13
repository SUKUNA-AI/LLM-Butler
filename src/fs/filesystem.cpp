#include "butler/fs/filesystem.hpp"

#include <cstdlib>
#include <sstream>

namespace butler::fs {

Path home_dir(std::error_code& ec)
{
    ec.clear();

    const char* home = std::getenv("HOME");
    if (home == nullptr || *home == '\0') {
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return {};
    }

    return Path(home);
}

Path root_dir(std::error_code& ec)
{
    ec.clear();

    const auto home = home_dir(ec);
    if (ec || home.empty()) {
        return {};
    }

    return home / "Butler";
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
