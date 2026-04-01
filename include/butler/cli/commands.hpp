#pragma once

// Здесь мы объявляем функции, которые будут реализованы в commands.cpp.
// Header нужен для того, чтобы другие .cpp-файлы знали,
// какие функции существуют и как их можно вызывать.

namespace butler::cli{

    void print_help();

    int handle_commands(int argc, char* argv[]);

} // namespace butler::cli


