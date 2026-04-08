#pragma once

#include "butler/fs/app_paths.hpp"
#include "butler/fs/filesystem_ops.hpp"

#include <format>
#include <fstream>
#include <nlohmann/json.hpp> // for json file
#include <string>
#include <system_error>

namespace butler::fs::conf {

struct ButlerConfig {
    Path root_dir;
    Path logs_dir;
    Path artifacts_dir;
    Path runtime_dir;
    Path config_file;

    std::string log_level = "info";
    int api_port = 8080; // localhost
};

// функция запоминает данные в структуру
ButlerConfig make_default_config();

// функция записывает данные и создает config.json
butler::fs::ops::FileOperationResult create_default_config();

} // namespace butler::fs::conf
