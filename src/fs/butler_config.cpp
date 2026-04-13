#include "butler/fs/butler_config.hpp"
#include "butler/fs/filesystem_ops.hpp"

#include <format>
#include <fstream> // for work with files
#include <nlohmann/json.hpp> // for json file
#include <system_error>

namespace butler::fs::conf {
ButlerConfig make_default_config(std::error_code& ec)
{
    ButlerConfig conf;

    auto paths = butler::fs::AppPaths::build(ec);

    conf.root_dir = paths.root_dir();
    conf.logs_dir = paths.logs_dir();
    conf.artifacts_dir = paths.artifacts_dir();
    conf.runtime_dir = paths.runtime_dir();
    conf.config_file = paths.config_file();

    return conf;
}

json build_config_json(const ButlerConfig& conf)
{
    json config;

    config["name"] = "LLM-Butler";
    config["version"] = 0.1;
    config["settings"] = { { "port", conf.api_port } };
    config["paths"] = {
        { "root", conf.root_dir },
        { "logs", conf.logs_dir },
        { "artifacts", conf.artifacts_dir },
        { "runtime", conf.runtime_dir },
        { "config_file", conf.config_file }
    };

    return config;
}

butler::fs::ops::FileOperationResult create_default_config()
{
    butler::fs::ops::FileOperationResult op_file;
    std::error_code ec;
    ButlerConfig conf = make_default_config(ec);

    if (ec) {
        op_file.result = false;
        op_file.message = std::format("Failed to resolve paths: {}\n", ec.message());
        return op_file;
    }

    Path conf_path = conf.config_file;

    // проверка чтобы НЕПЕРЕЗАПИСАТЬ уже существующи файл
    if (butler::fs::ops::file_exists(conf_path, ec)) {
        op_file.result = true;
        op_file.message = std::format("Config already exists: {}\n", conf_path.string());
        return op_file;
    }

    json expected = build_config_json(conf);

    // scope для соблюдения принципов RAII
    // (ну и чтобы файл закрылся сразу после использования)
    {
        std::ofstream file(conf_path);
        if (!file.is_open()) {
            op_file.result = false;
            op_file.message = "Failed to open config.json for writing\n";
            return op_file;
        }

        file << expected.dump(4);

        if (file.fail()) {
            op_file.result = false;
            op_file.message = "Failed to write config.json\n";
            return op_file;
        }

    } // scope

    op_file.result = true;
    op_file.message = std::format("Config created: {}\n", conf_path.string());
    return op_file;
}

butler::fs::ops::FileOperationResult validate_config_file(const Path& conf_path)
{
    std::error_code ec;
    ButlerConfig conf = make_default_config(ec);
    butler::fs::ops::FileOperationResult op_file;

    if (ec) {
        op_file.result = false;
        op_file.message = std::format("Validation failed: cannot resolve paths: {}", ec.message());
        return op_file;
    }
    json expected = build_config_json(conf);

    // сравнение записанных данных с "эталоном"
    std::ifstream verify(conf_path);
    if (!verify.is_open()) {
        op_file.result = false;
        op_file.message = "Validation failed: cannot open config file\n";
        return op_file;
    }

    try {
        json actual = json::parse(verify);
        if (actual != expected) {
            op_file.result = false;
            op_file.message = "Validation failed: config content mismatch\n";
            return op_file;
        }
    } catch (const json::parse_error& e) {
        op_file.result = false;
        op_file.message = std::format("Validation failed: invalid JSON: {}\n", e.what());
        return op_file;
    }

    op_file.result = true;
    op_file.message = "Config validation passed\n";
    return op_file;
}

Path get_config_path(std::error_code& ec)
{
    ButlerConfig conf = make_default_config(ec);
    return conf.config_file;
}

butler::fs::ops::FileOperationResult load_config()
{
    std::error_code ec;
    Path conf_path = get_config_path(ec);
    butler::fs::ops::FileOperationResult op_file;
    op_file.message = "Files: \n";

    if (ec) {
        op_file.result = false;
        op_file.message += std::format("Failed to resolve paths: {}", ec.message());
        return op_file;
    }

    if (!butler::fs::ops::file_exists(conf_path, ec)) {
        op_file.result = false;
        op_file.message += std::format("  -- File `config.json` not found at: {}\n", conf_path.string());
        return op_file;
    }

    // строгая проверка содержимого чтобы отобразить ошибки в status
    auto validation = validate_config_file(conf_path);
    if (!validation.result) {
        return validation;
    }

    op_file.result = true;
    op_file.message += std::format("  -- File `config.json` loaded & validated: {}\n", conf_path.string());
    return op_file;
}

} // namespace butler::fs::conf
