/* =================================================================== */
// Progrmame Client à destination d'un joueur qui doit deviner la case
// du trésor. Après chaque coup le résultat retourné par le serveur est
// affiché. Le coup consite en une abcsisse et une ordonnée (x, y).
/* =================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define N 10
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"

/* ====================================================================== */
/*                  Affichage du jeu en mode texte brut                   */
/* ====================================================================== */
void afficher_jeu(int jeu[N][N], int res, int points, int coups) {

    printf("\n************ TROUVEZ LE TRESOR ! ************\n");
    printf("    ");
    for (int i=0; i<10; i++)
        printf("  %d ", i+1);
    printf("\n    -----------------------------------------\n");
    for (int i=0; i<10; i++){
        printf("%2d  ", i+1);
        for (int j=0; j<10; j++) {
            printf("|");
            switch (jeu[i][j]) {
                case -1:
                    printf(" 0 ");
                    break;
                case 0:
                    printf(GREEN " T " RESET);
                    break;
                case 1:
                    printf(YELLOW " %d " RESET, jeu[i][j]);
                    break;
                case 2:
                    printf(RED " %d " RESET, jeu[i][j]);
                    break;
                case 3:
                    printf(MAGENTA " %d " RESET, jeu[i][j]);
                    break;
            }
        }
        printf("|\n");
    }
    printf("    -----------------------------------------\n");
    printf("Pts dernier coup %d | Pts total %d | Nb coups %d\n", res, points, coups);
}


#define SERVEURNAME "127.0.0.1"
int to_server_socket = -1;


/* ====================================================================== */
/*                    Fonction principale                                 */
/* ====================================================================== */
int main(int argc, char **argv) {

    int jeu[N][N];
    int lig, col;
    int res = -1, points = 0, coups = 0;

    /* Init args */


    struct sockaddr_in serverSockAddr;

    long hostAddr;

    char buffer[512]; 

    /* Init jeu */
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            jeu[i][j] = -1;


    /* Init caracteristiques serveur distant (struct sockaddr_in) */
    bzero(&serverSockAddr,sizeof(serverSockAddr));
    hostAddr = inet_addr(SERVEURNAME);
    if ( (long)hostAddr != (long)-1)
        bcopy(&hostAddr,&serverSockAddr.sin_addr,sizeof(hostAddr));

    serverSockAddr.sin_port = htons(5555);
    serverSockAddr.sin_family = AF_INET;

    /* creation de la socket */
    if ( (to_server_socket = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Probleme de la creation socket client\n");
        exit(0);
    }

    /* Etablissement connexion TCP avec process serveur distant */
    if(connect( to_server_socket,
                (struct sockaddr *)&serverSockAddr,
                sizeof(serverSockAddr)) < 0 )
    {
        printf("Probleme de la demande de connection\n");
        exit(0);
    }


    /* Tentatives du joueur : stoppe quand tresor trouvé */
    do {
        /* Construction requête (serialisation en chaines de caractères) */
        afficher_jeu(jeu, res, points, coups);
        printf("\nEntrer le numéro de ligne : ");
        scanf("%d", &lig);
        printf("Entrer le numéro de colonne : ");
        scanf("%d", &col);
        sprintf(buffer, "%d %d", lig, col);


        /* Envoi de la requête au serveur (send) */
        send(to_server_socket, buffer, 512, 0);

        
        /* Réception du resultat du coup (recv) */
        recv(to_server_socket, buffer, 512, 0);

        /* Deserialisation du résultat en un entier */
        sscanf(buffer, "%d", &res);
        

        /* Mise à jour */
        if (lig>=1 && lig<=N && col>=1 && col<=N)
            jeu[lig-1][col-1] = res;
        points += res;
        coups++;

    } while (res);

    /* Fermeture connexion TCP */
    shutdown(to_server_socket,2);
    close(to_server_socket);

    /* Terminaison du jeu : le joueur a trouvé le tresor */
    afficher_jeu(jeu, res, points, coups);
    printf("\nBRAVO : trésor trouvé en %d essai(s) avec %d point(s)"
            " au total !\n\n", coups, points);
    return 0;
}
