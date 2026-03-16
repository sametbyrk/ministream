# 🎵 MiniStream: C Tabanlı Müzik Platformu Bellek ve Performans Analizi

MiniStream, dinamik bellek yönetimi ve veri yapıları performansını analiz etmek amacıyla C dili ile geliştirilmiş bir müzik yayın platformu simülasyonudur. Proje, devasa boyutlardaki veri setleri üzerinde farklı mimari yaklaşımların ve veri yapılarının hız/bellek maliyetlerini kıyaslar.

**Geliştirici:** Samet Bayrak (2312101048)

---

## ✨ Öne Çıkan Özellikler

- 🛡️ **Kusursuz Bellek Yönetimi:** Geliştirilen özel `bellek_izci` modülü ve Valgrind testleri ile sistemde 0 byte bellek sızıntısı ve 0 bellek hatası garantilenmiştir. Çift silme (double-free) gibi açıklar `ref_sayisi` algoritması ile engellenmiştir.
- 🚀 **Mimari Benchmark (Pointer vs. Kopya):** Şarkıları bellekte kopyalamak yerine bellek adreslerini (Pointer) paylaşan mimari ile bellek kullanımında **23.5x**, tahsisat (malloc) yükünde ise **17.6x** iyileşme sağlanmıştır.
- ⚡ **Algoritma Benchmark (Hash Map vs. Linked List):** Hash Map $O(1)$ yapısı (Chaining yöntemi ile) sayesinde arama hızı, Linked List $O(n)$ yapısına göre veri seti 100.000 satıra ulaştığında **~218 katın** üzerinde muazzam bir devasa performans farkı (991 ms yerine 4.5 ms) yaratır.
- 🌐 **REST API & Canlı Dashboard:** C kütüphanesi (`.so`) Python ile wrapper ile sarmalanarak, benchmark sonuçlarını JSON formatında sunan özel bir web sunucusu oluşturulmuştur.
- 🎲 **Özgün Veri Üreteci:** Kendi sentetik (anlamlı sanatçı ve albüm verileri ile) 10.000'lerce satır csv verisi üretebilen `data/uretec.c` modülünü içerir.

---

## 📂 Proje Yapısı

```text
MINISTREAM/
├── backend/            # Python REST API (server.py)
├── data/               # Müzik CSV veri setleri ve kendi veri üreticimiz (uretec.c)
├── rapor/              # Analiz dokümanları
│   ├── valgrind/       # Valgrind tam hata/sızıntı raporları (.txt)
│   └── RAPOR.md        # Detaylı mimari analiz ve matematiksel sonuç raporu
├── src/                # C Kaynak kodları ve başlık dosyaları
│   ├── bellek_izci.c/h # Malloc ve free takipleri
│   ├── hash_map.c/h    # Hızlı arama motoru
│   ├── linked_list.c/h # Geleneksel Arama yapısı
│   └── ministream.c/h  # Ana iş mantığı
├── test/               # C Birim (Unit Test) test kodları
├── .gitignore          # Derlenmiş .exe/.so dosyalarının git bloklaması
├── Makefile            # Kolay derleme otomasyonu
└── README.md           # Proje dokümantasyonu (Bu dosya)
```

---

## 🚀 Kurulum ve Çalıştırma

### 1. Derleme
Projenin tüm testlerini ve yardımcı programlarını tek kalemde derlemek için:
```bash
make clean
make all
```

### 2. Testleri Çalıştırma (Valgrind ile Zorunlu Sızıntı Testi)
Tüm birim testleri (.exe / .out sürümlerini) anlık çalıştırıp konsolda görmek için:
```bash
make test
```
*(Valgrind detaylı analizi için `make valgrind` veya `make valgrind-benchmark` komutları RAPOR.md içerisinde kanıt olarak sunulmuştur).*

### 3. Dashboard Web Sunucusu (Görsel İnceleme)
Hocamızın projenin terminal çıktılarını ve benchmark grafikleri ile **0 sızıntı raporlarını** tek tıkla görebilmesi için oluşturulmuştur:
```bash
make web
```
Ardından herhangi bir tarayıcıda şu adrese gidin: `http://localhost:8765`

---

## 📊 Özet Performans Analiz Raporu

**1. Çoğaltma Yükü (Kopya vs Pointer) (10.000 Şarkı x 50.000 İstek):**
- Bellek Kullanımı: **23.5x tasarruf.** (85.64 MB vs 3.64 MB)
- Malloc Süresi: **2.4x daha hızlı.**
- Ayrılan Blok (Tahsisat): **17.6x daha az** Heap işgali.

**2. Arama Motoru Verimi (Linked List vs Hash Map) (100.000 Eleman):**
- Çözüm Hızı: **218.7x daha hızlı**. Linked List (~991ms) vs Hash Map (~4.5ms).

**3. Test Güvenliği:**
- Bellek izci: `SIZINTI: 0 byte` (Her ikisinde de).
- Valgrind: `ERROR SUMMARY: 0 errors from 0 contexts`. 
