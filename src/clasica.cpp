#include <iostream>
#include <algorithm>
#include <chrono>
#include <string>


using namespace std;


string normalizar(const string& A) {
    size_t i = 0;
    while (i + 1 < A.size() && A[i] == '0') i++;
    return A.empty() ? "0" : A.substr(i);
}


string multiplicarClasica(const string& A, const string& B) {
    string X = normalizar(A), Y = normalizar(B);
    if (X == "0" || Y == "0") return "0";

    size_t n = X.size(), m = Y.size();

    vector<int> r(n + m, 0);

    for (size_t i = 0; i < n; i++) {
        int a = X[n - 1 - i] - '0';
        if (a == 0) continue;
        for (size_t j = 0; j < m; j++) {
            int b = Y[m - 1 - j] - '0';
            r[i + j] += a * b;
        }
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

bool esNumero(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(),
                                [](char c) { return c >= '0' && c <= '9'; });
}

int main() {
    string a, b;
    cout << "Primera cadena: ";
    cin >> a;
    cout << "Segunda cadena: ";
    cin >> b;
    if (!esNumero(a) || !esNumero(b)) {
        cerr << ".\n";
        return 1;
    }
    auto inicio = chrono::steady_clock::now();
    string producto = multiplicarClasica(a, b);
    auto fin = chrono::steady_clock::now();

    chrono::duration<double, milli> ms = fin - inicio;
    cout << "Producto: " << producto << "\n";
    cout << "Tiempo: " << ms.count() << " ms ("
         << chrono::duration_cast<chrono::microseconds>(fin - inicio).count()
         << " us)\n";
    return 0;
}
