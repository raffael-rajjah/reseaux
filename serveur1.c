/* =================================================================== */
// Progrmame Serveur qui calcule le résultat d'un coup joué à partir
// des coordonnées reçues de la part d'un client "joueur".
// Version ITERATIVE : 1 seul client/joueur à la fois
/* =================================================================== */

#include <stdio.h>

/* =================================================================== */
/* FONCTION PRINCIPALE : SERVEUR ITERATIF                              */
/* =================================================================== */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "tresor.h"
#include <string.h>
#include <time.h>



char buffer[512];
int ma_socket;

int main ( void )
{
    int client_socket;
    struct sockaddr_in mon_address, client_address;
    int mon_address_longueur;
    int x_tresor = 4, y_tresor = 5;
    int xp, yp;


    // pour randomiser les coordonnées du trésor
    // srand(time(NULL));
    // x_tresor = 1 + rand()%10;
    // y_tresor = 1 + rand()%10;


    int res;

    bzero(&mon_address,sizeof(mon_address)); 
    mon_address.sin_port = htons(5555); 
    mon_address.sin_family = AF_INET;
    mon_address.sin_addr.s_addr = htonl(INADDR_ANY);


    /* creation de socket */
    if ((ma_socket = socket(AF_INET,SOCK_STREAM,0))== -1)
    {
        printf("Probleme avec la creation\n");
        return 0;
    }

    /* bind serveur - socket */
    bind(ma_socket,(struct sockaddr *)&mon_address,sizeof(mon_address));
    

    listen(ma_socket,5);

    mon_address_longueur = sizeof(client_address);
    client_socket = accept(ma_socket,
                          (struct sockaddr *)&client_address,
                           &mon_address_longueur);


        while(1){

            if(recv(client_socket, buffer, 512, 0) == -1){
                printf("ERROR recv");

                return 0;
            }

            if(strcmp(buffer, "0") == 0){ // si le client est déconnecté, fermer le socket et attendre un nouveau client
                
                memset(buffer, 0, sizeof(buffer));

                shutdown(client_socket,2);
                close(client_socket);

                printf("client disconnected");

                client_socket = accept(ma_socket,
                                (struct sockaddr *)&client_address,
                                &mon_address_longueur);

                continue;


            }

            printf("le serveur a recu: %s\n",buffer);



            sscanf(buffer, "%d %d",&xp, &yp);


            res = recherche_tresor(10, x_tresor, y_tresor, xp, yp);

            sprintf(buffer, "%d", res);

            printf("envoyer : %s\n", buffer);

            
            send(client_socket,buffer, 512, 0);

        }

}


