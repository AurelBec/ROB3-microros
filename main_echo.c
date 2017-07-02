#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fastsim_c.h"


int main(int argc, char* argv[])
{
	char out_log[256];
	char nom_log[256];
	char msg[256];

	int interval = 1;

	int i = 1;
	while( i < argc)
	{
		if( strcmp( argv[i], "--publish") == 0)
		{
			if( i+2 >= argc)
			{
				fprintf(stderr,"Erreur1 pas assez d'arguments apres \'--publish\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+2], '-') || strchr(argv[i+1], '-'))
			{
				fprintf(stderr,"Erreur2 pas assez d'arguments apres \'--publish\'\n");
				exit(EXIT_FAILURE);
			}

			strcpy(out_log, argv[i+1]);
			strcpy(nom_log, argv[i+2]);

			i += 2;
		}

		if( strcmp( argv[i], "--msg") == 0)
		{
			if( i+1 >= argc)
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--msg\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+1], '-') )
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--msg\'\n");
				exit(EXIT_FAILURE);
			}

			strcpy(msg, argv[i+1]);

			i++;
		}

		if( strcmp( argv[i], "--interval") == 0)
		{
			if( i+1 >= argc)
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--interval\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+1], '-') )
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--interval\'\n");
				exit(EXIT_FAILURE);
			}

			interval = atoi(argv[i+1]);

			i++;
		}
		
		++i;
	}

	int node_log = open(nom_log, O_WRONLY);
	if( node_log == -1 )
	{
		fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé\n");
		exit(EXIT_FAILURE);
	}

	for( i = 0; i < 2; ++i)
	{
		write(node_log, msg, strlen(msg) + 1);
		printf("%s\n", msg);
		sleep(interval);
	}

	close(node_log);

	return 0;
}
