/******************************************************************************/
/*			Application: ... */
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/

#include <curses.h> /* Primitives de gestion d'�cran */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>

#include "fon.h" /* primitives de la boite a outils */

#define SERVICE_DEFAUT "11111"
#define SERVEUR_DEFAUT "127.0.0.1"

void client_appli(char *serveur, char *service);

/*****************************************************************************/
/*--------------- programme client -----------------------*/

int main(int argc, char *argv[]) {

  char *serveur = SERVEUR_DEFAUT; /* serveur par defaut */
  char *service =
      SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc) {
  case 1: /* arguments par defaut */
    printf("serveur par defaut: %s\n", serveur);
    printf("service par defaut: %s\n", service);
    break;
  case 2: /* serveur renseigne  */
    serveur = argv[1];
    printf("service par defaut: %s\n", service);
    break;
  case 3: /* serveur, service renseignes */
    serveur = argv[1];
    service = argv[2];
    break;
  default:
    printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
    exit(1);
  }

  /* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
  /* service le numero de port sur le serveur correspondant au  */
  /* service desire par le client */

  client_appli(serveur, service);
}

/*****************************************************************************/
void client_appli(char *serveur, char *service) {

  /* procedure correspondant au traitement du client de votre application */

  /* Création d'une socket */
  int id;
  id = h_socket(AF_INET, SOCK_STREAM);

  /* Renseignement adresses distantes */
  struct sockaddr_in *p_adr_socket = NULL;
  adr_socket(service, serveur, SOCK_STREAM, &p_adr_socket);

  /* Demande de connexion au serveur */
  h_connect(id, p_adr_socket);

  /* Dialogue avec le serveur */
  /* Lecture du message demandant le nombre de couleurs */
  char tamp[42] = "";
  h_reads(id, tamp, 42);
  printf("%s", tamp);
  /* Envoyer le nombre de couleurs au serveur */
  int n = 0;
  scanf(" %d", &n);
  char nb[] = "";
  sprintf(nb, "%d", n);
  h_writes(id, nb, 2);
  /* Lecture du message demandant une proposition */
  char jouer[27] = "";
  h_reads(id, jouer, 27);
  printf("%s", jouer);
  /* Envoyer une proposition de couleurs */
  char *prop = malloc(sizeof(char) * n);
  scanf("%s", prop);
  h_writes(id, prop, n);
  /* Lecture de la réponse du serveur */
  char rep[2];
  char r[2] = "r";
  h_reads(id, rep, 2);
  int cpt = 1;
  /* Tant que le serveur répond "r" signifiant rejouer */
  while (!strcmp(rep, r)) {
    /* Lecture des anciens coups */
    printf("=====================\n");
    for (int i = 0; i < cpt; i++) {
      h_reads(id, prop, n);
      printf("%s\n", prop);
      h_reads(id, nb, 2);
      printf("Blancs : %s\n", nb);
      h_reads(id, nb, 2);
      printf("Rouges : %s\n", nb);
      printf("=====================\n");
    }
    /* Envoyer une proposition de couleurs */
    printf("Reessayer : ");
    scanf("%s", prop);
    h_writes(id, prop, n);
    /* Lecture de la réponse du serveur */
    h_reads(id, rep, 2);
    /* Compteur du nombre de coups */
    cpt++;
  }
  /* Affichage de l'issue de la partie avec le nombre de coups si victoire */
  char v[2] = "v";
  if (strcmp(v, rep)) {
    printf("Vous avez perdu\n");
  } else {
    printf("Vous avez gagné en %d coups\n", cpt);
  }

  /* Fermeture de la socket de communication */
  h_close(id);
}

/*****************************************************************************/
