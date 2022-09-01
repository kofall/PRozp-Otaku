#include "main.h"
#include "watek_komunikacyjny.h"
#include "queue.h"

extern pthread_mutex_t zegarMut;
extern struct Queue* queue;

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
/* TODO:
    -   RECEIVE REQUEST
    -   RECEIVE RELEASE
*/
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	    debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        pthread_mutex_lock( &zegarMut );
        zegar = (zegar > pakiet.ts ? zegar : pakiet.ts) + 1;
        pthread_mutex_unlock( &zegarMut );

        switch ( status.MPI_TAG ) {
	        case FINISH: 
                changeState(InFinish);
	            break;
            /* TODO
            case REQUEST:
                break;
            case RELEASE;
                break;
            */
	        default:
	            break;
        }
    }
}
