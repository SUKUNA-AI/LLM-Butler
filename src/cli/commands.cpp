#include "butler/cli/commands.hpp"
#include <string_view>
#include <iostream>

namespace butler::cli{

    int run_init(){
    // Заглушка
    // создать рабочую директорию Butler
    // создать подпапки
    // создать config
        std::cout<<"init не реализован\n";

        return 0;
    }

    int run_status(){
        // Заглушка
        // Реализовать потом:
        // проверка инициализации
        // существование директорий 
        // чтение конфига

        std::cout<<"run_status не реализован\n";

        return 0;
    }

    void print_unknow_command(std::string_view command){
        std::cerr <<"Unknow command: " << command << "\n";
        std::cerr << "Use 'butler --help' or 'butler help' to see available commands\n";
    }

    void print_help(){
        std::cout
            << "GPU Butler\n"
            << "Local-first runtime manager for LLM backends.\n\n"
            << "Usage:\n"
            << "  butler [command]\n\n"
            << "Commands:\n"
            << "  --help, help   Show this help message\n"
            << "  init           Initialize Butler workspace\n"
            << "  status         Show Butler status\n";
    }

    int handle_commands(int argc, char* argv[]){
        if (argc <= 1){
            print_help();
            return 0;
        }

        const std::string_view command{argv[1]};

        if (command == "--help" || command == "help") {
            print_help();
            return 0;
        }

        if (command == "init") {
            return run_init();
        }

        if (command == "status") {
            return run_status();
        }

        print_unknow_command(command);

        return 1;

    }

} //namespace butler::cli 