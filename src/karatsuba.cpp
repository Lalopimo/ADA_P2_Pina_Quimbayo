#include "algoritmos.hpp"
#include "aritmetica.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#ifndef UMBRAL
#define UMBRAL 1 //<---- si los operandos tienen <= UMBRAL dígitos se multiplica directo (UMBRAL >= 1)
#endif

// Funciones internas de este archivo 
// (el namespace anónimo evita que choquen con otros archivos y que se vean desde fuera).
namespace {

// Multiplicación escolar para el caso base. Acepta ceros a la izquierda.
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

// Precondición: X e Y tienen la MISMA longitud n >= 1 (con ceros a la izquierda si hace falta).
// NO se normalizan los operandos: así se conserva el relleno. En la llamada inicial n es una
// potencia de 2 y cada subproblema (x1y1, x0y0) vuelve a tener longitud exacta m = n/2.
// Solo el producto de las sumas (x1+x0)(y1+y0) puede traer un dígito más (m+1).
string karatsubaRec(const string& X, const string& Y) {
    size_t n = X.size();

    if (n <= UMBRAL) return multiplicadirecta(X, Y);

    // Longitud impar (solo ocurre en la rama de las sumas con dígito extra):
    // un cero más a la izquierda y las mitades vuelven a ser exactas.
    if (n % 2 == 1) return karatsubaRec(rellenar(X, n + 1), rellenar(Y, n + 1));

    size_t m = n / 2;

    auto [x1, x0] = partir(X, m);
    auto [y1, y0] = partir(Y, m);
    // partir normaliza (quita ceros); se restituye longitud exacta m para que cada
    // subproblema recursivo sea de tamaño m.
    x1 = rellenar(x1, m); x0 = rellenar(x0, m);
    y1 = rellenar(y1, m); y0 = rellenar(y0, m);

    string P1 = karatsubaRec(x1, y1);                 // P1 = x1*y1
    string P2 = karatsubaRec(x0, y0);                 // P2 = x0*y0

    string sx = sumar(x1, x0), sy = sumar(y1, y0);    // pueden tener m+1 dígitos
    size_t L = max({m, sx.size(), sy.size()});        // misma longitud para ambos (m o m+1)
    string P3 = karatsubaRec(rellenar(sx, L), rellenar(sy, L));  // P3 = (x1+x0)(y1+y0)

    // Orden de resta de la sección 3: primero P3 - P1 (>= P2), luego - P2. Nunca negativo.
    string medio = restar(restar(P3, P1), P2);        // = x1*y0 + x0*y1

    string R = sumar(sumar(desplazar(P1, 2 * m), desplazar(medio, m)), P2);
    return normalizar(R);
}

} // namespace

string karatsuba(string X, string Y) {
    X = normalizar(X);
    Y = normalizar(Y);
    size_t n = 1;
    while (n < max(X.size(), Y.size())) n <<= 1;      // potencia de 2 >= longitud máxima
    X = rellenar(X, n);
    Y = rellenar(Y, n);
    return karatsubaRec(X, Y);
}