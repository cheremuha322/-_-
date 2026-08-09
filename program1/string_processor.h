#pragma once

#include <string>

namespace string_processor {

// Для Программы №1: сортировка и замена чётных цифр
void sortDesc(std::string& s);

// Для Программы №1: сумма цифр в строке
long long sumNumer(const std::string& s);

// Для Программы №2: проверка корректности полученной суммы
bool isSum(const std::string& s);

} // namespace string_processor
