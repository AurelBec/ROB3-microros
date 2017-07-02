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
	char to_log[256];
	char nom_log[256];
	char fname[256];
	char buffer[256];

	int i = 1;
	while( i < argc)
	{
		if( strcmp( argv[i], "--listen") == 0)
		{

			if( i+2 >= argc)
			{
				fprintf(stderr, "ERR : Pas assez d'arguments apres \'--listen\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+2], '-') || strchr(argv[i+1], '-'))
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--listen\'\n");
				exit(EXIT_FAILURE);
			}

			strcpy(to_log, argv[i+1]);
			strcpy(nom_log, argv[i+2]);

			i += 2;
		}

		if( strcmp( argv[i], "--logfile") == 0)
		{

			if( i+1 >= argc)
			{
				fprintf(stderr, "ERR : Pas assez d'arguments apres \'--logfile\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+1], '-') )
			{
				fprintf(stderr, "ERR : Pas assez d'arguments apres \'--logfile\'\n");
				exit(EXIT_FAILURE);
			}

			strcpy(fname, argv[i+1]);
			i++;
		}

		++i;
	}

	//if(mkfifo(nom_log, 0644) != 0 )
	//{
	//	fprintf(stderr, "ERR : Impossible de créer le tube\n");
	//	exit(EXIT_FAILURE);
	//}
	int node_log = open(nom_log, O_RDONLY);
	if( node_log == -1 )
	{
		fprintf(stderr, "ERR (Log): Impossible d'ouvrir le tube nommé \"%s\"\n", nom_log);
		exit(EXIT_FAILURE);
	}


	FILE *f = fopen(fname, "a+");
	if( f == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ouvrir le fichier \"%s\"\n", fname);
		exit(EXIT_FAILURE);
	}

	while(read(node_log, buffer, 256))
	{
		//printf("%s\n", buffer);
		fprintf(f, "%s\n", buffer);
		fflush(f);
	}

	fclose(f);

	return 0;
}

