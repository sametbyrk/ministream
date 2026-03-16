/* test/test_bellek.c */
#include <stdio.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"

/* Test 1: 10.000 sarki olustur, hepsini sil, 0 sızıntı */
void test_toplu_olustur_sil() {
    printf("\n[BELLEK TEST] toplu olustur/sil\n");
    izci_sifirla();

    for (int i = 0; i < 10000; i++) {
        Sarki* s = sarki_olustur(i, "Test", "Sanatci", "Album", 200);
        izlened_free(s, sizeof(Sarki));
    }

    bellek_raporu_yazdir();
    /* SIZINTI: 0 byte olmalı */
}

/* Test 2: 100 liste, her birine 50 ozel sarki ekle, temizle, 0 sızıntı */
void test_liste_bellek() {
    printf("\n[BELLEK TEST] liste bellek yonetimi\n");
    izci_sifirla();

    /* Her listenin kendi sarkilari var (ref_sayisi tam kontrol altinda) */
    for (int i = 0; i < 100; i++) {
        CalmaListesi* l = liste_olustur(i, "Liste");

        /* 50 sarki olustur ve listeye ekle - her sarki bu listede ref=1 */
        for (int j = 0; j < 50; j++) {
            char baslik[32];
            snprintf(baslik, sizeof(baslik), "Track_%d_%d", i, j);
            Sarki* s = sarki_olustur(i * 50 + j, baslik, "Artist", "Album", 200);
            liste_sarki_ekle(l, s);  /* ref_sayisi: 0 -> 1 */
        }

        /* liste_temizle: her sarki icin ref 1->0, otomatik free edilir */
        liste_temizle(l);
        /* Burada hic sarki ve liste bellekte kalmamali */
    }

    bellek_raporu_yazdir();
    /* SIZINTI: 0 byte olmali */
}

/* Test 3: Paylasimli sarki - birden fazla listede ayni sarki */
void test_paylasimli_sarki() {
    printf("\n[BELLEK TEST] paylasimli sarki ref yonetimi\n");
    izci_sifirla();

    /* 10 sarki olustur */
    Sarki* sarkilar[10];
    for (int i = 0; i < 10; i++)
        sarkilar[i] = sarki_olustur(i, "Shared", "Artist", "Album", 200);

    /* 5 liste olustur, her birinde tum 10 sarkiyi paylas */
    CalmaListesi* listeler[5];
    for (int i = 0; i < 5; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        for (int j = 0; j < 10; j++)
            liste_sarki_ekle(listeler[i], sarkilar[j]);  /* her sarki ref++ */
    }
    /* Her sarki 5 listede -> ref_sayisi = 5 */

    /* Tum listeleri temizle - ref 5->4->3->2->1->0 */
    for (int i = 0; i < 5; i++)
        liste_temizle(listeler[i]);
    /* Son liste temizlenince ref=0 olur, sarkilar otomatik free edilir */

    bellek_raporu_yazdir();
    /* SIZINTI: 0 byte olmali - sarkilar son liste_temizle'de free edildi */
}


int main() {
    printf("===== MiniStream Bellek Testleri =====\n");
    test_toplu_olustur_sil();
    test_liste_bellek();
    test_paylasimli_sarki();
    printf("\n=====================================\n");
    return 0;
}
