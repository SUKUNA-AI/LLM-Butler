#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>

namespace butler::fs {

// Делаем удобное короткое имя
using Path = std::filesystem::path;

enum class DirectoryStatus {
    missing,
    directory,
    not_directory,
};

// Возвращает домашнюю директорию пользователя
Path home_dir(std::error_code& ec);

// Возвращает root directory Butler
//
// Для V0 мы приняли политику:
// Butler живёт в ~/Butler
Path root_dir(std::error_code& ec);

Path logs_dir(std::error_code& ec);

Path artifacts_dir(std::error_code& ec);

Path runtime_dir(std::error_code& ec);

// Формирует понятный текст ошибки для пользователя
std::string format_error(
    std::string_view action,
    const Path& p,
    const std::error_code& ec);

} // namespace butler::fs
