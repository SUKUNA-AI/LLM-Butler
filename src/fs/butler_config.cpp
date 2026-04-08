#include "butler/fs/butler_config.hpp"

#include <format>
#include <fstream> // for create file
#include <nlohmann/json.hpp> // for json file
#include <system_error>

namespace butler::fs::conf {
ButlerConfig make_default_config()
{
    ButlerConfig conf;
    std::error_code ec;

    auto paths = butler::fs::AppPaths::build(ec);

    conf.root_dir = paths.root_dir();
    conf.logs_dir = paths.logs_dir();
    conf.artifacts_dir = paths.artifacts_dir();
    conf.runtime_dir = paths.runtime_dir();
    conf.config_file = paths.config_file();

    return conf;
}

butler::fs::ops::FileOperationResult create_default_config()
{
    ButlerConfig conf = make_default_config();
    butler::fs::ops::FileOperationResult op_file;
    std::error_code ec;

    // по сути класс AppPaths должен возвращать корректный root dir
    // но перепроверка не помешает
    if (!butler::fs::ops::is_directory(conf.root_dir, ec)) {
        op_file.result = false;
        op_file.message = "Root directory not created";
        return op_file;
    }

    nlohmann::json config;
    config["name"] = "LLM-Butler";
    config["version"] = 0.1;
    config["settings"] = {
        { "port", conf.api_port },
    };
    config["paths"] = {
        { "root", conf.root_dir },
        { "logs", conf.logs_dir },
        { "artifacts", conf.artifacts_dir },
        { "runtime", conf.runtime_dir },
        { "config file", conf.config_file }
    };

    Path conf_path = conf.config_file;
    // попытка создать файл
    std::ofstream file(conf_path);

    // удалось ли создать файл
    if (!file.is_open()) {
        op_file.result = false;
        op_file.message = "Failed to open/create `config.json` for writing";
        return op_file;
    }

    // запись данных
    file << config.dump(4);

    if (file.fail()) {
        op_file.result = false;
        op_file.message = "Error occured during writing to file\n";
    } else {
        op_file.result = true;
        op_file.message = std::format("  -- Config File is SUCCEFULLY CREATED. Path: {}\n", conf_path.string());
    }

    return op_file;
}

} // namespace butler::fs::conf
