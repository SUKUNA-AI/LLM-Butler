#pragma once

#include "butler/fs/filesystem.hpp"
#include <system_error>

namespace butler::fs {

class AppPaths {

public:
    // данный фабричный метод находит домашнюю папку пользователя
    // и на основе ее создает пути, где будут создаваться необходимые папки и файлы
    static AppPaths build(std::error_code& ec);

    // const noexcept - функция не изменяет переданный объект
    //                  и никогда не выбросит исключение
    const Path& root_dir() const noexcept;
    const Path& config_file() const noexcept;
    const Path& logs_dir() const noexcept;
    const Path& artifacts_dir() const noexcept;
    const Path& runtime_dir() const noexcept;

    bool valid() const noexcept;

private:
    Path root_dir_;
    Path config_file_;
    Path logs_dir_;
    Path artifacts_dir_;
    Path runtime_dir_;
};

} // namespace butler::fs
