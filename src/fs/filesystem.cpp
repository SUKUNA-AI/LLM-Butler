#include "butler/fs/filesystem.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream> // for check_path
#include <sstream>
#include <system_error>

namespace butler::fs {

Path home_dir(std::error_code& ec)
{
    // error_code - альтернатива try catch, но без остановки программы,
    // а для записи данных в ec
    ec.clear();

    // getenv - возвращает путь к домашней папке пользователя
    const char* home = std::getenv("HOME");

    if (home == nullptr || *home == '\0') {
        ec = std::make_error_code(std::errc::no_such_file_or_directory);
        return { }; // инициализация по умолчанию для типа Path
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

    return std::filesystem::create_directories(p, ec);
}

bool ensure_directory_exists(const Path& root, std::string_view dir, std::error_code& ec)
{
    // возврат функции неспроста bool - может пригодиться в будущем
    ec.clear();
    Path full_path = root / dir;

    const bool path = path_exists(full_path, ec);
    if (!path) {
        std::cout << "  - '" << dir << "' directory not initializated\n";
        return false;
    }

    // вызывать те же функции из namespace butler::fs затратнее
    const bool directory = std::filesystem::is_directory(full_path, ec);
    // если по какому-то чудо вместо папок создалось что-то другое
    if (!directory) {
        std::filesystem::create_directories(full_path, ec);
    }

    std::cout << "  - '" << dir << "' directory initializated. Path: " << full_path << "\n";
    return true;
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
