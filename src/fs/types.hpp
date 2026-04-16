#pragma once

#include <filesystem>
#include <nlohmann/json.hpp> // for json file
#include <string>

namespace butler::fs {

using Path = std::filesystem::path;

enum class DirectoryStatus {
    missing,
    directory,
    not_directory,
};
} // namespace butler::fs

// =====================================
namespace butler::fs::ops {
struct FileOperationResult {

    bool result;
    std::string message;
};

enum class BootstrapStatus {
    complete, // 4/4 dirs + config.json
    partial, // 3/4 dirs + config.json
    missing, // < 3/4 dirs - config.json
};

enum class ConfigStatus {
    valid,
    invalid,
    missing,
};

struct SnapshotStatus {
    Path workspace_path;
    // Paths states
    DirectoryStatus root_dir;
    DirectoryStatus logs_dir;
    DirectoryStatus artifacts_dir;
    DirectoryStatus runtime_dir;
    // File state
    ConfigStatus config_file;
    //
    BootstrapStatus bootstrap_status;
};

} // namespace butler::fs::ops

// =================================================
namespace butler::fs::conf {

using json = nlohmann::json;

struct ButlerConfig {
    Path root_dir;
    Path logs_dir;
    Path artifacts_dir;
    Path runtime_dir;
    Path config_file;

    std::string log_level = "info";
    int api_port = 8080; // localhost
};

} // namespace butler::fs::conf

// ===================================================
