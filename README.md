# MiniStream: C Tabanlı Müzik Platformu Bellek ve Performans Analizi

MiniStream, dinamik bellek yönetimi ve veri yapıları performansını analiz etmek amacıyla C dili ile geliştirilmiş bir müzik yayın platformu simülasyonudur. Proje, devasa boyutlardaki veri setleri üzerinde farklı mimari yaklaşımların ve veri yapılarının hız/bellek maliyetlerini kıyaslar.

**Geliştirici:** Samet Bayrak (2312101048)

---

## Öne Çıkan Özellikler

- **Bellek Yönetimi:** Geliştirilen özel `bellek_izci` modülü ve Valgrind testleri ile sistemde 0 byte bellek sızıntısı ve 0 bellek hatası garantilenmiştir. Çift silme (double-free) gibi açıklar `ref_sayisi` algoritması ile engellenmiştir.
- **Mimari Benchmark (Pointer vs. Kopya):** Şarkıları bellekte kopyalamak yerine bellek adreslerini (Pointer) paylaşan mimari ile bellek kullanımında **23.5x**, tahsisat (malloc) yükünde ise **17.6x** iyileşme sağlanmıştır.
- **Algoritma Benchmark (Hash Map vs. Linked List):** Hash Map O(1) yapısı (Chaining yöntemi ile) sayesinde arama hızı, Linked List O(n) yapısına göre veri seti 100.000 satıra ulaştığında **~218 katın** üzerinde performans farkı (991 ms yerine 4.5 ms) yaratır.
- **REST API ve Dashboard:** C kütüphanesi (.so) Python ile sarmalanarak, benchmark sonuçlarını JSON formatında sunan bir web sunucusu oluşturulmuştur.
- **Veri Üreteci:** Kendi sentetik (sanatçı ve albüm verileri ile) veri üretebilen `data/uretec.c` modülünü içerir.

---

## Veri Setine Erişim

Proje kapsamında kullanılan orijinal veri setine aşağıdaki bağlantıdan ulaşılabilir:

- **Kaynak:** [Spotify 1.2M+ Songs Dataset (Kaggle)](https://www.kaggle.com/datasets/rodolfofigueroa/spotify-12m-songs)

İndirilen veri setinin projenin kök dizinindeki `data/sarkilar.csv` yoluyla kullanılması gerekmektedir. Alternatif olarak proje içerisindeki veri üretici de kullanılabilir.

---

## Proje Yapısı

```text
MINISTREAM/
├── backend/            # Python REST API (server.py)
├── data/               # Veri setleri ve veri üretici (uretec.c)
├── rapor/              # Analiz dokümanları
│   ├── valgrind/       # Valgrind hata raporları (.txt)
│   └── RAPOR.md        # Detaylı mimari analiz raporu
├── src/                # C Kaynak kodları ve başlık dosyaları
├── test/               # C Birim test kodları
├── .gitignore          # Derlenmiş dosyaların engellenmesi
├── Makefile            # Derleme otomasyonu
└── README.md           # Proje tanıtım dosyası
```

---

## Kurulum ve Çalıştırma

### 1. Derleme
Projenin tüm bileşenlerini derlemek için:
```bash
make clean
make all
```

### 2. Testler
Birim testleri çalıştırmak için:
```bash
make test
```
Detaylı Valgrind analizleri için `make valgrind` komutu kullanılabilir.

### 3. Web Sunucusu
Dashboard arayüzünü başlatmak için:
```bash
make web
```
Erişim adresi: `http://localhost:8765`

---

## Özet Performans Analiz Raporu

**1. Çoğaltma Yükü (Kopya vs Pointer) (10.000 Şarkı x 50.000 İstek):**
- Bellek Kullanımı: **23.5x tasarruf** (85.64 MB vs 3.64 MB).
- Malloc Süresi: **2.4x daha hızlı**.
- Ayrılan Blok (Tahsisat): **17.6x daha az** Heap işgali.

**2. Arama Motoru Verimi (Linked List vs Hash Map) (100.000 Eleman):**
- Çözüm Hızı: **218.7x daha hızlı**. Linked List (~991ms) vs Hash Map (~4.5ms).

**3. Test Güvenliği:**
- Bellek izci: `SIZINTI: 0 byte`.
- Valgrind: `ERROR SUMMARY: 0 errors from 0 contexts`.
