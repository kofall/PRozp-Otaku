#include "main.h"
#include "watek_komunikacyjny.h"
#include "queue.h"

void updateX(int pos) {
    int my_idx = find_by_src(queue, rank);
    while(wskaznik < pos) {
        wskaznik++;
        struct part *request = get_by_id(queue, wskaznik);
        aktualny_X += request->cuchy;
        if(aktualny_X >= X) {
            debug("Proces %d przepełnił X (X:%d, aktualny_X:%d): zerowanie X", request->src, X, aktualny_X);
            aktualny_X = 0;
            if(ubiegam_sie && wskaznik != my_idx) {
                wyzerowanie_kolejki = TRUE;
            }
            if(ubiegam_sie && wskaznik >= my_idx) {
                policzono_X = TRUE;
            }
            break;
        }
    }
    if(ubiegam_sie && wskaznik >= my_idx) {
        policzono_X = TRUE;
    }
    debug("KONTROLA X (X:%d, aktualny_X:%d)", X, aktualny_X);
}

void check_if_can_enter() {
    debug("---SPRAWDZENIE WARUNKÓW WEJŚCIA DO SEKCJI KRYTYCZNEJ---");
    // Warunek ACK
    if (otrzymane_ACK < N) {
        debug("1. Otrzymano wszystkie ACK (ACK:%d, aktualne:%d): NIE", N, otrzymane_ACK);
        debug("---NIE ZEZWOLONO NA DOSTĘP---");
        return;
    } else {
        debug("1. Otrzymano wszystkie ACK (ACK:%d, aktualne:%d): TAK", N, otrzymane_ACK);
    }
    // Warunek S
    int moj_idx = find_by_src(queue, rank) + 1;
    if (moj_idx>S) {
        debug("2. Jest miejsce (S:%d, ja:%d): NIE", S, moj_idx);
        debug("---NIE ZEZWOLONO NA DOSTĘP---");
        return;
    } else {
        debug("2. Jest miejsce (S:%d, ja:%d): TAK", S, moj_idx);
    }
    // Warunek M
    int suma_cuchow = 0;
    for(int idx=0;idx<moj_idx;idx++)
    {
        struct part* element_z_kolejki = get_by_id(queue, idx);
        suma_cuchow+=element_z_kolejki->cuchy;
        if (suma_cuchow>M) {
            debug("3. Suma cuchów mniejsza lub równa od M (M:%d, aktualne:%d): NIE", M, suma_cuchow);
            debug("---NIE ZEZWOLONO NA DOSTĘP---");
            return;
        }
    }
    debug("3. Suma cuchów mniejsza lub równa od M (M:%d, aktualne:%d): TAK", M, suma_cuchow);
    // Warunek X
    if (wyzerowanie_kolejki == TRUE) {
        if(wskaznik > -1) {
            debug("4. Wyzerowano kolejkę: NIE");
            debug("---NIE ZEZWOLONO NA DOSTĘP---");
            return;
        } else {
            debug("4. Wyzerowano kolejkę: TAK");
        }
    } else if(!policzono_X){
        updateX(find_by_src(queue, rank));
        if (wyzerowanie_kolejki == TRUE) {
            if(wskaznik > -1) {
                debug("4. Wyzerowano kolejkę: NIE");
                debug("---NIE ZEZWOLONO NA DOSTĘP---");
                return;
            } else {
                debug("4. Wyzerowano kolejkę: TAK");
            }
        }
    }
    debug("---ZEZWOLONO NA DOSTĘP---");
    otrzymane_ACK = 0;
    ubiegam_sie = FALSE;
    policzono_X = FALSE;
    pthread_mutex_unlock(&pokojMut);
}

void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        pthread_mutex_lock( &zegarMut );
        zegar = (zegar > pakiet.ts ? zegar : pakiet.ts) + 1;
        pthread_mutex_unlock( &zegarMut );

        switch ( status.MPI_TAG ) {
	        case FINISH: 
                changeState(InFinish);
	            break;
            case ACK:
                debug("Otrzymano ACK od %d z zegarem:%d", pakiet.src, pakiet.ts);
                otrzymane_ACK++;
                if(otrzymane_ACK == N) {
                    check_if_can_enter();
                }
                break;
            case REQUEST:
                debug("Otrzymano REQUEST od %d z zegarem:%d i cuhami:%d", pakiet.src, pakiet.ts, pakiet.cuchy);
                push_by_time(queue, pakiet.ts, pakiet.src, pakiet.cuchy);
                print_queue(queue);
                pthread_mutex_lock( &zegarMut );
                sendPacket(0, pakiet.src, ACK);
                pthread_mutex_unlock( &zegarMut );
                
                if(pakiet.src == rank) {
                    ubiegam_sie = TRUE;
                }
                break;
            case RELEASE:;
                debug("Otrzymano RELEASE od %d z zegarem:%d", pakiet.src, pakiet.ts);
                int pos = find_by_src(queue, pakiet.src);
                if(wskaznik < pos) {
                    updateX(pos);
                }
                pop_by_src(queue, pakiet.src);
                print_queue(queue);
                wskaznik--;
                if(wskaznik == -1) {
                    wyzerowanie_kolejki = FALSE;
                }

                if(ubiegam_sie) {
                    check_if_can_enter();
                } else if(pakiet.src == rank) {
                    cuchy += rand() % MAX_CUCH_INCREASE + 1;
                    pthread_mutex_unlock( &opuszczeniePokojuMut );
                }
                break;
	        default:
	            break;
        }
    }
}
