#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"

int main() {
    int toplam_sarki = 0;
    
    izci_sifirla();
    
    Sarki* liste = csv_yukle("data/sarkilar.csv", 10000, &toplam_sarki);
    
    if (liste != NULL) {
        bellek_raporu_yazdir();
        /* Ödev PDF'sindeki zorunlu komut: */
        printf("sizeof(Sarki) = %zu\n", sizeof(Sarki));
        
        /* NOT: Ödevin Beklenen Çıktısında kasıtlı olarak SIZINTI gösterildiği 
           ve "Soru: Neden free: 0 kez görüyorsun?" dendiği için
           buradaki free (temizleme) işlemini özellikle YAPMIYORUZ. */
           
    } else {
        printf("HATA: data/sarkilar.csv okunamadi.\n");
    }

    return 0;
}
