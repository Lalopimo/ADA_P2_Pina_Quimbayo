// Validación: Karatsuba vs multiplicación clásica (y contra valores conocidos).
// Compilar también con distintos umbrales: -DUMBRAL=1, -DUMBRAL=4, -DUMBRAL=64
// porque cada umbral ejercita caminos distintos del código.
#include "algoritmos.hpp"
#include <iostream>
#include <random>
#include <string>

static int total = 0, fallos = 0;

static std::string corto(const std::string& s) {
    return s.size() <= 30 ? s : s.substr(0, 30) + "...(" + std::to_string(s.size()) + " dig)";
}

// Compara clásica y Karatsuba entre sí; si se da `esperado`, también contra él.
static void verificar(const std::string& etiqueta, const std::string& a, const std::string& b,
                      const std::string* esperado = nullptr) {
    total++;
    std::string c = multiplicacionClasica(a, b);
    std::string k = karatsuba(a, b);
    bool ok = (c == k) && (!esperado || c == *esperado);
    if (!ok) {
        fallos++;
        if (fallos <= 10)
            std::cout << "FALLO [" << etiqueta << "] " << corto(a) << " x " << corto(b)
                      << "\n   clasica   = " << corto(c) << "\n   karatsuba = " << corto(k) << "\n";
    }
}

static std::string aleatorio(std::mt19937_64& g, std::size_t n, bool permitirCerosIzq) {
    std::uniform_int_distribution<int> d(0, 9);
    std::string s(n, '0');
    for (auto& c : s) c = char('0' + d(g));
    if (!permitirCerosIzq && n > 0 && s[0] == '0') s[0] = '1';
    return s;
}

int main() {
    std::mt19937_64 g(2024);

    // 1) Un dígito: todas las combinaciones 0..9 (oráculo: aritmética nativa, solo en el test)
    for (int i = 0; i <= 9; i++)
        for (int j = 0; j <= 9; j++) {
            std::string esp = std::to_string(i * j);
            verificar("1 digito", std::to_string(i), std::to_string(j), &esp);
        }

    // 2) Muchos ceros a la izquierda
    { std::string e = "5535";  verificar("ceros izq", "000123", "0045", &e); }
    { std::string e = "0";     verificar("ceros izq", "0000000000", "7", &e); }
    { std::string e = "1";     verificar("ceros izq", "0001", "0001", &e); }
    { std::string e = "546";   verificar("ceros izq", "00000000000000000042", "0000000000000000000000000000013", &e); }
    verificar("ceros izq", std::string(200, '0') + "987654321", std::string(150, '0') + "123456789");

    // 3) Ceros intercalados
    { std::string e = "1002001";           verificar("ceros interc", "1001", "1001", &e); }
    { std::string e = "1000000";           verificar("ceros interc", "1000", "1000", &e); }
    { std::string e = "10000000200000001"; verificar("ceros interc", "100000001", "100000001", &e); }
    { std::string e = "8101620081";        verificar("ceros interc", "90009", "90009", &e); }
    for (int t = 0; t < 300; t++) {   // números con muchos ceros, longitudes variadas
        std::string a = aleatorio(g, 1 + g() % 80, false), b = aleatorio(g, 1 + g() % 80, false);
        for (auto& c : a) if (g() % 3) c = '0';
        for (auto& c : b) if (g() % 3) c = '0';
        verificar("ceros interc (aleatorio)", a, b);
    }

    // 4) Longitudes impares y distintas (antes del relleno)
    { std::string e = "5535";      verificar("impares", "123", "45", &e); }
    { std::string e = "111111111"; verificar("impares", "12345679", "9", &e); }
    { std::string e = "99899001";  verificar("impares", "99999", "999", &e); }
    { std::string e = "7006652";   verificar("ejemplo informe", "1234", "5678", &e); }
    for (std::size_t la = 1; la <= 65; la++)
        for (std::size_t lb = 1; lb <= 65; lb++)
            for (int r = 0; r < 3; r++)
                verificar("longitudes", aleatorio(g, la, true), aleatorio(g, lb, true));

    // 5) Contra aritmética nativa (hasta 9 dígitos cada uno => producto < 2^63)
    for (int t = 0; t < 20000; t++) {
        unsigned long long p = g() % 1000000000ULL, q = g() % 1000000000ULL;
        std::string esp = std::to_string(p * q);
        verificar("nativo", std::to_string(p), std::to_string(q), &esp);
    }

    // 6) Productos grandes (varios cientos de dígitos)
    {   // (10^k - 1)^2 = 9..9 8 0..0 1  (con k-1 nueves y k-1 ceros)
        for (std::size_t k : {64u, 300u, 512u, 777u}) {
            std::string nueves(k, '9');
            std::string esp = std::string(k - 1, '9') + "8" + std::string(k - 1, '0') + "1";
            verificar("(10^k-1)^2", nueves, nueves, &esp);
        }
        std::string a = "1" + std::string(299, '0'), b = "1" + std::string(298, '0');
        std::string esp = "1" + std::string(597, '0');          // 10^299 * 10^298 = 10^597
        verificar("potencias de 10", a, b, &esp);
        for (std::size_t n : {100u, 257u, 500u, 1024u, 2048u})
            verificar("grande aleatorio", aleatorio(g, n, false), aleatorio(g, n, false));
    }

    // 7) Neutros: x*0 = 0, x*1 = x
    for (int t = 0; t < 50; t++) {
        std::string x = aleatorio(g, 1 + g() % 200, false);
        std::string cero = "0", uno = "1";
        verificar("por cero", x, cero, &cero);
        verificar("por uno", x, uno, &x);
    }

    std::cout << "Casos: " << total << "   Fallos: " << fallos << "\n";
    if (fallos == 0) std::cout << "VALIDACION OK\n";
    return fallos ? 1 : 0;
}
