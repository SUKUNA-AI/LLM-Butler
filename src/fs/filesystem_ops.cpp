#include "butler/fs/filesystem_ops.hpp"
#include "butler/fs/app_paths.hpp"
#include "butler/fs/filesystem.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <string_view>
#include <system_error>

namespace butler::fs::ops {

bool path_exists(const Path& p, std::error_code& ec)
{
    ec.clear();

    return std::filesystem::exists(p, ec);
}

bool file_exists(const Path& p, std::error_code& ec)
{
    ec.clear();

    return std::filesystem::is_regular_file(p, ec);
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
    // возвращает статус директории основываясь на полях enum class
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

FileOperationResult print_directory_error(std::string_view action, const butler::fs::Path& path, const std::error_code& ec)
{
    FileOperationResult res;
    res.message = butler::fs::format_error(action, path, ec);
    res.result = false;
    return res;
}

FileOperationResult ensure_directory_ready(const butler::fs::Path& path, std::string_view label)
{
    /*
     * сначала фунция проверяет путь и существование директории
     * затем: в лучшем случае -  создает необходимую дирекотрию
     *        в худшем случае - выдает ошибку
     * */
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

    res.message = std::format("  - {} created successfully. Path: {}\n", label, path.string());
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
        paths.root_dir(), "Butler root directory");

    if (!root.result) {
        return root;
    }

    auto logs = ensure_directory_ready(
        paths.logs_dir(),
        "logs directory");

    auto artifacts = ensure_directory_ready(
        paths.artifacts_dir(),
        "artifacts directory");

    auto runtime = ensure_directory_ready(
        paths.runtime_dir(),
        "runtime directory");

    // Собираем
    const bool all_ready = root.result && logs.result && artifacts.result && runtime.result;

    if (!all_ready) {
        res.message = "Butler initialization failed\n" + root.message + logs.message + artifacts.message + runtime.message;
        res.result = false;
        return res;
    }

    // Всё хорошо - возвращаем общий успех
    res.message = "MAIN directories are initialized:\n" + root.message + logs.message + artifacts.message + runtime.message;
    res.result = true;
    return res;
}

FileOperationResult report_directory_status(std::string_view label, const butler::fs::Path& path)
{
    // функция предоставляет информацию о состоянии директории:
    //      создана, несоздана, пропущена
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
    // функция которая нужна в status для проверки корректности
    //                                     созданных директорий
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

    const bool initialized = root.result && logs.result && artifacts.result && runtime.result;

    res.message += "Butler workspace at: " + paths.root_dir().string() + "\n\n";

    if (initialized) {
        res.message += "MAIN directories initialized:\n" + root.message + logs.message + artifacts.message + runtime.message;
        res.result = true;
    } else {
        res.message += "MAIN directories are not initialized\n" + root.message + logs.message + artifacts.message + runtime.message;
        res.result = false;
    }

    return res;
}

BootstrapStatus compute_bootstrap_status(const SnapshotStatus& snap)
{
    int count_dir { 0 };
    bool workspace { false };
    bool conf_file { false };

    if (snap.workspace_path.empty())
        return BootstrapStatus::missing;
    else
        workspace = true;

    if (!(snap.root_dir == butler::fs::DirectoryStatus::missing)) {
        count_dir += 1;
    }

    if (!(snap.logs_dir == butler::fs::DirectoryStatus::missing)) {
        count_dir += 1;
    }

    if (!(snap.artifacts_dir == butler::fs::DirectoryStatus::missing)) {
        count_dir += 1;
    }

    if (!(snap.runtime_dir == butler::fs::DirectoryStatus::missing)) {
        count_dir += 1;
    }

    if (!(snap.config_file == ConfigStatus::not_created)) {
        conf_file = true;
    }

    if (count_dir == 4 && conf_file && workspace) {
        return BootstrapStatus::complete;
    } else if (count_dir < 4 && conf_file && workspace) {
        return BootstrapStatus::partial;
    } else {
        return BootstrapStatus::missing;
    }
}

std::string format_dir_status(DirectoryStatus status)
{
    switch (status) {
    case DirectoryStatus::directory:
        return "initialized";
    case DirectoryStatus::not_directory:
        return "invalid (not a directory)";
    case DirectoryStatus::missing:
        return "missing";
    default:
        return "unknows";
    }
}

std::string format_file_status(ConfigStatus status)
{
    switch (status) {
    case ConfigStatus::created:
        return "initialized";
    case ConfigStatus::not_created:
        return "not initialized";
    default:
        return "unknows";
    }
}

std::string format_bootstrap_status(BootstrapStatus status)
{
    switch (status) {
    case BootstrapStatus::complete:
        return "bootstrap = complete. (That's mean successfully initialized)";
    case BootstrapStatus::partial:
        return "bootstrap = partial. (That's mean not initialized yet)";
    case BootstrapStatus::missing:
        return "bootstrap = missing. (That's mean u should run init)";
    default:
        return "unknows";
    }
}

std::string render_status_snapshot(const SnapshotStatus& snap, const butler::fs::AppPaths& paths)
{
    std::string message { "" };

    // workspace_path
    message += std::format("Butler workspace at: {}\n", snap.workspace_path.string());

    // Main dirs
    message += "\nMAIN directories:\n";
    message += std::format("  - root:      {} at {}\n", format_dir_status(snap.root_dir), paths.root_dir().string());
    message += std::format("  - logs:      {} at {}\n", format_dir_status(snap.logs_dir), paths.logs_dir().string());
    message += std::format("  - artifacts: {} at {}\n", format_dir_status(snap.artifacts_dir), paths.artifacts_dir().string());
    message += std::format("  - runtime:   {} at {}\n", format_dir_status(snap.runtime_dir), paths.runtime_dir().string());

    // Config
    message += "\nConfig:\n";
    message += std::format("  -- config.json {} at {}\n", format_file_status(snap.config_file), paths.config_file().string());

    // Bootstrap
    message += "\nBootstrap:\n";
    message += std::format("  - {}\n", format_bootstrap_status(snap.bootstrap_status));

    return message;
}

SnapshotStatus build_status_snapshot()
{
    std::error_code ec;
    SnapshotStatus snapshot;

    auto paths = butler::fs::AppPaths::build(ec);
    if (ec || !paths.valid()) {
        snapshot.workspace_path = "";
        snapshot.root_dir = DirectoryStatus::missing;
        snapshot.logs_dir = DirectoryStatus::missing;
        snapshot.artifacts_dir = DirectoryStatus::missing;
        snapshot.runtime_dir = DirectoryStatus::missing;
        snapshot.config_file = ConfigStatus::not_created;
        snapshot.bootstrap_status = BootstrapStatus::missing;
        return snapshot;
    }

    snapshot.workspace_path = home_dir(ec);
    snapshot.root_dir = get_directory_status(paths.root_dir(), ec);
    snapshot.logs_dir = get_directory_status(paths.logs_dir(), ec);
    snapshot.artifacts_dir = get_directory_status(paths.artifacts_dir(), ec);
    snapshot.runtime_dir = get_directory_status(paths.runtime_dir(), ec);

    bool config_exists = file_exists(paths.config_file(), ec);
    snapshot.config_file = config_exists ? ConfigStatus::created : ConfigStatus::not_created;

    snapshot.bootstrap_status = compute_bootstrap_status(snapshot);

    return snapshot;
}

} // namespace butler::fs::ops
