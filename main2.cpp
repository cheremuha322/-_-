#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "TcpLineServer.h"
#include "string_processor.h"

namespace {

// Удаление пробельных символов по краям
std::string strip(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;

    return s.substr(start, end - start);
}

// Безопасное преобразование строки в порт
bool toPort(const char* str, int& out) {
    if (!str || !*str) return false;
    
    char* end = nullptr;
    long val = std::strtol(str, &end, 10);
    
    // Проверяем, что вся строка была числом и значение в диапазоне порта
    if (*end != '\0' || val <= 0 || val > 65535) return false;
    
    out = static_cast<int>(val);
    return true;
}

} // namespace

int main(int argc, char* argv[]) {
    int port = 5555;

    if (argc >= 2) {
        if (!toPort(argv[1], port)) {
            std::cerr << "Неверный порт. Использование: program2 [port]" << std::endl;
            return 1;
        }
    }

    TcpLineServer server(port);
    if (!server.start()) {
        std::cerr << "[Program2] Не удалось запустить сервер на порту " << port << std::endl;
        return 1;
    }

    std::cout << "[Program2] Сервер запущен на порту " << port << std::endl;

    server.run([](const std::string& raw) {
        const std::string data = strip(raw);
        
        if (data.empty()) return;

        if (string_processor::isSum(data)) {
            std::cout << "[Program2] Принято корректное значение: " << data << std::endl;
        } else {
            std::cout << "[Program2] Ошибка валидации. Получено: '" << data << "'" << std::endl;
        }
    });

    return 0;
}