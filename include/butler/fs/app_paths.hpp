#pragma once

#include "butler/fs/filesystem.hpp"
#include <system_error>

namespace butler::fs {

class AppPaths {

public:
    static AppPaths build(std::error_code& ec);

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