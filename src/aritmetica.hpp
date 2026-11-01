#pragma once
#include <string>
#include <utility>

// Representación: std::string de dígitos '0'-'9', dígito más significativo primero.
// Todas las funciones devuelven el resultado normalizado (sin ceros a la izquierda, salvo el número "0").

std::string normalizar(const std::string &A);
std::string rellenar(const std::string &A, std::size_t n);                       // ceros a la izquierda hasta longitud n
std::string sumar(const std::string &A, const std::string &B);                   // A + B (longitudes distintas permitidas)
std::string restar(const std::string &A, const std::string &B);                  // A - B, requiere A >= B
std::string desplazar(const std::string &A, std::size_t k);                      // A * 10^k
std::pair<std::string, std::string> partir(const std::string &A, std::size_t m); // parte alta y parte baja = últimos m dígitos
