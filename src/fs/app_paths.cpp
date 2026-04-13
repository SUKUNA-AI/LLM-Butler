#include "butler/fs/app_paths.hpp"

namespace butler::fs {

AppPaths AppPaths::build(std::error_code& ec)
{
    // данный фабричный метод находит домашнюю папку пользователя
    // и на основе ее создает пути, где будут создаваться необходимые папки и файлы
    ec.clear();

    // вызов конструктора по умолчанию
    AppPaths paths { };

    const auto root = butler::fs::root_dir(ec);
    if (ec || root.empty()) {
        return paths;
    }

    paths.root_dir_ = root;
    paths.config_file_ = root / "config.json";
    paths.logs_dir_ = root / "logs";
    paths.artifacts_dir_ = root / "artifacts";
    paths.runtime_dir_ = root / "runtime";

    return paths;
}

const Path& AppPaths::root_dir() const noexcept
{
    return root_dir_;
}

const Path& AppPaths::config_file() const noexcept
{
    return config_file_;
}

const Path& AppPaths::logs_dir() const noexcept
{
    return logs_dir_;
}

const Path& AppPaths::artifacts_dir() const noexcept
{
    return artifacts_dir_;
}

const Path& AppPaths::runtime_dir() const noexcept
{
    return runtime_dir_;
}

bool AppPaths::valid() const noexcept
{
    return !root_dir_.empty();
}

} // namespace butler::fs
