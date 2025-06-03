# 🏗️ Apartman İnşaatı Simülasyonu

**İşletim Sistemleri Dönem Projesi - Bahar 2024-2025**  

## 📌 Proje Hakkında

Bu proje, çok katlı bir apartman inşaatı sürecini modelleyerek **process (süreç)**, **thread (iş parçacığı)** ve **senkronizasyon mekanizmaları** gibi işletim sistemi kavramlarını simüle etmektedir.

- **Her kat bir process** olarak temsil edilir.
- **Her daire bir thread** olarak çalışır.
- **Ortak inşaat kaynakları** mutex’lerle korunur.
- **Katlar arası geçiş** semaphore'larla senkronize edilir.
- **Daireler arası koordinasyon**, condition variable yapıları ile sağlanır.

## 🧠 Kullanılan Teknolojiler

- Programlama Dili: **C**
- Çoklu İş Parçacığı: **POSIX Threads (pthread)**
- Senkronizasyon:
  - `semaphore.h` (katlar arası sıralama)
  - `pthread_mutex_t` (kaynak kilitleme)
  - `pthread_cond_t` (aşama eşzamanlama)
- Paylaşılan Bellek: `sys/shm.h`
- İşlem Yönetimi: `fork()`, `waitpid()`

## 🏗️ Sistem Mimarisi

```txt
Main Process
 ├─ Floor 1 Process
 │   ├─ Apartment 1 Thread
 │   ├─ Apartment 2 Thread
 │   ├─ Apartment 3 Thread
 │   └─ Apartment 4 Thread
 ├─ Floor 2 Process
 │   └─ ...
 .
 └─ Floor 10 Process
```

## 🔁 İnşaat Aşamaları

- Her daire şu 8 aşamadan geçer:
    1. Kaba İnşaat
    2. Su Tesisatı
    3. Elektrik Tesisatı
    4. Yalıtım
    5. HVAC (İklimlendirme)
    6. Sıva
    7. Zemin Döşeme
    8. İç Tasarım

- Her adımda ihtiyaç duyulan kaynaklar mutex ile korunur.

## 🛠️ Derleme ve Çalıştırma
- 🔧 Derleme
```txt
gcc -o apartman main_proje.c -lpthread -lrt
```
- ▶️ Çalıştırma
```txt
./apartman
```
## 📦 Dosya Yapısı
| Dosya Adı          | Açıklama                                       |
| ------------------ | ---------------------------------------------- |
| `main_proje.c`     | Projenin tüm kaynak kodlarını içeren C dosyası |
| `README.md`        | Proje açıklama ve kullanım kılavuzu            |
| `proje_raporu.pdf` | Projeye ait ayrıntılı teknik rapor             |

## 🧪 Örnek Çıktıdan Bir Kesit
```txt
Apartman İnşaatı Simülasyonu Başlıyor
Kat 1 inşaatı başlıyor.
Kat 1, Daire 1 inşaata başlıyor.
Kat 1, Daire 1, Kaba İnşaat için Vinç kaynağını talep ediyor...
Kat 1, Daire 1, Kaba İnşaat için Vinç kaynağını aldı.
...
Apartman inşaatı tamamlandı! Toplam 10 kat ve 40 daire başarıyla inşa edildi.
```
---
## 📸 Ekran Görüntüsü
<img src="https://github.com/ncrim7/Multi-Storey-Apartment-Building-Construction/blob/main/img.png" width="1000px" height="auto">. 
---
## ⚙️ Öğrenilen İşletim Sistemi Kavramları
- Process yönetimi

- Thread oluşturma ve yönetimi

- Kaynak paylaşımı ve senkronizasyon

- Yarış koşullarının önlenmesi

- Paylaşılan bellek kullanımı

## 📈 Geliştirilebilir Özellikler
- Dinamik kat/daire sayısı belirleme

- Kaynak kullanım analizi ve performans ölçümleri

- Deadlock tespiti ve çözüm algoritmaları

- Gerçek zamanlı ilerleme görselleştirmesi

##

🎓 Bu proje, işletim sistemi konularının gerçek hayat senaryoları ile nasıl modellenebileceğini göstermek amacıyla hazırlanmıştır.
