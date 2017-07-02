#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "fastsim_c.h"
#include "util_node.h"
#include "util_config.h"
#include "util_fifo.h"


Config* config_new()
{
	Config *config = (Config*) malloc(sizeof(Config));

	config->node = NULL;
	config->fifo = NULL;

	return config;
}


Node* config_lecture(const char* fname)
{
	FILE *f = fopen(fname, "r");
	if( f == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ouvrir le fichier \"%s\"\n", fname);
		exit(EXIT_FAILURE);
	}

	char buffer[TAILLE_MSG];

	Node* node_root = NULL;
	Node* node;

	while (fgets(buffer, TAILLE_MSG, f) != NULL)
	{
		int size_buffer = strlen(buffer);
		if( buffer[size_buffer - 1] == '\n')
			buffer[size_buffer - 1] = '\0';

		//Avant les ':'
		char* params;
		const char* token = strtok_r(buffer, ":", &params);

		//Apres les ':'
		char command[TAILLE_MSG];
		sscanf(token, "%s", command);

		if( strcmp(command, "node") == 0)
		{
			node = node_new();
			node->suiv = node_root;
			node_root = node;

			node_add_name(node, params);
		}
		else if( strcmp(command, "bin") == 0)
		{
			node_add_bin(node, params);
		}
		else if( strcmp(command, "publish") == 0)
		{
			node_add_port(node, params, 'p');
		}
		else if( strcmp(command, "listen") == 0)
		{
			node_add_port(node, params, 'l');
		}
		else if( strcmp(command, "args") == 0)
		{
			node_add_args(node, params);
		}
		else
		{
			fprintf(stderr, "ERR : Commande inconnue \"%s\"\n", command);
			exit(EXIT_FAILURE);
		}
	}
	fclose(f);

	return node_root;
}


void config_verif_exe(Config* config)
{
	Node *node = config->node;
	struct stat info_file;

	while(node != NULL)
	{
		if(stat(node->name_file, &info_file) == -1 )
		{
			fprintf(stderr, "ERR : Impossible de trouver l\'executable \"%s\"\n", node->name_file);
			exit(EXIT_FAILURE);
		}
		node = node->suiv;
	}

	printf("OK : Tout les executables existent\n");
}


void config_verif_fifo(Config* config)
{
	Node *node = config->node;
	if(node == NULL)
	{
		fprintf(stderr, "ERR : Aucun node en parametre\n" );
		exit(EXIT_FAILURE);
	}

	//Creation liste fifo
	while(node != NULL)
	{
		config_add_fifo(config, node->publish, 'p');
		config_add_fifo(config, node->listen, 'l');

		node = node->suiv;
	}

	//Verification liste fifo
	Fifo* fifo = config->fifo;
	while(fifo != NULL)
	{
		if(fifo->nb_publish != 1 || fifo->nb_listen != 1)
		{
			fprintf(stderr, "ERR : Nombre d\'I/O du fifo \'%s\' incorrect\n", fifo->name);
			exit(EXIT_FAILURE);
		}

		fifo = fifo->suiv;
	}

	printf("OK : Tout les fifo sont valides\n");
}


void config_add_fifo(Config* config, Port* port, const char port_type)
{
	Fifo *fifo = NULL;

	while(port != NULL)
	{
		if( fifo_is_present(config->fifo, port->nom_du_fifo) == 0)
		{
			fifo = fifo_new(port->nom_du_fifo);
			fifo->suiv = config->fifo;
			config->fifo = fifo;
		}

		fifo_add_port(config->fifo, port->nom_du_fifo, port_type);
		port = port->suiv;
	}
}

int config_nb_node(Config* config)
{
	int nb_nodes = 0;
	Node* node = config->node;

	while(node != NULL)
	{
		nb_nodes ++;
		node = node->suiv;
	}

	return nb_nodes;
}


void config_free(Config * config)
{
	node_free(config->node);
	fifo_free(config->fifo);
	free(config);
}
