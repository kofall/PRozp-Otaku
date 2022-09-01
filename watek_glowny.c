#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    while (stan != InFinish) {
        int perc = random()%100; 

        if (perc < STATE_CHANGE_PROB) {
            if (stan==InLobby) {
                debug("Zmieniam stan na ubieganie się");
                changeState( InQueue );
                /* TODO
                    Obsługa wysłania REQ
                */
                debug("Skończyłem wysyłać");
            } else if(stan == InQueue) {
                /* TODO
                    Sprawdzenie warunków wejścia do kolejki
                */
                changeState( InRoom );
                debug("Jestem w pokoju");
            } else if(stan == InRoom) {
                /* TODO
                    Obsługa wysłania RELEASE
                */
                changeState( InLobby );
                debug("Wyszedłem z pokoju");
            }
        }
        if (stan == InLobby)
            sleep(random() % MAX_SEC_IN_LOBBY + 1);
        else if (stan == InQueue)
            sleep(random() % MAX_SEC_IN_ROOM + 1);
    }
}
