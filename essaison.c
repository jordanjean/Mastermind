
#include<stdio.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/uio.h>

#define TAILLE_BUF 80500
main()
{
char buf[TAILLE_BUF];
int taille_enr,taille_joue;

int fsp;   /* descripteur de fichier */

fsp=open("/dev/audio", O_RDONLY);
printf("fsp: %d", (int)fsp);

printf("\n j'enregistre\n " );

taille_enr=read(fsp, buf,TAILLE_BUF);

printf("\n j'ai enregistre: %d bytes\n " ,taille_enr);

close(fsp);

fsp=open("/dev/audio",O_WRONLY);
printf("\n je joue: \n");

taille_joue=write(fsp, buf,taille_enr);

printf("\n j'ai joue: %d bytes\n " ,taille_joue);
close(fsp);


}
