/* hash_map.h */
#ifndef HASH_MAP_H
#define HASH_MAP_H

/* Forward declaration - tam tanim ministream.h'da */
typedef struct Sarki Sarki;

#define TABLO_BOYUTU 1024

typedef struct {
    Sarki* kovalar[TABLO_BOYUTU];
} HashMap;

int      hash_fonk(int id);
HashMap* hashmap_olustur(void);
void     hashmap_ekle(HashMap* map, Sarki* sarki);
Sarki*   sarki_ara_map(HashMap* map, int id);
void     hashmap_temizle(HashMap* map);

/* Benchmark yardimcisi - ministream.c'de implemente */
HashMap* veri_uret_map(int n);

#endif /* HASH_MAP_H */
