#include "butler/fs/filesystem.hpp"

#include <cstdlib> 
#include <sstream> 

namespace butler::fs{

Path home_dir(std::error_code& ec){
    ec.clear();

    const char* home = std::getenv("HOME");

    if (home == nullptr || *home == '\0'){
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return {};
    }

    return Path(home);
    
    }

Path root_dir(std::error_code& ec){
    ec.clear();

    const Path home = home_dir(ec);

    if(ec){return{};}

    return home / "Butler";
    }

bool path_exists(const Path& p, std::error_code& ec) {
    ec.clear();

    return std::filesystem::exists(p, ec);
    }

bool is_directory(const Path& p, std::error_code& ec){
    ec.clear();

    return std::filesystem::is_directory(p, ec);
    }

bool create_directories(const Path& p, std::error_code& ec){

    return std::filesystem::create_directories(p, ec);
    }


std::string format_error(
    std::string_view action,
    const Path& p,
    const std::error_code& ec
) {
    // ostringstream - это поток, в который удобно "собирать" строку по частям.
    std::ostringstream out;

    // Сначала описываем действие.
    out << action << " failed";

    // Если путь непустой - добавим его в сообщение.
    if (!p.empty()) {
        out << " for path: " << p.string();
    }

    // Если ошибка реально есть - добавляем её описание и код.
    if (ec) {
        out << " | error: " << ec.message();
        out << " (code: " << ec.value() << ")";
    }

    // Возвращаем готовую строку.
    return out.str();
}

} // namespace butler::fs