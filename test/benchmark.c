/* test/benchmark.c */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/hash_map.h"
#include "../src/linked_list.h"

/* Milisaniye cinsinden sure hesapla */
double ms_olc(struct timespec t1, struct timespec t2) {
    return (t2.tv_sec  - t1.tv_sec)  * 1000.0 +
           (t2.tv_nsec - t1.tv_nsec) / 1e6;
}

void benchmark_arama(int n_sarki) {
    struct timespec t1, t2;
    srand(42);

    /* ---- Linked list ---- */
    izci_sifirla();
    Sarki* bas = veri_uret_liste(n_sarki);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++)
        sarki_ara_liste(bas, rand() % n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    double liste_ms  = ms_olc(t1, t2);
    size_t liste_mem = izci_toplam_ayrildi();
    liste_temizle_hepsi(bas);

    /* ---- Hash map ---- */
    izci_sifirla();
    HashMap* map = veri_uret_map(n_sarki);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++)
        sarki_ara_map(map, rand() % n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    double map_ms  = ms_olc(t1, t2);
    size_t map_mem = izci_toplam_ayrildi();
    hashmap_temizle(map);

    printf("| %8d | %10.3f ms | %10.3f ms | %7.1fx |\n",
           n_sarki, liste_ms, map_ms,
           liste_ms > 0 ? liste_ms / map_ms : 0);
    (void)liste_mem; (void)map_mem;
}

int main() {
    printf("\n=== ARAMA BENCHMARK (1000 sorgu) ===\n");
    printf("| N sarki | LinkedList   |    HashMap   |    Fark |\n");
    printf("|---------|--------------|--------------|----------|\n");

    int boyutlar[] = {100, 1000, 10000, 100000};
    for (int i = 0; i < 4; i++)
        benchmark_arama(boyutlar[i]);

    printf("\n");

    /* Kopya vs Pointer modeli deney */
    deney_calistir();

    return 0;
}
