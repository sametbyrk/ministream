/* hash_map.c */
#include <stdio.h>
#include "ministream.h"
#include "hash_map.h"
#include "linked_list.h"
#include "bellek_izci.h"


int hash_fonk(int id) {
    return id % TABLO_BOYUTU;
}

/* Bos hash map olustur, tum kovalari NULL'a ayarla */
HashMap* hashmap_olustur(void) {
    HashMap* map = (HashMap*)izlenen_malloc(sizeof(HashMap));
    for (int i = 0; i < TABLO_BOYUTU; i++)
        map->kovalar[i] = NULL;
    return map;
}

/* Hash map'e sarki ekle - chaining yontemi:
   Ayni kova indeksine dusen sarkilar, linked list olarak
   birbirine baglanir (sonraki pointer'i ile). */
void hashmap_ekle(HashMap* map, Sarki* sarki) {
    int idx = hash_fonk(sarki->id);
    sarki->sonraki = map->kovalar[idx]; /* mevcut basi sonraya al */
    map->kovalar[idx] = sarki;          /* yeni sarkiyi basa koy */
}

/* Hash map uzerinde arama - O(1) ortalama */
Sarki* sarki_ara_map(HashMap* map, int id) {
    int idx = hash_fonk(id);
    return sarki_ara_liste(map->kovalar[idx], id);
}

/* Hash map'i temizle - tum kovalari ve sarkilari free et */
void hashmap_temizle(HashMap* map) {
    for (int i = 0; i < TABLO_BOYUTU; i++) {
        Sarki* curr = map->kovalar[i];
        while (curr) {
            Sarki* sonraki = curr->sonraki;
            izlened_free(curr, sizeof(Sarki));
            curr = sonraki;
        }
    }
    izlened_free(map, sizeof(HashMap));
}
