#pragma once
#include <string>

// Interfaz común de los dos algoritmos.
// Representación: string de dígitos '0'-'9', dígito más significativo primero.
// Ambas devuelven el producto normalizado (sin ceros a la izquierda, salvo "0").
std::string multiplicacionClasica(const std::string& a, const std::string& b);
std::string karatsuba(std::string a, std::string b);
