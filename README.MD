# GPU Butler

GPU Butler - это учебный C++20 CLI-проект, с которого начинается разработка будущего local-first runtime manager для LLM backend'ов.

Сейчас это ранний V0-скелет:
- проект собирается в один CLI-бинарь;
- поддерживает базовые команды `help`, `init`, `status`;
- `init` и `status` пока остаются заглушками.

## Сборка

```bash
mkdir -p build
g++ -std=c++20 apps/cli/main.cpp src/cli/commands.cpp -Iinclude -o build/butler
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

## Текущее поведение

- `butler` и `butler --help` показывают справку.
- `butler init` пока сообщает, что инициализация ещё не реализована.
- `butler status` пока сообщает, что проверка статуса ещё не реализована.
- неизвестная команда возвращает ошибку и подсказывает вызвать help.

## Структура проекта

```text
apps/cli/main.cpp                # точка входа CLI
include/butler/cli/commands.hpp # объявления CLI-команд
src/cli/commands.cpp            # реализация CLI-команд
```
