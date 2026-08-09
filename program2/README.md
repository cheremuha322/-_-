### program2/README.md:

```markdown
# Программа №2

TCP-сервер для приёма и валидации данных от Программы №1.

## Функционал

- Слушает TCP-порт
- Принимает строки от клиента
- Валидирует полученные данные (проверка корректности суммы)

## Сборка

```bash
g++ -std=c++17 -pthread -o program2 main.cpp TcpLineServer.cpp string_processor.cpp
