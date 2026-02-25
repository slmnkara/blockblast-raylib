# Proje: C ve Raylib ile Block Blast Klonu

## Proje Tanımı ve Hedefleri

Bu proje, Block Blast oyununun C ve Raylib kütüphanesi kullanılarak geliştirilmiş klonudur. Temel amaç, bellek yönetimi ve pointer manipülasyonu gibi düşük seviyeli yetkinlikleri oyun geliştirme dinamikleriyle birleştirerek modüler bir yapı ortaya koymaktır. Projede harici kütüphane kullanımı yasaktır.

## Teknik Kısıtlar ve Mimari

**Veri Yapıları:** Oyun elemanlarının struct yapıları ile tanımlanması.
**Hafıza Yönetimi:** malloc/free ile dinamik bellek kontrolü.
**Algoritmalar:** Çok boyutlu diziler, döngüler ve koşullu ifadeler.
**Pointerlar:** Fonksiyonlar arası veri aktarımı ve bellek manipülasyonu.
**Dosya Yönetimi (I/O):** Skor tablosunun ve durumun kaydedilmesi.
**Animasyon:** Sadece ham kod ve Raylib'in yerleşik zamanlayıcısı (GetFrameTime) ile timer mantığı kurulması.

## Modüler Ayrıştırma

**main.c:** Ana oyun döngüsü ve durum makinesi (Menü, Oyun, Bitiş).
**grid.c / grid.h:** Çok boyutlu dizi yönetimi, blok yerleştirme ve temizleme algoritmaları.
**shapes.c / shapes.h:** Blok yapıları, bellek tahsisi ve rastgele üretim mantığı.
**input.c / input.h:** Sürükle-bırak kontrolleri ve grid koordinat dönüşümleri.
**visuals.c / visuals.h:** Raylib texture render işlemleri, UI çizimleri.
**save.c / save.h:** Yüksek skor File I/O işlemleri.

## Mantıksal Akış

**Çarpışma Algılaması:** Sürüklenen matris ile hedef ızgara matrisinin pointer üzerinden çakışma testi.
**Izgara Temizleme ve Kombo:** Satır/sütun taraması. Puan = Taban x N x Kombo formülü ile skor hesabı. (N: Eşzamanlı temizlenen hat sayısı).
**Oyun Bitiş:** Mevcut blokların boşluklara sığmama durumunun arka planda simüle edilmesi.

## Teslimat ve Değerlendirme

Teslim edilecek materyaller: Kaynak kodlar, proje raporu, maksimum 5 dakikalık demo videosu ve canlı sunum dosyası. Her üye kodun tamamına hakim olmak zorundadır.

| Kriter | Ağırlık |
| --- | --- |
| Zorunlu Programlama Konseptlerinin Kullanımı | %25 |
| Sözlü Sunum ve Jüri Performansı | %25 |
| Kod Kalitesi ve Doğruluğu | %20 |
| Yaratıcılık ve Oyun Tasarımı | %15 |
| Rapor ve Dokümantasyon | %10 |
| Video Sunumu | %5 |

## Takım Görev Dağılımı (To-Do List)

Deneyimsiz üyelerin iş yükü görsel ve işitsel statik fonksiyonlar ile mantıksal veri tanımlamaları arasında eşitlenmiştir.

### Mukaddes (Dosya I/O)

* **Görev 1:** `save.c` modülünü yazmak, skorları .txt veya .dat formatında diske kaydetmek ve okumak.
* **Görev 2:** Ücretsiz ses (audio) dosyalarını projeye dahil edip oyun içi olaylara bağlamak.

### Berivan (Görselleştirme)

* **Görev 1:** Metinleri, skor tabelasını ve oyun sonu ekranı yazılarını Raylib ile çizdirmek.
* **Görev 2:** Blok dokularını (texture), bekleme tepsisini ve temel arka planı Raylib ile çizdirmek.

### Süleyman (Çekirdek Algoritmalar)

* **Görev 1:** `shapes.c` içerisinde kullanılacak blok türlerinin statik matris dizilerini tanımlamak.
* **Görev 2:** Alt kısımdaki bekleme tepsisinin (tray) veri mantığını ve yenilenmesini kurgulamak.
* **Görev 3:** Inputları alıp sürükle-bırak mekaniğini grid matrisine matematiksel olarak oturtmak (Snap to grid).
* **Görev 4:** Çarpışma algılama, satır/sütun temizleme ve oyun sonu kontrolü algoritmalarını yazmak.
* **Görev 5:** Pointer veri aktarım ağını kurmak ve malloc/free işlemlerini kontrol ederek bellek sızıntısını engellemek.
