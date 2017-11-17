/******************************************************************************/
/*			Application: MASTERMIND			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : Jordan JEAN, Tim LEPAGE						      */
/*		Date : 14/04/2017						      */
/*									      */
/******************************************************************************/

#include <curses.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <time.h>

#include "fon.h" /* Primitives de la boite a outils */

#define SERVICE_DEFAUT "11111"

/* Structure définissant un coup */
typedef struct {
  char *prop;
  int blanc;
  int rouge;
} coup;

void serveur_appli(char *service); /* programme serveur */

void ajouter_coup(coup *liste[10], int cpt, char *prop, char *comb,
                  int n); /* ajoute un coup à la liste des coups */

void afficher_coups(
    coup *liste[10], int k, int n,
    int id_client); /* affiche les précédents coups sur la machine du client */

int present(int n, int i,
            char *c); /* Vérifie la présence de n dans la chaine c */

/******************************************************************************/
/*---------------- programme serveur ------------------------------*/

int main(int argc, char *argv[]) {

  char *service = SERVICE_DEFAUT; /* numero de service par defaut */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc) {
  case 1:
    printf("defaut service = %s\n", service);
    break;
  case 2:
    service = argv[1];
    break;

  default:
    printf("Usage:serveur service (nom ou port) \n");
    exit(1);
  }

  /* service est le service (ou num�ro de port) auquel sera affect�
  ce serveur*/

  serveur_appli(service);
}

/******************************************************************************/
void serveur_appli(char *service) {

  /* Procedure correspondant au traitemnt du serveur de votre application */
  /* Création d'une socket */
  int id_serveur, id_client, pid;
  id_serveur = h_socket(AF_INET, SOCK_STREAM);

  /*Renseignement de la structure sockaddr_in*/
  struct sockaddr_in *p_adr_socket = NULL;
  adr_socket(service, NULL, SOCK_STREAM, &p_adr_socket);

  /* Association à la socket */
  h_bind(id_serveur, p_adr_socket);

  /* Mise en l'écoute de la socket */
  h_listen(id_serveur, 10);

  while (1) {
    /* Récupération de la socket de communication avec un client */
    struct sockaddr_in *p_adr_client = malloc(sizeof(struct sockaddr_in));
    id_client = h_accept(id_serveur, p_adr_client);

    /* Création d'un processus fils */
    pid = fork();
    if (pid != 0) {
      /* Fermeture de la socket de communication */
      h_close(id_client);
    } else {
      /* Fermeture de la socket passive */
      h_close(id_serveur);
      /* Dialogue avec le client */

      /* Demande du nombre de couleur à deviner */
      char tamp[42] = "Entrer le nombre de couleurs à deviner : ";
      h_writes(id_client, tamp, 42);
      char nb[] = "";
      unsigned int n = 0;
      /* Lecture du nombre de couleurs */
      h_reads(id_client, nb, 2);
      sscanf(nb, "%d", &n);
      /* Génération d'une combinaison de chiffre entre 0 et 7 */
      char *comb = malloc(sizeof(int) * n);
      int alea;
      for (int i = 0; i < n; i++) {
        srand(time(NULL));
        do {
          alea = rand() % 8;
        } while (present(alea, i, comb));
        sprintf(&comb[i], "%d", alea);
      }
      printf("\nCombinaison : %s\n", comb);
      /* Demander au client une proposition */
      char jouer[27] = "Entrer une proposition : ";
      h_writes(id_client, jouer, 27);
      /* Initialisation des variables et de la liste des coups du client */
      int cpt = 0;
      int victoire = 0;
      coup *liste_coup[10];
      char c;
      char rejouer[2] = "r";
      /* Tant que le client n'a pas épuisé son nombre d'essais et qu'il n'a pas gagné */
      while (cpt < 10 && victoire != 1) {
        char *prop = malloc(sizeof(char) * n);
        /* On lit la proposition et on l'ajoute à la liste des coups */
        h_reads(id_client, prop, n);
        ajouter_coup(liste_coup, cpt, prop, comb, n);
        cpt++;
        if (!strcmp(comb, prop)) {
          victoire = 1;
        } else {
          /* On demande au client de rejouer et lui envoie la liste des coups mise à jour */
          h_writes(id_client, rejouer, 2);
          afficher_coups(liste_coup, cpt, n, id_client);
        }
      }
      /* On envoie au client un caractère signifiant si il a gagné ou perdu */
      char perdu[2] = "d";
      char gagne[2] = "v";
      if (cpt >= 10) {
        h_writes(id_client, perdu, 2);
      } else {
        h_writes(id_client, gagne, 2);
      }

      /* Fermeture de connexion client */
      h_close(id_client);

      /* Fin processus fils */
      exit(EXIT_SUCCESS);
    }
  }
}

/******************************************************************************/

void ajouter_coup(coup *liste[10], int k, char *prop, char *comb, int n) {
  coup *c = (coup *)malloc(sizeof(coup));
  int blanc = 0;
  int rouge = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (prop[i] == comb[j]) {
        if (i == j) {
          rouge++;
        } else {
          blanc++;
        }
      }
    }
  }
  c->blanc = blanc;
  c->rouge = rouge;
  c->prop = prop;
  liste[k] = c;
  return;
}

void afficher_coups(coup *liste[10], int k, int n, int id_client) {
  char nb_blanc[2];
  char nb_rouge[2];
  for (int i = 0; i < k; i++) {
    h_writes(id_client, liste[i]->prop, n);
    sprintf(nb_blanc, "%d", liste[i]->blanc);
    sprintf(nb_rouge, "%d", liste[i]->rouge);
    h_writes(id_client, nb_blanc, 2);
    h_writes(id_client, nb_rouge, 2);
  }
  return;
}

int present(int n, int i, char *c) {
  for (int j = 0; j < i; j++) {
    if (n == (c[j] - '0')) {
      return 1;
    }
  }
  return 0;
}
