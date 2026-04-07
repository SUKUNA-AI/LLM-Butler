#include "butler/fs/filesystem_ops.hpp"
#include "butler/fs/filesystem.hpp"
#include "butler/fs/app_paths.hpp"

#include <format>
#include <sstream>
#include <string_view>
#include <system_error>

namespace butler::fs::ops {

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

    const bool directory = butler::fs::ops::is_directory(p, ec);
    if (ec) {
        return DirectoryStatus::missing;
    }

    return directory ? DirectoryStatus::directory : DirectoryStatus::not_directory;
}

FileOperationResult
print_directory_error(std::string_view action, const butler::fs::Path& path, const std::error_code& ec)
{
    FileOperationResult res;
    res.message = butler::fs::format_error(action, path, ec);
    res.result = false;
    return res;
}

FileOperationResult ensure_directory_ready(const butler::fs::Path& path, std::string_view label)
{
    FileOperationResult res;
    std::error_code ec;
    const auto status = get_directory_status(path, ec);

    if (ec) {
        return print_directory_error("Inspect directory", path, ec);
    }

    if (status == DirectoryStatus::directory) {
        res.message = std::format("  - {} already exists: {}\n", label, path.string());
        res.result = true;
        return res;
    }

    if (status == DirectoryStatus::not_directory) {
        res.message = std::format("  - {} exists but is not a directory: {}\n", label, path.string());
        res.result = false;
        return res;
    }

    const bool created = butler::fs::ops::create_directories(path, ec);
    if (ec) {
        return print_directory_error("Create directory", path, ec);
    }

    if (!created) {
        res.message = std::format("Could not create directory: {}\n", path.string());
        res.result = false;
        return res;
    }

    const auto verified_status = get_directory_status(path, ec);
    if (ec) {
        return print_directory_error("Verify directory", path, ec);
    }

    if (verified_status != DirectoryStatus::directory) {
        res.message = std::format("Directory was not created correctly: {}\n", path.string());
        res.result = false;
        return res;
    }

    res.message = std::format("{} created successfully\n", label);
    res.result = true;
    return res;
}

FileOperationResult ensure_butler_initialization()
{
    FileOperationResult res;
    std::error_code ec;

    const auto paths = butler::fs::AppPaths::build(ec);
    if (ec || !paths.valid()) {
        res.message = "Could not resolve Butler app paths\n";
        res.result = false;
        return res;
    }

    auto root = ensure_directory_ready(
        paths.root_dir(), "Butler root directory"
    );

    if (!root.result) {
        return root;
    }

    auto logs = ensure_directory_ready(
        paths.logs_dir(),
        "logs directory"
    );

    auto artifacts = ensure_directory_ready(
        paths.artifacts_dir(),
        "artifacts directory"
    );

    auto runtime = ensure_directory_ready(
        paths.runtime_dir(),
        "runtime directory"
    );

    // Собираем
    const bool all_ready =
        root.result &&
        logs.result &&
        artifacts.result &&
        runtime.result;

    if (!all_ready) {
        res.message =
            "Butler initialization failed\n" +
            root.message +
            logs.message +
            artifacts.message +
            runtime.message;
        res.result = false;
        return res;
    }

    // Всё хорошо - возвращаем общий успех
    res.message =
        "Butler is initialized:\n" +
        root.message +
        logs.message +
        artifacts.message +
        runtime.message;

    res.result = true;
    return res;
}

FileOperationResult report_directory_status(std::string_view label, const butler::fs::Path& path)
{
    FileOperationResult res;
    std::error_code ec;
    const auto status = get_directory_status(path, ec);

    if (ec) {
        return print_directory_error("Inspect directory", path, ec);
    }

    if (status == butler::fs::DirectoryStatus::directory) {
        res.message = std::format("  - {}: initialized ({})\n", label, path.string());
        res.result = true;
        return res;
    }

    if (status == butler::fs::DirectoryStatus::not_directory) {
        res.message = std::format("  - {}: invalid, path is not a directory ({})\n", label, path.string());
        res.result = false;
        return res;
    }

    res.message = std::format("  - {}: missing ({})\n", label, path.string());
    res.result = false;
    return res;
}

FileOperationResult check_main_directories()    
{
    FileOperationResult res;
    std::error_code ec;

    const auto paths = butler::fs::AppPaths::build(ec);

    if (ec || !paths.valid()) {
        res.message = "Could not resolve Butler app paths\n";
        res.result = false;
        return res;
    }

    auto root = report_directory_status("root", paths.root_dir());
    auto logs = report_directory_status("logs", paths.logs_dir());
    auto artifacts = report_directory_status("artifacts", paths.artifacts_dir());
    auto runtime = report_directory_status("runtime", paths.runtime_dir());

    const bool initialized =
        root.result &&
        logs.result &&
        artifacts.result &&
        runtime.result;

    res.message = "Butler workspace " + paths.root_dir().string() + "\n";

    if (initialized) {
        res.message +=
            "Butler initialized:\n" +
            root.message +
            logs.message +
            artifacts.message +
            runtime.message;
        res.result = true;
    } else {
        res.message +=
            "Butler is not initialized\n" +
            root.message +
            logs.message +
            artifacts.message +
            runtime.message;
        res.result = false;
    }

    return res;
}

} // namespace butler::fs::ops
