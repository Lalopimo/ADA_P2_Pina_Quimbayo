# Makefile de la Práctica 2 (Karatsuba vs clásica).
#   make test     compila y corre la validación con varios umbrales
#   make medir    compila y corre las mediciones (un CSV por umbral en docs/)
#   make clean    borra lo compilado
# Parámetros opcionales:  make medir NMAX=4096 MUESTRAS=7 UMBRALES="1 16 64 128"

CXX      ?= g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Isrc
ALGO     := src/aritmetica.cpp src/clasica.cpp src/karatsuba.cpp
HDRS     := $(wildcard src/*.hpp)

UMBRALES_TEST := 1 2 4 64
UMBRALES      ?= 1 16 64 128
NMAX          ?= 4096
MUESTRAS      ?= 7
PLAT          := $(shell uname -s)_$(shell uname -m)

.PHONY: test medir info clean

build/test_u%: $(ALGO) tests/test_validacion.cpp $(HDRS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) -DUMBRAL=$* $(ALGO) tests/test_validacion.cpp -o $@

build/medicion_u%: $(ALGO) src/medicion.cpp $(HDRS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) -DUMBRAL=$* $(ALGO) src/medicion.cpp -o $@

test: $(foreach u,$(UMBRALES_TEST),build/test_u$(u))
	for u in $(UMBRALES_TEST); do echo "== Validacion con UMBRAL=$$u"; ./build/test_u$$u || exit 1; done

info:
	mkdir -p docs
	{ echo "fecha: $$(date)"; echo "sistema: $$(uname -a)"; echo "compilador: $$($(CXX) --version | head -1)"; \
	  echo "banderas: $(CXXFLAGS)"; \
	  if command -v lscpu >/dev/null 2>&1; then lscpu | grep -E "Model name|^CPU\(s\)|max MHz|Thread|Core"; \
	  else echo "cpu: $$(sysctl -n machdep.cpu.brand_string 2>/dev/null)"; fi; } > docs/entorno_$(PLAT).txt
	cat docs/entorno_$(PLAT).txt

medir: info $(foreach u,$(UMBRALES),build/medicion_u$(u))
	for u in $(UMBRALES); do \
	  echo "== Midiendo UMBRAL=$$u  (n_max=$(NMAX), muestras=$(MUESTRAS))"; \
	  ./build/medicion_u$$u $(NMAX) $(MUESTRAS) > docs/resultados_$(PLAT)_u$$u.csv || exit 1; \
	done
	echo "Listo: CSV en docs/resultados_$(PLAT)_u*.csv"

clean:
	rm -rf build
