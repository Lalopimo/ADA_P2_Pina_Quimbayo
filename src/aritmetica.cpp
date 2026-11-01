#include "aritmetica.hpp"

#include <algorithm>
#include <utility>

using namespace std;

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
