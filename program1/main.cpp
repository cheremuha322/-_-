#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "SharedBuffer.h"
#include "SocketClient.h"
#include "string_processor.h"

namespace {

bool isValidInput(const std::string& s) {
    if (s.empty() || s.size() > 64) return false;
    for (char c : s) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

bool toPort(const char* str, int& out) {
    if (!str || !*str) return false;
    char* end = nullptr;
    long val = std::strtol(str, &end, 10);
    if (*end != '\0' || val <= 0 || val > 65535) return false;
    out = static_cast<int>(val);
    return true;
}

void producerLoop(SharedBuffer& buffer) {
    std::string line;
    while (true) {
        std::cout << "[Program1] Введите цифры (до 64) или Ctrl+D: " << std::flush;
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl;
            break;
        }
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (!isValidInput(line)) {
            std::cerr << "[Program1] Ошибка: только цифры, длина <= 64." << std::endl;
            continue;
        }

        string_processor::sortDesc(line);
        // push принимает std::string по значению — передаём без std::move
        buffer.push(line);
    }
    buffer.stop();
}

void consumerLoop(SharedBuffer& buffer, SocketClient& client) {
    std::string data;
    while (buffer.pop(data)) {
        std::cout << "[Program1] Поток 2 получил: " << data << std::endl;

        long long sum = string_processor::sumNumer(data);
        std::cout << "[Program1] Сумма цифр: " << sum << std::endl;

        // Явный каст к long для совместимости со всеми компиляторами
        std::string message = std::to_string(static_cast<long>(sum));

        if (!client.sendLine(message)) {
            std::cerr << "[Program1] Не удалось отправить в Программу 2" << std::endl;
        }
    }
}

} // namespace

int main(int argc, char* argv[]) {
    std::signal(SIGPIPE, SIG_IGN);

    std::string host = "127.0.0.1";
    int port = 5555;

    if (argc >= 2) host = argv[1];
    if (argc >= 3 && !toPort(argv[2], port)) {
        std::cerr << "Неверный порт. Использование: program1 [host] [port]" << std::endl;
        return 1;
    }

    SharedBuffer buffer;
    SocketClient client(host, port);

    std::thread consumer(consumerLoop, std::ref(buffer), std::ref(client));

    producerLoop(buffer);
    consumer.join();
    return 0;
}
