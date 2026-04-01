# GPU Butler

LLM Butler - это учебный C++20 CLI-проект, с которого начинается разработка будущего runtime-manager для LLM backend'ов.  

## Сборка проекта

```bash
g++ -std=c++20 apps/cli/main.cpp src/cli/commands.cpp -Iinclude -o build/butler

Запуск:
./build/butler
./build/butler --help
./build/butler help
./build/butler init
./build/butler status
./build/butler abracadabra 
``` 
