# Práctica 2: Multiplicación rápida de enteros (Karatsuba)

Análisis y Diseño de Algoritmos

**Integrantes:** José Eduardo Piña Montero · Angel David Quimbayo Carvajal

## Requisitos

- `g++` o `clang++` con soporte C++17, y `make`
- Python 3 con `matplotlib` (solo para las gráficas)

## Compilación y ejecución

Todos los comandos se ejecutan desde la raíz del repositorio. Los binarios se generan en `build/`.

### Validar la correctitud

```bash
make test
```

Compila y ejecuta las pruebas con varios umbrales. Cada ejecución debe terminar con `VALIDACION OK`.

### Medir tiempos

```bash
make medir
```

Compila y mide la multiplicación clásica y Karatsuba (tamaños de 2 a 4096 dígitos). Los resultados quedan en `docs/` como archivos CSV, junto con un archivo `entorno_*.txt` que describe el sistema.

Parámetros opcionales:

```bash
make medir NMAX=4096 MUESTRAS=7 UMBRALES="1 16 64 128"
```

### Graficar

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install matplotlib
python3 scripts/graficar.py docs/macos_corrida1 docs/macos_corrida2 docs/macos_corrida3 --prefijo docs/macos
```

Genera `docs/macos_tiempo.png`, `docs/macos_razon.png` y `docs/macos_tabla.tex`.

### Limpiar

```bash
make clean
```

### Compilar el informe

```bash
tectonic informe/informe.tex
```