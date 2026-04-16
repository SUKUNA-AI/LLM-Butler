#pragma once

#include "butler/fs/app_paths.hpp"
#include "butler/fs/types.hpp"

#include <format>
#include <fstream>
#include <nlohmann/json.hpp> // for json file
#include <string>
#include <system_error>

namespace butler::fs::conf {

using json = nlohmann::json;

// функция запоминает данные в структуру
ButlerConfig make_default_config(std::error_code& ec);

// функция проверяе// функция СОХРАНЯЕТ данные в json файл
json build_config_json(const ButlerConfig& conf);

// функция создает config.json
butler::fs::ops::FileOperationResult create_default_config();

// функция проверяет корректность записанного config.json
butler::fs::ops::FileOperationResult validate_config_file(const Path& conf_path);

// функция является основным звеном файла, она проверяет, обрабытывает
// и дает сведения о config.json
butler::fs::ops::FileOperationResult load_config();

} // namespace butler::fs::conf
