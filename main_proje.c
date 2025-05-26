/**
 Bu program, 10 katlı ve her katta 4 daire bulunan bir apartmanın inşaat sürecini
 modellemektedir. Her kat bir process, her daire bir thread olarak temsil edilir.
 İnşaat sürecindeki senkronizasyon işlemleri için semaphore, mutex ve condition variable
 yapıları kullanılmıştır.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define TOPLAM_KAT 10    // Toplam kat sayısı
#define DAIRE 4  // Her katta bulunan daire sayısı
#define TOPLAM_DAIRE (TOPLAM_KAT * DAIRE)  // Toplam daire sayısı

#define INSAAT_ADIMLARI 8  // Her dairenin inşaat aşamaları sayısı

// İnşaat aşamaları
typedef enum {
    KABA_INSAAT = 0,  // Kaba inşaat
    SU_TESISATI = 1,            // Su tesisatı
    ELEKTRIK = 2,          // Elektrik tesisatı
    YALITIM = 3,          // Yalıtım işlemi (ses ve ısı)
    HVAC = 4,   // Havalandırma ve iklimlendirme sistemi
    SIVA = 5,          // Sıva işlemi
    DOSEME = 6,            // Zemin döşeme (parke, fayans vb.)
    DESIGN = 7      // İç tasarım ve dekorasyon
} Constructionadim;

// Ortak kaynaklar
typedef enum {
    VINC = 0,       // Vinç
    ASANSOR = 1,    // Asansör
    SU_KAYNAGI = 2, // Su kaynağı
    ELEKTRIK_KAYNAGI = 3, // Elektrik kaynağı
    YALITIM_EQUIPMENT = 4, // Yalıtım ekipmanları
    HVAC_TEAM = 5,   // HVAC uzman ekibi
    DOSEME_EKIPMANLARI = 6, // Zemin döşeme araçları
    TOPLAM_KAYNAK = 7  // Toplam kaynak sayısı
} Resource;

// Ortak kaynak isimlerini metinsel olarak saklama
const char* resource_names[TOPLAM_KAYNAK] = {
    "Vinç",
    "Asansör",
    "Su Kaynağı",
    "Elektrik Kaynağı",
    "Yalıtım Ekipmanları",
    "HVAC Uzman Ekibi",
    "Zemin Döşeme Araçları"
};

// Aşama isimlerini metinsel olarak saklama
const char* adim_names[INSAAT_ADIMLARI] = {
    "Kaba İnşaat",
    "Su Tesisatı",
    "Elektrik Tesisatı",
    "Yalıtım İşlemi",
    "HVAC Kurulumu",
    "Sıva İşlemi",
    "Zemin Döşeme",
    "İç Tasarım"
};

// Her daire için thread'in çalışma parametreleri
typedef struct {
    int kat_numara;            // Kat numarası (1-10)
    int daire_numara;        // Daire numarası (1-4)
    pthread_mutex_t *resource_mutexes;  // Ortak kaynaklar için mutex'ler
    pthread_mutex_t *floor_mutex;       // Kat içi senkronizasyon için mutex
    pthread_cond_t *floor_condition;    // Kat içi senkronizasyon için condition variable
    int *adim_completion;              // Her dairenin hangi aşamada olduğunu tutan dizi
} ApartmentParams;

// Paylaşılan bellek için yapı
typedef struct {
    int floor_complete[TOPLAM_KAT];  // Her katın tamamlanma durumu
    sem_t floor_semaphores[TOPLAM_KAT + 1];  // Kat semaphore'ları
} SharedMemory;

// Daire inşaat süreci fonksiyonu (thread tarafından çalıştırılır)
void* insa_et(void* args) {
    ApartmentParams* params = (ApartmentParams*)args;
    int floor = params->kat_numara;
    int apt = params->daire_numara;
    
    printf("Kat %d, Daire %d inşaata başlıyor.\n", floor, apt);
    
    // Her inşaat aşaması için
    for (int adim = 0; adim < INSAAT_ADIMLARI; adim++) {
        // İnşaat aşaması süresini (saniye) rastgele belirleme (1-3 saniye arası)
        int construction_time = (rand() % 3) + 1;
        
        // Kaynak ihtiyaçlarını belirleme
        Resource required_resource;
        switch (adim) {
            case KABA_INSAAT:
                required_resource = VINC;
                break;
            case SU_TESISATI:
                required_resource = SU_KAYNAGI;
                break;
            case ELEKTRIK:
                required_resource = ELEKTRIK_KAYNAGI;
                break;
            case YALITIM:
                required_resource = YALITIM_EQUIPMENT;
                break;
            case HVAC:
                required_resource = HVAC_TEAM;
                break;
            case SIVA:
                required_resource = ASANSOR;  // Sıva malzemeleri için asansör kullanılır
                break;
            case DOSEME:
                required_resource = DOSEME_EKIPMANLARI;
                break;
            case DESIGN:
                required_resource = ASANSOR;  // İç tasarım malzemeleri için asansör kullanılır
                break;
        }
        
        // Ortak kaynak için mutex kilidini al
        printf("Kat %d, Daire %d, %s için %s kaynağını talep ediyor...\n", 
               floor, apt, adim_names[adim], resource_names[required_resource]);
        pthread_mutex_lock(&params->resource_mutexes[required_resource]);
        
        printf("Kat %d, Daire %d, %s için %s kaynağını aldı.\n", 
               floor, apt, adim_names[adim], resource_names[required_resource]);
        
        // İnşaat aşamasını simüle et
        printf("Kat %d, Daire %d, %s aşaması başladı (%d saniye).\n", 
               floor, apt, adim_names[adim], construction_time);
        sleep(construction_time); // Kritik bölge - kaynak kullanımı
        printf("Kat %d, Daire %d, %s aşaması tamamlandı.\n", 
               floor, apt, adim_names[adim]);
        
        // Ortak kaynağı serbest bırak
        pthread_mutex_unlock(&params->resource_mutexes[required_resource]);
        printf("Kat %d, Daire %d, %s kaynağını serbest bıraktı.\n", 
               floor, apt, resource_names[required_resource]);
        
        // Bu dairenin tamamlanan aşamasını güncelle
        pthread_mutex_lock(params->floor_mutex);
        params->adim_completion[(apt-1) * INSAAT_ADIMLARI + adim] = 1;
        
        // Eğer bu aşama tüm dairelerde tamamlandıysa, signal gönder
        int all_complete = 1;
        for (int i = 0; i < DAIRE; i++) {
            if (params->adim_completion[i * INSAAT_ADIMLARI + adim] == 0) {
                all_complete = 0;
                break;
            }
        }
        
        if (all_complete) {
            printf("Kat %d'deki tüm dairelerde %s aşaması tamamlandı.\n", 
                   floor, adim_names[adim]);
            pthread_cond_broadcast(params->floor_condition);
        }
        pthread_mutex_unlock(params->floor_mutex);
        
        // Eğer su tesisatı, elektrik tesisatı, yalıtım veya HVAC aşamasındaysak,
        // tüm dairelerin bu aşamayı tamamlamasını beklemeliyiz
        if (adim == SU_TESISATI || adim == ELEKTRIK || adim == YALITIM || adim == HVAC) {
            pthread_mutex_lock(params->floor_mutex);
            while (1) {
                int everyone_done = 1;
                for (int i = 0; i < DAIRE; i++) {
                    if (params->adim_completion[i * INSAAT_ADIMLARI + adim] == 0) {
                        everyone_done = 0;
                        break;
                    }
                }
                
                if (everyone_done) break;
                
                printf("Kat %d, Daire %d, diğer dairelerin %s aşamasını tamamlamasını bekliyor.\n", 
                       floor, apt, adim_names[adim]);
                pthread_cond_wait(params->floor_condition, params->floor_mutex);
            }
            pthread_mutex_unlock(params->floor_mutex);
        }
    }
    
    printf("Kat %d, Daire %d inşaatı tamamlandı.\n", floor, apt);
    return NULL;
}

// Kat inşaat süreci (her process bu fonksiyonu çalıştırır)
void construct_floor(int kat_numara, SharedMemory* shared_mem) {
    printf("Kat %d inşaatı başlıyor.\n", kat_numara);
    
    // Alt katın tamamlanmasını bekle (Kat 1 dışındaki tüm katlar için)
    if (kat_numara > 1) {
        printf("Kat %d, Kat %d'in tamamlanmasını bekliyor...\n", kat_numara, kat_numara - 1);
        sem_wait(&shared_mem->floor_semaphores[kat_numara - 1]);
        printf("Kat %d tamamlandı, Kat %d inşaatına geçiliyor.\n", kat_numara - 1, kat_numara);
    }
    
    // Her kat için ortak kaynakların mutex'lerini oluştur
    pthread_mutex_t resource_mutexes[TOPLAM_KAYNAK];
    for (int i = 0; i < TOPLAM_KAYNAK; i++) {
        pthread_mutex_init(&resource_mutexes[i], NULL);
    }
    
    // Kat içi senkronizasyon için mutex ve condition variable
    pthread_mutex_t floor_mutex;
    pthread_cond_t floor_condition;
    pthread_mutex_init(&floor_mutex, NULL);
    pthread_cond_init(&floor_condition, NULL);
    
    // Her daire için thread oluştur
    pthread_t apartment_threads[DAIRE];
    ApartmentParams params[DAIRE];
    
    // Aşama tamamlanma durumlarını takip etmek için dizi
    int adim_completion[DAIRE * INSAAT_ADIMLARI] = {0};
    
    // Her daire için thread oluştur ve parametreleri hazırla
    for (int apt = 1; apt <= DAIRE; apt++) {
        params[apt-1].kat_numara = kat_numara;
        params[apt-1].daire_numara = apt;
        params[apt-1].resource_mutexes = resource_mutexes;
        params[apt-1].floor_mutex = &floor_mutex;
        params[apt-1].floor_condition = &floor_condition;
        params[apt-1].adim_completion = adim_completion;
        
        // Thread'i başlat
        pthread_create(&apartment_threads[apt-1], NULL, insa_et, &params[apt-1]);
    }
    
    // Tüm dairelerin tamamlanmasını bekle
    for (int apt = 0; apt < DAIRE; apt++) {
        pthread_join(apartment_threads[apt], NULL);
    }
    
    // Kaynakları temizle
    for (int i = 0; i < TOPLAM_KAYNAK; i++) {
        pthread_mutex_destroy(&resource_mutexes[i]);
    }
    pthread_mutex_destroy(&floor_mutex);
    pthread_cond_destroy(&floor_condition);
    
    // Bu katın tamamlandığını işaretle
    shared_mem->floor_complete[kat_numara - 1] = 1;
    
    // Bir sonraki katın başlayabileceğini bildir
    printf("Kat %d inşaatı tamamlandı. Kat %d inşaatına izin veriliyor.\n", kat_numara, kat_numara + 1);
    sem_post(&shared_mem->floor_semaphores[kat_numara]);
}

int main() {
    printf("Apartman İnşaatı Simülasyonu Başlıyor\n");
    printf("Toplam Kat: %d, Her Katta Daire: %d\n", TOPLAM_KAT, DAIRE);
    
    // Rastgele sayı üreteci başlat
    srand(time(NULL));
    
    // Paylaşılan bellek oluştur
    key_t key = ftok("apartment_simulation", 'R');
    int shmid = shmget(key, sizeof(SharedMemory), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    
    SharedMemory* shared_mem = (SharedMemory*)shmat(shmid, NULL, 0);
    if (shared_mem == (void*)-1) {
        perror("shmat failed");
        exit(1);
    }
    
    // Başlangıç durumunu ayarla
    for (int i = 0; i < TOPLAM_KAT; i++) {
        shared_mem->floor_complete[i] = 0;
    }
    
    // Katlar arası senkronizasyon için semaphore'ları başlat
    for (int i = 0; i <= TOPLAM_KAT; i++) {
        if (sem_init(&shared_mem->floor_semaphores[i], 1, (i == 0) ? 1 : 0) != 0) {
            perror("sem_init failed");
            exit(1);
        }
    }
    
    printf("Temel atma işlemi tamamlandı. Kat inşaatları başlıyor...\n");
    
    // Her kat için process oluştur
    pid_t child_pids[TOPLAM_KAT];
    
    for (int floor = 1; floor <= TOPLAM_KAT; floor++) {
        child_pids[floor-1] = fork();
        
        if (child_pids[floor-1] < 0) {
            perror("fork failed");
            exit(1);
        } else if (child_pids[floor-1] == 0) {
            // Çocuk process - bu katın inşaatını yap
            construct_floor(floor, shared_mem);
            exit(0);
        }
    }
    
    // Ana process tüm çocuk process'lerin tamamlanmasını bekler
    for (int floor = 0; floor < TOPLAM_KAT; floor++) {
        waitpid(child_pids[floor], NULL, 0);
    }
    
    printf("Apartman inşaatı tamamlandı! Toplam %d kat ve %d daire başarıyla inşa edildi.\n", 
           TOPLAM_KAT, TOPLAM_DAIRE);
    
    // Semaphore'ları temizle
    for (int i = 0; i <= TOPLAM_KAT; i++) {
        sem_destroy(&shared_mem->floor_semaphores[i]);
    }
    
    // Paylaşılan belleği temizle
    shmdt(shared_mem);
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}