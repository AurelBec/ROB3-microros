#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "fastsim_c.h"
#include "util_node.h"
#include "util_config.h"
#include "util_fifo.h"


Fifo* fifo_new(const char* name)
{
	Fifo* fifo = (Fifo*) malloc(sizeof(Fifo));
	if(fifo == NULL)
	{
		fprintf(stderr, "ERR : Impossible de creer le maillon fifo\n");
		exit(EXIT_FAILURE);
	}

	fifo->name = (char *) malloc( STR_SIZEOF(name) );
	if(fifo->name == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ecrire le nom du maillon fifo \n");
		exit(EXIT_FAILURE);
	}

	sscanf(name, "%s", fifo->name);
	fifo->nb_publish = 0;
	fifo->nb_listen = 0;
	fifo->suiv = NULL;

	return fifo;
}

int fifo_is_present(Fifo* fifo, const char* name)
{
	while( fifo != NULL)
	{
		if( strcmp(name, fifo->name) == 0)
			return 1;

		fifo = fifo->suiv;
	}

	return 0;
}

void fifo_add_port(Fifo* fifo, const char* name, const char port_type)
{
	while( fifo != NULL)
	{
		if( strcmp(fifo->name, name) == 0)
		{
			switch( port_type)
			{
				case 'p':
					fifo->nb_publish += 1;
					break;
				case 'l':
					fifo->nb_listen += 1;
					break;
				default : break;
			}
		}

		fifo = fifo->suiv;
	}
}


void fifo_creer(Fifo* lfifo)
{
	//system("rm -rf *fifo");

	Fifo* fifo = lfifo;
	while(fifo != NULL)
	{
		if(mkfifo(fifo->name, 0777) != 0 )
		{
			fprintf(stderr, "ERR : Impossible de créer le fifo\n");
			exit(EXIT_FAILURE);
		}

		fifo = fifo->suiv;
	}
}


void fifo_free(Fifo* fifo)
{
	if(fifo ==NULL)
		return;

	Fifo* fifo_temp = fifo->suiv;
	free(fifo->name);
	free(fifo);

	fifo_free(fifo_temp);
}
