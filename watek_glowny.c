#include "main.h"
#include "watek_glowny.h"
#include "queue.h"

void mainLoop()
{
    srandom(rank);
    while (stan != InFinish) {
        int perc = random()%100; 

        if (perc < STATE_CHANGE_PROB) {
            if (stan==InLobby) {
                debug("Zmieniam stan na ubieganie się");
                if (cuchy > M)
                {
                    continue; //Dodać losową zmiene wartości cuchów tak żeby ten warunek mógl w końcu nie być spełniony
                }
                ubiegam_sie = 1;
                changeState( InQueue );
                for (int i=0; i<size; i++){
                    if(i!=rank){
                        sendPacket(0, i, REQUEST);
                    }
                }
                pthread_mutex_lock( &pokojMut );
                debug("Skończyłem wysyłać requesty.");
            } else if(stan == InQueue) {
                // Czekanie na odblokowanie mutexa w watku komunikacyjnym
                pthread_mutex_lock( &pokojMut );
                changeState( InRoom );
                debug("Jestem w pokoju");
            } else if(stan == InRoom) {
                for (int i=0; i<size; i++){
                    sendPacket(0, i, RELEASE);
                }
                if(policzono_X != TRUE){
                    struct part* wartoscSpodWskaznika;
                    wartoscSpodWskaznika=find(queue, wskaznik);
                    while(wartoscSpodWskaznika->src!=rank)
                    {
                        aktualny_X+=wartoscSpodWskaznika->cuchy;
                        if (aktualny_X>=X) aktualny_X=0;
                        wskaznik++;
                        wartoscSpodWskaznika=find(queue, wskaznik);
                    }
                }
                delete_idx(queue, wskaznik);
                cuchy+=random()%cuchy;
                wskaznik--;
                policzono_X=FALSE;
                
                changeState( InLobby );
                pthread_mutex_unlock( &pokojMut );
                debug("Wyszedłem z pokoju");
            }
        }
        if (stan == InLobby)
            sleep(random() % MAX_SEC_IN_LOBBY + 1);
        else if (stan == InQueue)
            sleep(random() % MAX_SEC_IN_ROOM + 1);
    }
}
