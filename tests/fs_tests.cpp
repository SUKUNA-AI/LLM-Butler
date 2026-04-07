#include "butler/fs/app_paths.hpp"
#include "butler/fs/filesystem_ops.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace {

namespace bfs = butler::fs;
namespace ops = butler::fs::ops;

void expect(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void set_home(const std::filesystem::path& path)
{
#ifdef _WIN32
    _putenv_s("HOME", path.string().c_str());
#else
    setenv("HOME", path.string().c_str(), 1);
#endif
}

long long unique_suffix()
{
    return std::chrono::steady_clock::now().time_since_epoch().count();
}

struct TempHome {
    explicit TempHome(std::string name)
        : path(std::filesystem::temp_directory_path()
            / (std::move(name) + "-" + std::to_string(unique_suffix())))
    {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
        std::filesystem::create_directories(path, ec);
        expect(!ec, "failed to create temp HOME");
        set_home(path);
    }

    ~TempHome()
    {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
    }

    std::filesystem::path path;
};

void app_paths_are_based_on_home()
{
    TempHome home("butler-app-paths-test");

    std::error_code ec;
    const auto paths = bfs::AppPaths::build(ec);

    expect(!ec, "AppPaths::build returned error");
    expect(paths.valid(), "AppPaths should be valid when HOME is set");
    expect(paths.root_dir() == home.path / "Butler", "wrong root_dir");
    expect(paths.config_file() == home.path / "Butler" / "config.json", "wrong config_file");
    expect(paths.logs_dir() == home.path / "Butler" / "logs", "wrong logs_dir");
    expect(paths.artifacts_dir() == home.path / "Butler" / "artifacts", "wrong artifacts_dir");
    expect(paths.runtime_dir() == home.path / "Butler" / "runtime", "wrong runtime_dir");
}

void status_reports_missing_before_init()
{
    TempHome home("butler-status-missing-test");

    const auto result = ops::check_main_directories();

    expect(!result.result, "status should fail before init");
    expect(result.message.find("Butler is not initialized") != std::string::npos,
        "status should report not initialized");
    expect(!std::filesystem::exists(home.path / "Butler"),
        "status must not create Butler workspace");
}

void init_creates_workspace_and_is_idempotent()
{
    TempHome home("butler-init-test");
    const auto root = home.path / "Butler";

    const auto init = ops::ensure_butler_initialization();
    expect(init.result, "init should succeed on empty HOME");
    expect(std::filesystem::is_directory(root), "init should create root directory");
    expect(std::filesystem::is_directory(root / "logs"), "init should create logs directory");
    expect(std::filesystem::is_directory(root / "artifacts"), "init should create artifacts directory");
    expect(std::filesystem::is_directory(root / "runtime"), "init should create runtime directory");

    const auto status = ops::check_main_directories();
    expect(status.result, "status should succeed after init");

    const auto second_init = ops::ensure_butler_initialization();
    expect(second_init.result, "init should be idempotent");
}

void init_fails_when_root_is_file()
{
    TempHome home("butler-root-file-test");
    const auto root = home.path / "Butler";

    {
        std::ofstream file(root);
        expect(file.good(), "failed to create root placeholder file");
    }

    const auto init = ops::ensure_butler_initialization();
    expect(!init.result, "init should fail when root path is a file");
    expect(init.message.find("not a directory") != std::string::npos,
        "init should explain that root path is not a directory");
}

} // namespace

int main()
{
    try {
        app_paths_are_based_on_home();
        status_reports_missing_before_init();
        init_creates_workspace_and_is_idempotent();
        init_fails_when_root_is_file();
    } catch (const std::exception& ex) {
        std::cerr << "Test failed: " << ex.what() << '\n';
        return 1;
    }

    std::cout << "All fs tests passed\n";
    return 0;
}
