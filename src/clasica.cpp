#include "algoritmos.hpp"
#include "aritmetica.hpp"

#include <string>
#include <vector>

using namespace std;

// Multiplicación escolar Θ(n^2): cada dígito de A por cada dígito de B.
string multiplicacionClasica(const string &A, const string &B)
{
    string X = normalizar(A), Y = normalizar(B);
    if (X == "0" || Y == "0")
        return "0";

    size_t n = X.size(), m = Y.size();

    vector<int> r(n + m, 0);

    for (size_t i = 0; i < n; i++)
    {
        int a = X[n - 1 - i] - '0';
        if (a == 0)
            continue;
        for (size_t j = 0; j < m; j++)
        {
            int b = Y[m - 1 - j] - '0';
            r[i + j] += a * b;
        }
    }

    int acarreo = 0;
    for (size_t k = 0; k < r.size(); k++)
    {
        int v = r[k] + acarreo;
        r[k] = v % 10;
        acarreo = v / 10;
    }

    string R(r.size(), '0');
    for (size_t k = 0; k < r.size(); k++)
        R[r.size() - 1 - k] = char('0' + r[k]);

    return normalizar(R);
}