# Derleyici ve bayraklar
CC     = gcc
CFLAGS = -g -Wall -Wextra

# Kaynak dosyalar
SRC = src/ministream.c src/bellek_izci.c src/hash_map.c src/linked_list.c

# ---- Hedefler ----

all: test_temel test_bellek benchmark test_csv

test_temel: $(SRC) test/test_temel.c
	$(CC) $(CFLAGS) $(SRC) test/test_temel.c -o test_temel

test_bellek: $(SRC) test/test_bellek.c
	$(CC) $(CFLAGS) $(SRC) test/test_bellek.c -o test_bellek

benchmark: $(SRC) test/benchmark.c
	$(CC) $(CFLAGS) $(SRC) test/benchmark.c -o benchmark

test_csv: $(SRC) test/csv_oku.c
	$(CC) $(CFLAGS) $(SRC) test/csv_oku.c -o test_csv

# Opsiyonel: shared library (Python wrapper icin)
lib: $(SRC)
	$(CC) -shared -fPIC $(CFLAGS) $(SRC) -o ministream.so

# Veri uretici (opsiyonel)
uretec: data/uretec.c
	$(CC) $(CFLAGS) data/uretec.c -o uretec

# ---- Test ve Temizlik ----

test: test_temel test_bellek test_csv
	./test_temel
	./test_bellek
	./test_csv

valgrind: test_temel
	valgrind \
	  --leak-check=full \
	  --show-leak-kinds=all \
	  --track-origins=yes \
	  --error-exitcode=1 \
	  ./test_temel

valgrind-benchmark: benchmark
	valgrind \
	  --leak-check=full \
	  --show-leak-kinds=all \
	  --track-origins=yes \
	  ./benchmark 2>&1 | tee rapor/valgrind/benchmark_valgrind.txt

clean:
	rm -f test_temel test_bellek test_csv benchmark ministream.so uretec

# Web sunucusunu baslat (Hoca icin kolaylik)
web: lib
	python3 backend/server.py

.PHONY: all test valgrind valgrind-benchmark clean lib uretec web
