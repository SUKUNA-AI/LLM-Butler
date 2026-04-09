#pragma once

#include "butler/fs/app_paths.hpp"
#include "butler/fs/filesystem_ops.hpp"

#include <format>
#include <fstream>
#include <nlohmann/json.hpp> // for json file
#include <string>
#include <system_error>

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

// функция запоминает данные в структуру
ButlerConfig make_default_config(std::error_code& ec);

// функция проверяе// функция СОХРАНЯЕТ данные в json файл
json build_config_json(const ButlerConfig& conf);

// функция записывает создает config.json и проверяет целостность файла
butler::fs::ops::FileOperationResult create_default_config();

// функция загружает config.json
butler::fs::ops::FileOperationResult load_config();

} // namespace butler::fs::conf
