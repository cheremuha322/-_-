#include "string_processor.h"

// Обязательно подключите эти библиотеки для std::sort и std::isdigit
#include <algorithm>
#include <cctype>
#include <string>
#include <utility>

// Обязательно оберните всё в namespace string_processor
namespace string_processor {

void sortDesc(std::string& s) {
    // Сортируем по убыванию
    std::sort(s.begin(), s.end(), std::greater<char>());

    std::string result;
    result.reserve(s.size() * 2);
    
    for (char c : s) {
        // Если цифра и чётная
        if (std::isdigit(static_cast<unsigned char>(c)) && ((c - '0') % 2 == 0)) {
            result += '*'; 
        } else {
            result += c;
        }
    }
    s = std::move(result);
}

long long sumNumer(const std::string& s) {
    long long sum = 0;
    for (char c : s) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            sum += static_cast<long long>(c - '0');
        }
    }
    return sum;
}

bool isSum(const std::string& s) {
    if (s.empty()) return false;
    size_t start = (s[0] == '+') ? 1 : 0;
    if (start >= s.size()) return false;
    for (size_t i = start; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    return true;
}

} // namespace string_processor
