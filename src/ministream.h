/* ministream.h */
#ifndef MINISTREAM_H
#define MINISTREAM_H

#include <stdlib.h>
#include "bellek_izci.h"

/* ---- Sarki ---- */
typedef struct Sarki {
    int  id;
    char baslik[100];   /* name  - sarki adi */
    char sanatci[100];  /* artists - sanatci adi */
    char album[100];    /* album - album adi */
    int  sure_sn;       /* duration_ms / 1000 -> saniyeye cevrilmis */
    int  yil;           /* year  - cikis yili */
    int  ref_sayisi;    /* kac calma listesinde kullaniliyor */
    struct Sarki* sonraki; /* linked list icin next pointer */
} Sarki;

/* ---- Calma Listesi ---- */
typedef struct {
    int     id;
    char    isim[50];
    Sarki** sarkilar;     /* heap'teki pointer dizisi - Sarki* dizisi */
    int     sarki_sayisi; /* su an listede kac sarki var */
    int     kapasite;     /* dizi kapasitesi (realloc ile buyur) */
} CalmaListesi;

/* ---- Kullanici ---- */
typedef struct {
    int            id;
    char           isim[50];
    CalmaListesi** listeler;     /* kullanicinin calma listeleri */
    int            liste_sayisi;
} Kullanici;

/* ==== Temel Operasyonlar ==== */
Sarki*        sarki_olustur(int id, const char* baslik,
                             const char* sanatci, const char* album,
                             int sure);
int           sarki_sil(Sarki* sarki);

CalmaListesi* liste_olustur(int id, const char* isim);
int           liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki);
void          liste_sarki_cikar(CalmaListesi* liste, int idx);
void          liste_temizle(CalmaListesi* liste);

/* ==== Benchmark Yardimcilari ==== */
Sarki*   veri_uret_liste(int n);
void     liste_temizle_hepsi(Sarki* bas);
void     kopya_modeli_test(int n_sarki, int n_liste, int sarki_per_liste);
void     pointer_modeli_test(int n_sarki, int n_liste, int sarki_per_liste);
void     deney_calistir(void);

/* ==== CSV Okuma ==== */
Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam);

/* ==== Opsiyonel Backend ==== */
const char* deney_json(void);

#endif /* MINISTREAM_H */
