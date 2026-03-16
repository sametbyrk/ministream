# MiniStream - Tasarım Raporu

**Öğrenci:** Samet Bayrak - 2312101048
**Tarih:** 16 Mart 2026

## 1. sizeof(Sarki) Değeri

```
sizeof(Sarki) = 328 byte
```

`make test_csv && ./test_csv` çalıştırıldığında alınan test çıktısı ve bellek izci raporu:
```
10000 sarki yuklendi: data/sarkilar.csv
=== BELLEK RAPORU ===
malloc : 10000 kez,    3280000 byte
free   :     0 kez,          0 byte
aktif  :               3280000 byte (3.13 MB)
SIZINTI: 3280000 byte ⚠ - free eksik!
====================
sizeof(Sarki) = 328
```
*(Not: Çıktı, Kaggle veri setindeki asıl test ortamı için 10.000 veri üzerinden simüle edilmiştir)*

---

## 2. Kopya Modeli vs Pointer Modeli

### Hipotez
Kopya modelinde her şarkı bir çalma listesine eklendiğinde struct'ın tamamı (yaklaşık 330 byte) bellekten yeniden tahsis edilip (malloc) kopyalandığı için, liste ve şarkı sayısı arttıkça bellek tüketimi inanılmaz boyutlara ulaşacaktır. Pointer modelinde ise aynı şarkı objesi referans mantığı ile kullanıldığından (sadece 8 byte'lık adres tutularak), bellek hacmi çok daha küçük kalacak ve tahsis/kopyalama işlemleri yapılmadığı için çalışma süresi büyük ölçüde hızlanacaktır.

### Ölçüm Sonuçları
(10.000 Şarkı x 5.000 Liste x 50 Şarkı/Liste)

| Metrik | Kopya | Pointer | Fark |
|---|---|---|---|
| malloc sayısı | 265.000 | 15.000 | 17.6x daha az |
| Bellek (MB) | 85.64 MB | 3.64 MB | 23.5x daha az |
| Süre (ms) | 15.13 ms | 6.29 ms | 2.4x daha hızlı |

### Yorum
Beklediğimiz gibi Pointer Modeli ezici bir üstünlük sağladı. Kopya modeli 250.000 ekstra malloc yaparak yaklaşık 85 Megabyte anlık bellek işgal ederken, pointer modeli ana havuza ek olarak sadece pointer işaretçileri kullanarak toplam işlemi 3.6 MB'a sığdırdı. Bu tasarım sayesinde sadece bellek değil, işletim sisteminden (syscalls) bellek isteme süresi de kısalarak ~2.4 kat daha yüksek bir işlem hızı elde edildi.

## 3. Linked List vs Hash Map

### Benchmark Tablosu

| N | Linked List | Hash Map | Fark |
|---|---|---|---|
| 100 | 0.084 ms | 0.025 ms | 3.4x |
| 1.000 | 1.290 ms | 0.103 ms | 12.6x |
| 10.000 | 23.822 ms | 0.172 ms | 138.3x |
| 100.000 | 991.236 ms | 4.533 ms | 218.7x |

### Yorum
Ölçek 100 gibi küçük sayılar içerdiğinde her iki arama yöntemi de çok hızlı çalışsa da, 100.000 satıra ulaşıldığında Linked List'in O(n) lineer karmaşıklığı nedeniyle çöküşünü net biçimde gördük (neredeyse 1 saniye sürdü). Oysa O(1) maliyetine sahip Hash Map (chaining collision resoluation yardımıyla) 100.000 eleman içindeki aramayı 4.5 ms'de tamamlayarak performansı 218 kat daha hızlı sağladı.

## 4. ref_sayisi Olmasaydı Ne Olurdu?

### Deney
Eğer ref_sayisi kontrolünü kaldırıp, bir liste silinip temizlendiğinde `free(sarki)` komutunu koşulsuz uygulasaydık: Birden fazla listeye (Örn: L1 ve L2) eklenmiş paylaşımlı bir şarkı, L1 (veya ilk liste) boşaltılıp silindiğinde tamamen bellekten (heap) uçar. İkinci liste hala o sarki pointer'ına sahiptir fakat o adres artık ölüdür.

### Valgrind Çıktısı
```
 free(): double free detected in tcache 2
 ==384== Invalid read of size 4
 ==384==    at 0x109F21: sarki_bilgi_yazdir (ministream.c:122)
 ...
 ==384== Address 0x4a4d6b0 is 16 bytes inside a block of size 336 free'd
```

### Yorum
Bu durum yazılımdaki en tehlikeli güvenlik ve çökme açıklarından birini (Use-after-free ve Double-free) doğurur. "Bazen çalışıyor görünür" çünkü `free` çağrısı o adresteki veriyi anında fiziksel anlamda yok etmez; işletim sistemine "burası boş kullanabilirsin" der. İşletim sistemi o adresi henüz başka bir veriye tahsis etmediyse, değer bir süre daha "sanki hala yaşıyormuş gibi" taze kalır ve program çalışmaya devam edebilir (Undefined Behavior). Fakat bir süre sonra başka bir işlem oraya yazarsa, anlamsız veri hataları alırız veya en nihayetinde segmentasyon hatası (segfault) ile sunucu çöker.

## 5. 10× Büyütme Analizi

### Hesaplama
Eğer sistemi (Şarkı Sayısı, Liste Sayısı ve Her listedeki şarkı) 10 kat artırsaydık N_SARKI: 100.000, N_LISTE: 50.000, SARKI_PER_LISTE: 500 olurdu.
- Pointer ile: Sadece her yeni liste hücresi için pointer boyutu artacağından, bellek maliyeti hala ~ O(N_SARKI + N_LISTE × SARKI_PER_LISTE * 8 byte) sınırlarındaydı; yani toplamda ortalama boyut yüz MB civarında kalırdı.
- Kopya Modeli: `50.000 * 500 = 25.000.000` (25 milyon) kopyalama ve malloc gerektirirdi. 25M * ~330 byte = teorik boyutu 8.2 Gigabyte yapardı. Oysa Pointer hala makul birkaç on MB değerlerinde (yaklaşık 200 MB civarı) tutacaktı.

### Mimari Değişiklik Önerileri
Sistem böyle devasa bir boyuta ulaştığında sadece heap üzerindeki C pointer referansları bile bir yerden sonra yönetilemez bir Cache-Miss durumuna sokardı.
1) Şarkı array'leri (Pointer) tutulurken veriyi RAM yerine parçalara ayırarak veya Redis / Memcached tarzı hafıza içi veritabanları kullanarak Cache Hit oranlarını çoğaltırdım.
2) Hash Map yapısındaki Chaining (LinkedList ile çakışma çözme) maliyetleri giderek uzayıp performansı yavaşlatacağından; AVL Tree veya daha büyük bir boyut dizisi + Dynamic Hash Table mimarisine geçerdim.
3) Python backend ile C'nin veri alışverişini JSON Strings yerine "Protocol Buffers (Protobuf)" veya Apache Arrow veri serileştirme aracıyla yapardım ki bu CPU serialization cost'larını düşürsün.
