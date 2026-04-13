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
    std::error_code ec;
    ButlerConfig conf = make_default_config(ec);
    butler::fs::ops::FileOperationResult op_file;

    if (ec) {
        op_file.result = false;
        op_file.message = std::format("Failed to resolve paths: {}", ec.message());
        return op_file;
    }

    Path conf_path = conf.config_file;
    // есть ли уже такой файл
    if (butler::fs::ops::path_exists(conf_path, ec)) {
        op_file.result = true;
        op_file.message = std::format("  -- File `config.json` exists. Path: {}\n", conf_path.string());
        return op_file;
    }

    json expected = build_config_json(conf);
    // scope тут нужен для того чтобы file после выхода из этого
    // блока закрылся, тем самым устранив непонятные, возможные ошибки
    {
        std::ofstream file(conf_path);

        if (!file.is_open()) {
            op_file.result = false;
            op_file.message = "Failed to open/create `config.json` for writing";
            return op_file;
        }

        file << expected.dump(4);

        if (file.fail()) {
            op_file.result = false;
            op_file.message = "Failed to write data to `config.json`";
            return op_file;
        }
    } // scope

    // правильно ли записались данные в файл
    std::ifstream verify(conf_path);

    if (!verify.is_open()) {
        op_file.result = false;
        op_file.message = "Cannot REOPEN config.json for verification";
        return op_file;
    }

    try {
        // parse - превращение файла в нужную структуру
        json actual = json::parse(verify);

        if (actual != expected) {
            op_file.result = false;
            op_file.message = "Verification error: config file was saved INCORRECTLY";
            return op_file;
        }
    } catch (const json::parse_error& e) {
        op_file.result = false;
        op_file.message = std::format("Config was written INCORRECTLY: {}\n", e.what());
        return op_file;
    }

    op_file.result = true;
    op_file.message = std::format("Config created & verified: {}\n", conf_path.string());
    return op_file;
}

butler::fs::ops::FileOperationResult load_config()
{
    return create_default_config();
}

} // namespace butler::fs::conf
