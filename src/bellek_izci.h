/* bellek_izci.h */
#ifndef BELLEK_IZCI_H
#define BELLEK_IZCI_H

#include <stdlib.h>

typedef struct {
    size_t toplam_ayrildi;  /* simdiye kadar malloc edilen toplam byte */
    size_t toplam_serbest;  /* simdiye kadar free edilen toplam byte */
    int    malloc_sayisi;   /* kac kez malloc cagirildi */
    int    free_sayisi;     /* kac kez free cagirildi */
} BellekIzci;

/* Tum projede bu iki fonksiyonu kullan, malloc/free'yi DOGRUDAN kullanma */
void*  izlenen_malloc(size_t boyut);
void   izlened_free(void* ptr, size_t boyut);

void   izci_sifirla(void);
void   bellek_raporu_yazdir(void);
size_t aktif_bellek(void);

/* Getter fonksiyonlari - izci degiskeni static oldugu icin
   test dosyalarindan dogrudan erisilemez. Bu fonksiyonlari kullan */
int    izci_malloc_sayisi(void);
int    izci_free_sayisi(void);
size_t izci_toplam_ayrildi(void);
size_t izci_toplam_serbest(void);

#endif /* BELLEK_IZCI_H */
