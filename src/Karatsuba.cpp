#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#ifndef UMBRAL
#define UMBRAL 1 //<---- apartir de este numero empieza a utilizar recursion
#endif


string normalizar(const string& A) {
    size_t i = 0;
    while (i + 1 < A.size() && A[i] == '0') i++;
    return A.empty() ? "0" : A.substr(i);
}


string rellenar(const string& A, size_t n) {
    if (A.size() >= n) return A;
    return string(n - A.size(), '0') + A;
}

string sumar(const string& A, const string& B) {
    string R;
    R.reserve(max(A.size(), B.size()) + 1);
    int i = (int)A.size() - 1, j = (int)B.size() - 1, acarreo = 0;
    while (i >= 0 || j >= 0 || acarreo) {
        int s = acarreo;
        if (i >= 0) s += A[i--] - '0';
        if (j >= 0) s += B[j--] - '0';
        R.push_back(char('0' + s % 10));
        acarreo = s / 10;
    }
    reverse(R.begin(), R.end());
    return normalizar(R);
}

string restar(const string& A, const string& B) {
    string R;
    R.reserve(A.size());
    int i = (int)A.size() - 1, j = (int)B.size() - 1, prestamo = 0;
    while (i >= 0) {
        int d = (A[i--] - '0') - prestamo;
        if (j >= 0) d -= (B[j--] - '0');
        if (d < 0) { d += 10; prestamo = 1; } else prestamo = 0;
        R.push_back(char('0' + d));
    }
    reverse(R.begin(), R.end());
    return normalizar(R);
}

string desplazar(const string& A, size_t k) {
    if (A == "0") return "0";
    return A + string(k, '0');
}

pair<string, string> partir(const string& A, size_t m) {
    if (A.size() <= m) return {"0", normalizar(A)};
    string alta = A.substr(0, A.size() - m);
    string baja = A.substr(A.size() - m);
    return {normalizar(alta), normalizar(baja)};
}

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

string karatsuba(string X, string Y) {
    X = normalizar(X);
    Y = normalizar(Y);
    size_t n = 1;
    while (n < max(X.size(), Y.size())) n <<= 1;
    X = rellenar(X, n);
    Y = rellenar(Y, n);
    return karatsubaRec(X, Y);
}

bool esNumero(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(),
                                [](char c) { return c >= '0' && c <= '9'; });
}

int main() {
    string a, b;
    cout << "Primer cadena: ";
    cin >> a;
    cout << "Segunda cadena: ";
    cin >> b;
    if (!esNumero(a) || !esNumero(b)) {
        cerr << "Solo se pueden numeros positivos.\n";
        return 1;
    }
    auto inicio = chrono::steady_clock::now();
    string producto = karatsuba(a, b);
    auto fin = chrono::steady_clock::now();

    chrono::duration<double, milli> ms = fin - inicio;
    cout << "Resultado: " << producto << "\n";
    cout << "Tiempo: " << ms.count() << " ms ("
         << chrono::duration_cast<chrono::microseconds>(fin - inicio).count()
         << " us)\n";
    return 0;
}
