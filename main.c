#include "main.h"

/* reszta */
#include "watek_komunikacyjny.h"
#include "watek_glowny.h"
#include "queue.h"

/* wątki */
#include <pthread.h>

/* sem_init sem_destroy sem_post sem_wait */
// #include <semaphore.h>

/* flagi dla open */
//#include <fcntl.h>

/* Część zadaniowa */
int S; // liczba stanowisk
int N; // liczba Otaku
int M; // dopuszczalna liczba cuchów w pomieszczeniu
int X; // maksymalny limit cuchów do wymiany przewodniczącego
int aktualny_X = 0; // aktualnie naliczony limit cuchów
int cuchy; // moja wartość cuchów
int otrzymane_ACK = 0; // otrzymane zgody od innych procesów
int ubiegam_sie = FALSE; // wartość boolean -> czy ubiegam się o dostęp do kolejki
int wyzerowanie_kolejki = FALSE; // wartość boolean -> czy czekać na opróźnienie się kolejki
int policzono_X = FALSE; // wartość boolean -> czy uaktualniłem już limit cuchów
int wskaznik = -1; // pozycja, na której zakończone liczenie cuchów z kolejki
struct Queue *queue; // kolejka żądań

/* Część programowa */
state_t stan = InLobby;
int size, rank, zegar = 0;
MPI_Datatype MPI_PAKIET_T;
pthread_t threadKom, threadMon;

pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t zegarMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pokojMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t opuszczeniePokojuMut = PTHREAD_MUTEX_INITIALIZER;

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

/* srprawdza, czy są wątki, tworzy typ MPI_PAKIET_T
*/
void inicjuj(int argc, char **argv)
{
    /* Inicjalizacja części programowej */
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    const int nitems=FIELDNO; /* bo packet_t ma FIELDNO pól */
    int       blocklengths[FIELDNO] = {1,1,1};
    MPI_Datatype typy[FIELDNO] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[FIELDNO]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, cuchy);

    MPI_Type_create_struct(nitems, blocklengths, offsets, typy, &MPI_PAKIET_T);
    MPI_Type_commit(&MPI_PAKIET_T);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(rank);

    /* Inicjalizacja części zadaniowej */
    sscanf(argv[1], "%d", &S);
    N = size;
    sscanf(argv[2], "%d", &M);
    sscanf(argv[3], "%d", &X);
    queue = create_queue();
    cuchy = M / S;

    pthread_create( &threadKom, NULL, startKomWatek , 0);
    debug("jestem");
}

/* usunięcie zamkków, czeka, aż zakończy się drugi wątek, zwalnia przydzielony typ MPI_PAKIET_T
   wywoływane w funkcji main przed końcem
*/
void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(threadKom,NULL);
    if (rank==0) pthread_join(threadMon,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt = 0;
    if (pkt == 0) {
        pkt = malloc(sizeof(packet_t));
        freepkt=1;
    }
    pkt->src = rank;
    zegar += 1;
    pkt->ts = zegar;
    pkt->cuchy = cuchy;
    char *tag_str = tag == REQUEST ? "REQUEST" : tag == RELEASE ? "RELEASE" : "ACK";
    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    if(tag == RELEASE || tag == ACK) {
        debug("Wysłano %s do %d z zegarem:%d", tag_str, destination, zegar);
    }
    else {
        debug("Wysłano %s do %d z zegarem:%d i cuchami:%d", tag_str, destination, zegar, pkt->cuchy);
    }
    if (freepkt) free(pkt);
}

void changeState( state_t newState )
{
    pthread_mutex_lock( &stateMut );
    if (stan==InFinish) { 
	    pthread_mutex_unlock( &stateMut );
        return;
    }
    stan = newState;
    pthread_mutex_unlock( &stateMut );
}

int main(int argc, char **argv)
{
    /* Tworzenie wątków, inicjalizacja danych itp */
    inicjuj(argc, argv);
    
    // w pliku "watek_glowny.c"
    mainLoop();

    // koniec przetwarzania
    finalizuj();
    return 0;
}

