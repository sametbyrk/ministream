/* ministream.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ministream.h"
#include "hash_map.h"
#include "linked_list.h"

/* ============================================================
   TEMEL SARKI OPERASYONLARI
   ============================================================ */

/* Heap'te yeni Sarki olustur, ref_sayisi = 0, sonraki = NULL */
Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci,
                     const char* album, int sure) {
    Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
    if (!s) return NULL;
    s->id         = id;
    s->ref_sayisi = 0;
    s->sonraki    = NULL;
    s->sure_sn    = sure;
    s->yil        = 0;
    strncpy(s->baslik,  baslik  ? baslik  : "", 99);
    strncpy(s->sanatci, sanatci ? sanatci : "", 99);
    strncpy(s->album,   album   ? album   : "", 99);
    s->baslik[99]  = '\0';
    s->sanatci[99] = '\0';
    s->album[99]   = '\0';
    return s;
}

/* ref_sayisi == 0 ise free et ve 0 dondur; degilse -1 dondur */
int sarki_sil(Sarki* sarki) {
    if (!sarki) return -1;
    if (sarki->ref_sayisi > 0) {
        printf("UYARI: '%s' hala %d listede kullaniliyor!\n",
               sarki->baslik, sarki->ref_sayisi);
        return -1;
    }
    izlened_free(sarki, sizeof(Sarki));
    return 0;
}

/* ============================================================
   CALMA LISTESI OPERASYONLARI
   ============================================================ */

/* Heap'te yeni liste olustur, baslangic kapasitesi 10 */
CalmaListesi* liste_olustur(int id, const char* isim) {
    CalmaListesi* l = (CalmaListesi*)izlenen_malloc(sizeof(CalmaListesi));
    if (!l) return NULL;
    l->id          = id;
    l->sarki_sayisi = 0;
    l->kapasite    = 10;
    strncpy(l->isim, isim ? isim : "", 49);
    l->isim[49]    = '\0';
    /* sarkilar dizisi: realloc ile buyuyecegi icin plain malloc kullan
       (izlened_malloc ile track etmek byte sayisini bozar) */
    l->sarkilar    = (Sarki**)malloc(l->kapasite * sizeof(Sarki*));
    if (!l->sarkilar) {
        izlened_free(l, sizeof(CalmaListesi));
        return NULL;
    }
    return l;
}

/* Listeye pointer ekle, ref_sayisi++, kapasite doluysa realloc ile 2x buyut */
int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki) {
    if (!liste || !sarki) return -1;
    if (liste->sarki_sayisi >= liste->kapasite) {
        liste->kapasite *= 2;
        liste->sarkilar = realloc(liste->sarkilar,
                                  liste->kapasite * sizeof(Sarki*));
        if (!liste->sarkilar) return -1;
    }
    liste->sarkilar[liste->sarki_sayisi++] = sarki;
    sarki->ref_sayisi++;
    return 0;
}

/* Indeksteki sarkiyi cikar, ref_sayisi--, son elemanla swap */
void liste_sarki_cikar(CalmaListesi* liste, int idx) {
    if (!liste || idx < 0 || idx >= liste->sarki_sayisi) return;
    liste->sarkilar[idx]->ref_sayisi--;   /* once azalt */
    /* son elemanla doldur, sayaci azalt */
    liste->sarkilar[idx] = liste->sarkilar[--liste->sarki_sayisi];
}

/* Listedeki tum sarkilarin ref_sayisi'ni azalt,
   ref_sayisi == 0 olan sarkilari free et,
   sarkilar dizisini ve CalmaListesi struct'ini free et */
void liste_temizle(CalmaListesi* liste) {
    if (!liste) return;
    for (int i = 0; i < liste->sarki_sayisi; i++) {
        liste->sarkilar[i]->ref_sayisi--;
        if (liste->sarkilar[i]->ref_sayisi == 0)
            izlened_free(liste->sarkilar[i], sizeof(Sarki));
    }
    /* sarkilar pointer dizisi: plain malloc ile ayrildi, plain free ile serbest birak */
    free(liste->sarkilar);
    liste->sarkilar = NULL;
    izlened_free(liste, sizeof(CalmaListesi));
}

/* ============================================================
   VERİ URETME FONKSIYONLARI
   ============================================================ */

/* n adet sarki olusturup linked list olarak dondur */
Sarki* veri_uret_liste(int n) {
    Sarki* bas = NULL;
    char baslik[32];
    for (int i = n - 1; i >= 0; i--) {
        snprintf(baslik, sizeof(baslik), "Track_%05d", i);
        Sarki* s = sarki_olustur(i, baslik, "Sanatci", "Album", 180);
        s->sonraki  = bas;
        bas = s;
    }
    return bas;
}

/* n adet sarki olusturup hash map'e ekleyip dondur */
HashMap* veri_uret_map(int n) {
    HashMap* map = hashmap_olustur();
    char baslik[32];
    for (int i = 0; i < n; i++) {
        snprintf(baslik, sizeof(baslik), "Track_%05d", i);
        Sarki* s = sarki_olustur(i, baslik, "Sanatci", "Album", 180);
        hashmap_ekle(map, s);
    }
    return map;
}

/* Linked list'teki tum sarkilari free et */
void liste_temizle_hepsi(Sarki* bas) {
    while (bas) {
        Sarki* sonraki = bas->sonraki;
        izlened_free(bas, sizeof(Sarki));
        bas = sonraki;
    }
}

/* ============================================================
   KOPYA vs POİNTER MODELI
   ============================================================ */

void kopya_modeli_test(int n_sarki, int n_liste, int sarki_per_liste) {
    /* 1. n_sarki adet orijinal sarki olustur (ozgun havuz) */
    Sarki** havuz = (Sarki**)malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        char baslik[32];
        snprintf(baslik, sizeof(baslik), "Track_%05d", i);
        havuz[i] = sarki_olustur(i, baslik, "Sanatci", "Album", 180);
    }

    /* 2. n_liste adet calma listesi olustur */
    CalmaListesi** listeler = (CalmaListesi**)malloc(n_liste * sizeof(CalmaListesi*));
    for (int i = 0; i < n_liste; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        for (int j = 0; j < sarki_per_liste; j++) {
            /* HER ekleme icin: malloc + memcpy -> yeni kopya */
            Sarki* kopya = (Sarki*)izlenen_malloc(sizeof(Sarki));
            memcpy(kopya, havuz[rand() % n_sarki], sizeof(Sarki));
            kopya->ref_sayisi = 0;
            kopya->sonraki    = NULL;
            /* listeye kopyayi ekle (ref_sayisi ile degil, dogrudan) */
            if (listeler[i]->sarki_sayisi >= listeler[i]->kapasite) {
                listeler[i]->kapasite *= 2;
                listeler[i]->sarkilar = realloc(listeler[i]->sarkilar,
                    listeler[i]->kapasite * sizeof(Sarki*));
            }
            listeler[i]->sarkilar[listeler[i]->sarki_sayisi++] = kopya;
        }
        /* listeyi ve TUM kopyalari free et */
        for (int j = 0; j < listeler[i]->sarki_sayisi; j++)
            izlened_free(listeler[i]->sarkilar[j], sizeof(Sarki));
        free(listeler[i]->sarkilar);
        izlened_free(listeler[i], sizeof(CalmaListesi));
    }
    free(listeler);

    /* havuzu free et */
    for (int i = 0; i < n_sarki; i++)
        izlened_free(havuz[i], sizeof(Sarki));
    free(havuz);
}

void pointer_modeli_test(int n_sarki, int n_liste, int sarki_per_liste) {
    /* 1. n_sarki adet orijinal sarki olustur */
    Sarki** havuz = (Sarki**)malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        char baslik[32];
        snprintf(baslik, sizeof(baslik), "Track_%05d", i);
        havuz[i] = sarki_olustur(i, baslik, "Sanatci", "Album", 180);
    }

    /* 2. n_liste adet calma listesi olustur */
    CalmaListesi** listeler = (CalmaListesi**)malloc(n_liste * sizeof(CalmaListesi*));
    for (int i = 0; i < n_liste; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        for (int j = 0; j < sarki_per_liste; j++) {
            /* Yeni malloc YOK - sadece pointer kopyalaniyor */
            liste_sarki_ekle(listeler[i], havuz[rand() % n_sarki]);
        }
        /* listeyi temizle (sarkilari free ETME, sadece ref_sayisi azalt) */
        for (int j = 0; j < listeler[i]->sarki_sayisi; j++)
            listeler[i]->sarkilar[j]->ref_sayisi--;
        free(listeler[i]->sarkilar);
        izlened_free(listeler[i], sizeof(CalmaListesi));
    }
    free(listeler);

    /* havuzu free et (ref_sayisi == 0 olanlar) */
    for (int i = 0; i < n_sarki; i++)
        izlened_free(havuz[i], sizeof(Sarki));
    free(havuz);
}

void deney_calistir(void) {
    int N_SARKI        = 10000;
    int N_LISTE        = 5000;
    int SARKI_PER_LISTE = 50;

    struct timespec t1, t2;

    printf("=== KOPYA MODELI ===\n");
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    kopya_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double kopya_ms = (t2.tv_sec - t1.tv_sec) * 1000.0 +
                      (t2.tv_nsec - t1.tv_nsec) / 1e6;
    bellek_raporu_yazdir();
    printf("Sure : %.2f ms\n", kopya_ms);

    printf("\n=== POINTER MODELI ===\n");
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pointer_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double pointer_ms = (t2.tv_sec - t1.tv_sec) * 1000.0 +
                        (t2.tv_nsec - t1.tv_nsec) / 1e6;
    bellek_raporu_yazdir();
    printf("Sure : %.2f ms\n", pointer_ms);

    printf("\n--- KARSILASTIRMA ---\n");
    if (kopya_ms > 0 && pointer_ms > 0)
        printf("Hiz farki : %.1fx daha hizli\n", kopya_ms / pointer_ms);
}

/* ============================================================
   CSV OKUMA
   ============================================================ */

/* Yardimci: CSV satirindan belirli bir alani cikar.
   Kaggle CSV'sinde alanlar virgülle ayrilir.
   Tirnaklarla sarılmıs alanlar olabilir. */
static void csv_alan_oku(const char* satir, int alan_no, char* hedef,
                         int max_uzunluk) {
    int alan = 0;
    int i    = 0;
    int tirnak_icinde = 0;

    /* Istenen alan numarasina kadar ilerle */
    while (alan < alan_no && satir[i] != '\0') {
        if (satir[i] == '"') tirnak_icinde = !tirnak_icinde;
        else if (satir[i] == ',' && !tirnak_icinde) alan++;
        i++;
    }

    /* Alani oku */
    int j = 0;
    tirnak_icinde = 0;
    while (satir[i] != '\0' && j < max_uzunluk - 1) {
        if (satir[i] == '"') { tirnak_icinde = !tirnak_icinde; i++; continue; }
        if (satir[i] == ',' && !tirnak_icinde) break;
        hedef[j++] = satir[i++];
    }
    hedef[j] = '\0';
}

/* CSV'den sarki oku, linked list olarak dondur.
   Kaggle kolon sirasi:
     0:id, 1:name, 2:album, 3:album_id, 4:artists,
     ..., 20:duration_ms, ..., 22:year */
Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam) {
    FILE* f = fopen(dosya_yolu, "r");
    if (!f) {
        printf("HATA: %s acilamadi!\n", dosya_yolu);
        return NULL;
    }

    char satir[2048];   /* Kaggle CSV satirlari uzun olabilir */
    fgets(satir, sizeof(satir), f);   /* ilk satir baslik, atla */

    Sarki* bas = NULL;
    *toplam    = 0;

    while (fgets(satir, sizeof(satir), f) && *toplam < limit) {
        Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
        s->id         = *toplam;
        s->ref_sayisi = 0;
        s->sonraki    = bas;

        /* Kaggle CSV kolon sirasi */
        csv_alan_oku(satir, 1,  s->baslik,  100);   /* name    */
        csv_alan_oku(satir, 2,  s->album,   100);   /* album   */
        csv_alan_oku(satir, 4,  s->sanatci, 100);   /* artists */

        char gecici[32];
        csv_alan_oku(satir, 20, gecici, 32);          /* duration_ms */
        s->sure_sn = atoi(gecici) / 1000;

        csv_alan_oku(satir, 22, gecici, 32);          /* year */
        s->yil = atoi(gecici);

        bas = s;
        (*toplam)++;
    }

    fclose(f);
    printf("%d sarki yuklendi: %s\n", *toplam, dosya_yolu);
    return bas;
}

/* ============================================================
   OPSİYONEL BACKEND - JSON CIKTI
   ============================================================ */
const char* deney_json(void) {
    static char buf[2048];
    int N_SARKI = 10000, N_LISTE = 5000, SARKI_PER_LISTE = 50;
    struct timespec t1, t2;

    /* 1. Bellek Modelleri Testi */
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    kopya_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double kopya_ms  = (t2.tv_sec-t1.tv_sec)*1000.0 + (t2.tv_nsec-t1.tv_nsec)/1e6;
    int kopya_malloc = izci_malloc_sayisi();
    size_t kopya_byte = izci_toplam_ayrildi();

    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pointer_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double pointer_ms  = (t2.tv_sec-t1.tv_sec)*1000.0 + (t2.tv_nsec-t1.tv_nsec)/1e6;
    int pointer_malloc = izci_malloc_sayisi();
    size_t pointer_byte = izci_toplam_ayrildi();

    /* 2. Arama Algoritmalari Testi (Gercel/Canli Deney) */
    double ll_ms[4], hm_ms[4];
    int boyutlar[] = {100, 1000, 10000, 100000};
    
    for (int i = 0; i < 4; i++) {
        int n = boyutlar[i];
        Sarki* bas   = veri_uret_liste(n);
        HashMap* map = veri_uret_map(n);
        
        int sorgu_sayisi = 1000;

        /* LinkedList O(n) OLCUMU */
        clock_gettime(CLOCK_MONOTONIC, &t1);
        for (int k = 0; k < sorgu_sayisi; k++)
            sarki_ara_liste(bas, rand() % n);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        ll_ms[i] = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) / 1e6;

        /* Hash Map O(1) OLCUMU */
        clock_gettime(CLOCK_MONOTONIC, &t1);
        for (int k = 0; k < sorgu_sayisi; k++)
            sarki_ara_map(map, rand() % n);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        hm_ms[i] = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) / 1e6;

        /* Temizlik */
        liste_temizle_hepsi(bas);
        hashmap_temizle(map);
    }

    snprintf(buf, sizeof(buf),
        "{\n"
        "  \"kopya\":   { \"malloc_sayisi\": %d, \"toplam_byte\": %zu, \"sure_ms\": %.2f },\n"
        "  \"pointer\": { \"malloc_sayisi\": %d, \"toplam_byte\": %zu, \"sure_ms\": %.2f },\n"
        "  \"bellek_farki\": %.1f,\n"
        "  \"hiz_farki\": %.1f,\n"
        "  \"arama_benchmark\": [\n"
        "    { \"n\": 100,     \"linked_list\": %.3f, \"hash_map\": %.3f, \"fark\": %.1f },\n"
        "    { \"n\": 1000,    \"linked_list\": %.3f, \"hash_map\": %.3f, \"fark\": %.1f },\n"
        "    { \"n\": 10000,   \"linked_list\": %.3f, \"hash_map\": %.3f, \"fark\": %.1f },\n"
        "    { \"n\": 100000,  \"linked_list\": %.3f, \"hash_map\": %.3f, \"fark\": %.1f }\n"
        "  ]\n"
        "}\n",
        kopya_malloc,   kopya_byte,   kopya_ms,
        pointer_malloc, pointer_byte, pointer_ms,
        kopya_byte  > 0 ? (double)kopya_byte  / pointer_byte  : 0,
        kopya_ms    > 0 ? kopya_ms / pointer_ms : 0,
        ll_ms[0], hm_ms[0], hm_ms[0] > 0 ? ll_ms[0] / hm_ms[0] : 0,
        ll_ms[1], hm_ms[1], hm_ms[1] > 0 ? ll_ms[1] / hm_ms[1] : 0,
        ll_ms[2], hm_ms[2], hm_ms[2] > 0 ? ll_ms[2] / hm_ms[2] : 0,
        ll_ms[3], hm_ms[3], hm_ms[3] > 0 ? ll_ms[3] / hm_ms[3] : 0);
    return buf;
}
