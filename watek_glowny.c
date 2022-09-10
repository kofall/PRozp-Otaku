#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"
#include "queue.h"

void mainLoop()
{
    int printed = FALSE;
    srand(time(NULL) + rank);
    while (stan != InFinish) {
        int perc = rand()%100; 
        if (perc < STATE_CHANGE_PROB) {
            if (stan==InLobby) {
                if (cuchy > M) {
                    if(!printed) {
                        printed = TRUE;
                        debug("Koniec przetwarzania (M:%d, moje_cuchy: %d)", M, cuchy);
                    }
                    continue;
                }
                pthread_mutex_unlock( &opuszczeniePokojuMut );
                changeState( InQueue );
                
                pthread_mutex_lock( &zegarMut );
                debug("Zmieniam stan na \"Ubiegam się\"");
                for (int i=0; i<size; i++){
                    sendPacket(0, i, REQUEST);
                }
                pthread_mutex_unlock( &zegarMut );
                pthread_mutex_lock( &pokojMut );
            } else if(stan == InQueue) {
                // Czekanie na odblokowanie mutexa w watku komunikacyjnym
                debug("Czekam na wejście do pokoju");
                pthread_mutex_lock( &pokojMut );
                pthread_mutex_lock( &opuszczeniePokojuMut );
                changeState( InRoom );
                debug("Zmieniam stan na \"Jestem w pokoju\"");
            } else if(stan == InRoom) {
                debug("Chcę wyjść z pokoju");
                pthread_mutex_lock( &zegarMut );
                for (int i=0; i<size; i++){
                    sendPacket(0, i, RELEASE);
                }
                pthread_mutex_unlock( &zegarMut );
                pthread_mutex_unlock( &pokojMut );
                pthread_mutex_lock( &opuszczeniePokojuMut );
                changeState( InLobby );
                debug("Zmieniam stan na \"Jestem poza pokojem\"");
            }
        }
        if (stan == InLobby)
            sleep(random() % MAX_SEC_IN_LOBBY + 1);
        else if (stan == InRoom)
            sleep(random() % MAX_SEC_IN_ROOM + 1);
    }
}
