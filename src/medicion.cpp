// Medición de tiempos: multiplicación clásica vs Karatsuba.
//
// Uso:   ./medicion [n_max=4096] [muestras=7]  > resultados.csv
// Salida: CSV por la salida estándar; progreso y resumen por la de error.

#include "algoritmos.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#ifndef UMBRAL
#define UMBRAL 1 // debe coincidir con el de karatsuba.cpp: compilar todo con el mismo -DUMBRAL
#endif

using reloj = std::chrono::steady_clock;

static const double OBJETIVO_US = 100000.0; // cada lote debe durar >= 100 ms
static volatile std::size_t sumidero = 0;

static std::string generar(std::mt19937_64 &gen, std::size_t n)
{
    std::uniform_int_distribution<int> dig(0, 9);
    std::string s(n, '0');
    for (auto &c : s)
        c = char('0' + dig(gen));
    if (s[0] == '0')
        s[0] = '1'; // exactamente n dígitos
    return s;
}

template <class F>
static double medirLote(F f, const std::string &a, const std::string &b, long reps)
{
    std::size_t acc = 0;
    auto t0 = reloj::now();
    for (long i = 0; i < reps; i++)
    {
        std::string r = f(a, b);
        acc += r.size() + (unsigned char)r.back();
    }
    auto t1 = reloj::now();
    sumidero = sumidero + acc;
    return std::chrono::duration<double, std::micro>(t1 - t0).count();
}

template <class F>
static long calibrar(F f, const std::string &a, const std::string &b)
{
    long reps = 1;
    for (;;)
    {
        double us = medirLote(f, a, b, reps);
        if (us >= OBJETIVO_US)
            return reps;
        reps *= (us < OBJETIVO_US / 10) ? 10 : 2;
    }
}

struct Resultado
{
    double mediana_us, minimo_us;
    long reps;
};

template <class F>
static Resultado medirTamano(F f, const std::string &a, const std::string &b, int muestras)
{
    (void)f(a, b); // calentamiento (cachés, asignador de memoria)
    long reps = calibrar(f, a, b);
    std::vector<double> t;
    for (int m = 0; m < muestras; m++)
        t.push_back(medirLote(f, a, b, reps) / reps);
    std::sort(t.begin(), t.end());
    return {t[t.size() / 2], t.front(), reps};
}

int main(int argc, char **argv)
{
    std::size_t nMax = argc > 1 ? std::strtoull(argv[1], nullptr, 10) : 4096;
    int muestras = argc > 2 ? std::atoi(argv[2]) : 7;
    if (nMax < 2 || muestras < 1)
    {
        std::cerr << "Uso: ./medicion [n_max>=2] [muestras>=1]\n";
        return 1;
    }

    auto clasica = [](const std::string &x, const std::string &y)
    { return multiplicacionClasica(x, y); };
    auto karats = [](const std::string &x, const std::string &y)
    { return karatsuba(x, y); };

    std::mt19937_64 gen(12345);
    std::vector<std::size_t> ns;
    std::vector<double> razones;

    std::cout << "n,umbral,reps_clasica,reps_karatsuba,clasica_us_mediana,clasica_us_min,"
                 "karatsuba_us_mediana,karatsuba_us_min,razon_clasica_sobre_karatsuba\n";
    std::cout << std::fixed << std::setprecision(3);

    for (std::size_t n = 2; n <= nMax; n *= 2)
    {
        std::string a = generar(gen, n), b = generar(gen, n);

        if (multiplicacionClasica(a, b) != karatsuba(a, b))
        { // fuera de la zona cronometrada
            std::cerr << "ERROR: los algoritmos difieren para n=" << n << "\n";
            return 2;
        }

        Resultado c = medirTamano(clasica, a, b, muestras);
        Resultado k = medirTamano(karats, a, b, muestras);
        double razon = c.mediana_us / k.mediana_us; // > 1  => Karatsuba es mas rapido

        std::cout << n << "," << UMBRAL << "," << c.reps << "," << k.reps << ","
                  << c.mediana_us << "," << c.minimo_us << ","
                  << k.mediana_us << "," << k.minimo_us << "," << razon << "\n";
        std::cout.flush();
        std::cerr << "n=" << n << "  clasica=" << c.mediana_us << " us  karatsuba="
                  << k.mediana_us << " us  razon=" << razon << "\n";
        ns.push_back(n);
        razones.push_back(razon);
    }

    // Cruce "persistente": menor n > UMBRAL desde el cual Karatsuba gana en todos los tamaños siguientes.
    // (Para n <= UMBRAL, Karatsuba ejecuta el mismo caso base cuadrático: esa razón no cuenta.)
    long idx = -1;
    for (long i = (long)ns.size() - 1; i >= 0 && ns[i] > UMBRAL && razones[i] > 1.0; i--)
        idx = i;
    if (idx >= 0)
        std::cerr << "Punto de cruce (UMBRAL=" << UMBRAL << "): n = " << ns[idx] << " digitos\n";
    else
        std::cerr << "No hay punto de cruce en el rango medido (UMBRAL=" << UMBRAL << ")\n";
    std::cerr << "checksum (para evitar eliminacion del calculo): " << sumidero << "\n";
    return 0;
}