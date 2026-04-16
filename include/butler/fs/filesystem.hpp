#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>

#include "butler/fs/types.hpp"

namespace butler::fs {

using Path = std::filesystem::path;

// Возвращает домашнюю директорию пользователя
Path home_dir(std::error_code& ec);

// Возвращает root directory Butler
//
// Для V0 мы приняли политику:
// Butler живёт в ~/Butler
Path root_dir(std::error_code& ec);

// Формирует понятный текст ошибки для пользователя
std::string format_error(
    std::string_view action,
    const Path& p,
    const std::error_code& ec);

} // namespace butler::fs
