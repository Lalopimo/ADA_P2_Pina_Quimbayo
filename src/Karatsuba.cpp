#include "algoritmos.hpp"
#include "aritmetica.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#ifndef UMBRAL
#define UMBRAL 1 //<---- por debajo de este tamaño se multiplica directo, si no recursión
#endif

// Funciones internas de este archivo 
// (el namespace anónimo evita que choquen con otros archivos y que se vean desde fuera).
namespace {

string multiplicadirecta(const string& X, const string& Y) {
    size_t n = X.size(), m = Y.size();
    vector<int> r(n + m, 0);
    for (size_t i = 0; i < n; i++) {
        int a = X[n - 1 - i] - '0';
        if (a == 0) continue;
        for (size_t j = 0; j < m; j++)
            r[i + j] += a * (Y[m - 1 - j] - '0');
    }
    int acarreo = 0;
    for (size_t k = 0; k < r.size(); k++) {
        int v = r[k] + acarreo;
        r[k] = v % 10;
        acarreo = v / 10;
    }
    string R(r.size(), '0');
    for (size_t k = 0; k < r.size(); k++)
        R[r.size() - 1 - k] = char('0' + r[k]);
    return normalizar(R);
}

string karatsubaRec(string X, string Y) {
    X = normalizar(X);
    Y = normalizar(Y);

    if (X == "0" || Y == "0") return "0";

    if (X.size() <= UMBRAL && Y.size() <= UMBRAL)
        return multiplicadirecta(X, Y);

    size_t n = max(X.size(), Y.size());
    X = rellenar(X, n);
    Y = rellenar(Y, n);
    size_t m = n / 2;

    auto [x1, x0] = partir(X, m);
    auto [y1, y0] = partir(Y, m);

    string P0 = karatsubaRec(x0, y0);
    string P2 = karatsubaRec(x1, y1);
    string P1 = karatsubaRec(sumar(x1, x0), sumar(y1, y0));

    string medio = restar(restar(P1, P2), P0);

    string R = sumar(sumar(desplazar(P2, 2 * m), desplazar(medio, m)), P0);
    return normalizar(R);
}

} // namespace

string karatsuba(string X, string Y) {
    X = normalizar(X);
    Y = normalizar(Y);
    size_t n = 1;
    while (n < max(X.size(), Y.size())) n <<= 1;
    X = rellenar(X, n);
    Y = rellenar(Y, n);
    return karatsubaRec(X, Y);
}