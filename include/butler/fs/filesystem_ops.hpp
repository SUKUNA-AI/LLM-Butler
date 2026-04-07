#pragma once

#include "butler/fs/filesystem.hpp"

#include <filesystem>
#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

// filesystem operations
namespace butler::fs::ops {

struct FileOperationResult {
    bool result;
    std::string message;
};
// сообщает об ошибке пользователя со всеми подробностоями
FileOperationResult print_directory_error(std::string_view action, const butler::fs::Path& path, const std::error_code& ec);

// включает множественную проверку переданной директории
FileOperationResult ensure_directory_ready(const butler::fs::Path& path, std::string_view label);

// сообщает о статусе состояния Butler(root) и его подпапкок: logs, artifacts, runtime директорий
FileOperationResult check_main_directories();

// проверяет инициализацию Butler (и подпапок), если нет то создает
FileOperationResult ensure_butler_initialization();

// сообщает о статусе директории (существует/несуществует/не ициализирована)
FileOperationResult report_directory_status(std::string_view label, const butler::fs::Path& path);

// ---------------------------
// Проверка существования пути.
bool path_exists(const Path& p, std::error_code& ec);

// Проверка, что путь существует именно как директория
bool is_directory(const Path& p, std::error_code& ec);

bool create_directories(const Path& p, std::error_code& ec);

// Возвращает текущее состояние директории, не изменяя файловую систему.
DirectoryStatus get_directory_status(const Path& p, std::error_code& ec);

} // namespace butler::fs::ops
