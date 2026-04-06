# LLM Butler

LLM Butler - это учебный C++20 CLI-проект, с которого начинается разработка будущего local-first runtime manager для LLM backend'ов.

Сейчас это ранний V0-скелет:
- проект собирается в один CLI-бинарь;
- поддерживает базовые команды `help`, `init`, `status`;
- `init` создаёт базовую структуру каталогов в `~/Butler` и безопасно работает при повторном запуске;
- `status` только проверяет состояние workspace и не меняет файловую систему.

## Возможности V0

- `butler help` и `butler --help` показывают справку по доступным командам.
- `butler init` создаёт `~/Butler` и базовые подпапки `logs`, `artifacts`, `runtime`.
- повторный `butler init` не считается ошибкой, если директории уже существуют.
- `butler status` проверяет `root`, `logs`, `artifacts`, `runtime` и сообщает итог: `Butler is initialized` или `Butler is not initialized`.
- неизвестная команда возвращает ошибку и подсказывает вызвать help.

## Сборка Через CMake

```bash
cmake -S . -B build
cmake --build build
```

После сборки бинарь будет доступен как `build/butler`.

## Ручная Сборка

```bash
mkdir -p build
g++ -std=c++20 apps/cli/main.cpp src/cli/commands.cpp src/fs/filesystem.cpp src/fs/filesystem_ops.cpp -Iinclude -o build/butler
```

## Запуск

```bash
./build/butler
./build/butler --help
./build/butler help
./build/butler init
./build/butler status
./build/butler abracadabra
```

## Описание команд

### `butler init`

- создаёт директории `~/Butler`, `~/Butler/logs`, `~/Butler/artifacts`, `~/Butler/runtime`;
- печатает отдельные сообщения для созданных или уже существующих директорий;
- завершает работу сообщением `Butler is initialized`, только если все обязательные директории в корректном состоянии.

### `butler status`

- печатает путь до workspace Butler;
- сообщает статус каждой обязательной директории;
- не создаёт и не исправляет каталоги;
- завершает работу сообщением `Butler is initialized` или `Butler is not initialized`.

## Структура Проекта

```text
apps/cli/main.cpp                 # точка входа CLI
include/butler/cli/commands.hpp  # объявления CLI-команд
include/butler/fs/filesystem.hpp # файловые утилиты и пути Butler
src/cli/commands.cpp             # реализация CLI-команд
src/fs/filesystem.cpp            # реализация файловых операций
CMakeLists.txt                   # конфигурация сборки через CMake
```
