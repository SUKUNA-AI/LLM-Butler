#pragma once

#include <filesystem>
#include <string>
#include <filesystem>
#include <system_error>

namespace butler::fs {

// Делаем удобное короткое имя
using Path = std::filesystem::path;

// Возвращает домашнюю директорию пользователя

Path home_dir(std::error_code& ec);

// Возвращает root directory Butler
//
// Для V0 мы приняли политику:
// Butler живёт в ~/.butler

Path root_dir(std::error_code& ec);


// Проверка существования пути.
bool path_exists(const Path& p, std::error_code& ec);

// Проверка, что путь существует именно как директория
bool is_directory(const Path& p, std::error_code& ec);

bool create_directories(const Path& p, std::error_code& ec);

// Формирует понятный текст ошибки для пользователя
std::string format_error(
    std::string_view action,
    const Path& p,
    const std::error_code& ec
);

} //namespace butler::fs