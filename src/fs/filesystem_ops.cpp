#include "butler/fs/filesystem_ops.hpp"
#include "butler/fs/filesystem.hpp"

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

FileOperationResult ensure_butler_inizialization()
{
    FileOperationResult res;
    std::error_code ec;
    const auto root = root_dir(ec);
    if (ec || root.empty()) {
        res.message = "Could not resolve Butler root directory\n";
        res.result = false;
        return res;
    }

    auto dir_ready = ensure_directory_ready(root, "Butler root directory");
    if (!dir_ready.result) {
        res.message = dir_ready.message;
        res.result = false;
        return res;
    }

    const auto logs = butler::fs::logs_dir(ec);
    if (ec || logs.empty()) {
        res.message = "Could not resolve Butler logs directory\n";
        res.result = false;
        return res;
    }

    const auto artifacts = butler::fs::artifacts_dir(ec);
    if (ec || artifacts.empty()) {
        res.message = "Could not resolve Butler artifacts directory\n";
        res.result = false;
        return res;
    }

    const auto runtime = butler::fs::runtime_dir(ec);
    if (ec || runtime.empty()) {
        res.message = "Could not resolve Butler runtime directory\n";
        res.result = false;
        return res;
    }

    auto logs_dir = ensure_directory_ready(logs, "logs directory");
    auto artifacts_dir = ensure_directory_ready(artifacts, "artifacts directory");
    auto runtime_dir = ensure_directory_ready(runtime, "runtime directory");

    bool all_ready = true;
    all_ready &= logs_dir.result;
    all_ready &= artifacts_dir.result;
    all_ready &= runtime_dir.result;

    if (!all_ready) {
        res.message = "Butler initialization failed\n";
        res.result = false;
        return res;
    }

    res.message = std::format("Butler is initialized:\n{}{}{}", logs_dir.message, artifacts_dir.message, runtime_dir.message);
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
    const auto root = butler::fs::root_dir(ec);
    if (ec || root.empty()) {
        res.message = "Could not resolve Butler root directory\n";
        res.result = false;
        return res;
    }

    const auto logs = butler::fs::logs_dir(ec);
    if (ec || logs.empty()) {
        res.message = "Could not resolve Butler logs directory\n";
        res.result = false;
        return res;
    }

    const auto artifacts = butler::fs::artifacts_dir(ec);
    if (ec || artifacts.empty()) {
        res.message = "Could not resolve Butler artifacts directory\n";
        res.result = false;
        return res;
    }

    const auto runtime = butler::fs::runtime_dir(ec);
    if (ec || runtime.empty()) {
        res.message = "Could not resolve Butler runtime directory\n";
        res.result = false;
        return res;
    }

    res.message = "Butler workspace: " + root.string() + "\n";
    auto root_dir = report_directory_status("root", root);
    auto logs_dir = report_directory_status("logs", logs);
    auto artifacts_dir = report_directory_status("artifacts", artifacts);
    auto runtime_dir = report_directory_status("runtime", runtime);

    bool initialized = true;
    initialized &= root_dir.result;
    initialized &= logs_dir.result;
    initialized &= artifacts_dir.result;
    initialized &= runtime_dir.result;

    if (initialized) {
        res.message += std::format("Butler initialized:\n{}{}{}{}", root_dir.message, logs_dir.message, artifacts_dir.message, runtime_dir.message);
        res.result = true;
    } else {
        res.result = false;
        res.message += "Butler is not initialized\n";
    }

    return res;
}

} // namespace butler::fs::ops
