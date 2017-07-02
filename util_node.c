#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fastsim_c.h"
#include "util_node.h"

Node* node_new()
{
	Node* node = (Node*) malloc( sizeof(Node));
	if(node == NULL)
	{
		fprintf(stderr, "ERR : Impossible de creer le node\n");
		exit(EXIT_FAILURE);
	}

	node->name      = NULL;
	node->bin       = NULL;
	node->name_file = NULL;
	node->args      = NULL;
	node->listen    = NULL;
	node->publish   = NULL;
	node->suiv      = NULL;

	return node;
}

Port* port_new()
{
	Port* port = (Port*) malloc(sizeof(Port));
	if(port == NULL)
	{
		fprintf(stderr, "ERR : Impossible de creer le port\n");
		exit(EXIT_FAILURE);
	}

	port->nom_du_port = NULL;
	port->nom_du_fifo = NULL;
	port->suiv        = NULL;

	return port;
}


void node_add_name(Node* node, const char* buffer)
{
	if(node == NULL)
	{
		fprintf(stderr, "ERR : Aucun node cree\n" );
		exit(EXIT_FAILURE);
	}

	node->name = (char *) malloc( STR_SIZEOF(buffer) );
	if(node->name == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ecrire le nom du node \n");
		exit(EXIT_FAILURE);
	}

	sscanf(buffer, "%s", node->name);
}


void node_add_bin(Node *node, const char* buffer)
{
	if(node == NULL)
	{
		fprintf(stderr, "ERR : Aucun node cree\n" );
		exit(EXIT_FAILURE);
	}

	node->bin = (char *) malloc( STR_SIZEOF(buffer));
	node->name_file = (char *) malloc( STR_SIZEOF(buffer));

	if( node->bin == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ecrire la commande\n");
		exit(EXIT_FAILURE);
	}

	if( node->name_file == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ecrire le nom de l\'exe\n");
		exit(EXIT_FAILURE);
	}

	sscanf(buffer, "%s", node->bin);
	buffer = node->bin + 2;
	sscanf(buffer, "%s", node->name_file);
}


void node_add_args(Node* node, const char* buffer)
{
	if(node == NULL)
	{
		fprintf(stderr, "ERR : Aucun node cree\n");
		exit(EXIT_FAILURE);
	}

	node->args = (char *) malloc( STR_SIZEOF(buffer));
	if( node->args == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ecrire les arguments\n");
		exit(EXIT_FAILURE);
	}

	strcpy(node->args, buffer);
}


void node_add_port(Node *node, const char* buffer, const char port_type)
{
	if(node == NULL)
	{
		fprintf(stderr, "ERR : Aucun node cree\n");
		exit(EXIT_FAILURE);
	}

	Port* port = port_new();

	port->nom_du_port = (char *) malloc( STR_SIZEOF(buffer));
	if(port->nom_du_port == NULL)
	{
		fprintf(stderr, "ERR : Impossible d'ecrire le nom du port\n");
		exit(EXIT_FAILURE);
	}

	port->nom_du_fifo = (char *) malloc( STR_SIZEOF(buffer));
	if(port->nom_du_fifo == NULL)
	{
		fprintf(stderr, "ERR: Impossible d'ecrire le nom du fifo\n");
		exit(EXIT_FAILURE);
	}

	switch( port_type)
	{
		case 'p' :
			port->suiv = node->publish;
			node->publish = port;

			if(sscanf(buffer, "%s -> %s", port->nom_du_port, port->nom_du_fifo) != 2)
			{
				fprintf(stderr, "ERR : Impossible de lire le port\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'l':
			port->suiv = node->listen;
			node->listen = port;

			if(sscanf(buffer, "%s <- %s", port->nom_du_port, port->nom_du_fifo) != 2)
			{
				fprintf(stderr, "ERR : Impossible de lire le port\n");
				exit(EXIT_FAILURE);
			}
			break;
		default : break;
	}

}


void node_print(Node* node)
{
	if(node == NULL)
		return;

	printf("%s [%s %s]\n", node->name, node->bin, (node->args != NULL) ? node->args : "");

	printf("  publish:\n");
	port_print(node->publish, "to");

	printf("  listen:\n");
	port_print(node->listen, "from");

	printf("\n");
	node_print(node->suiv);
}


void port_print(Port* port, const char* port_type)
{
	if(port == NULL)
		return;

	printf("    %s %s %s\n", port->nom_du_port, port_type, port->nom_du_fifo);
	port_print(port->suiv, port_type);
}


void node_exec(Node* node, const char* path)
{
	//On utilise un tableau de pointeur vers les chaines copiées dans un tableau statique pour eviter les erreurs memoires avec strdup
	char arg[TAILLE_MSG][TAILLE_MSG];
	char *ptr_arg[TAILLE_MSG];

	char path_exe[TAILLE_MSG];
	char *buffer;
	Port* port;
	int i = 0;

	//./exe
	strcpy(arg[i++], node->bin);

	//--args
	buffer = strtok(node->args, " ");
	while( buffer != NULL )
	{
		strcpy(arg[i++], buffer);
		buffer = strtok(NULL, " ");
	}

	//--publish
	port = node->publish;
	while( port != NULL)
	{
		strcpy(arg[i++], "--publish");
		strcpy(arg[i++], port->nom_du_port);
		strcpy(arg[i++], port->nom_du_fifo);

		port = port->suiv;
	}

	//--listen
	port = node->listen;
	while( port != NULL)
	{
		strcpy(arg[i++], "--listen");
		strcpy(arg[i++], port->nom_du_port);
		strcpy(arg[i++], port->nom_du_fifo);

		port = port->suiv;
	}

	strcpy(arg[i++], "&");

	//On recupere les adresses des arguments
	int nb_arg = i;
	for(i = 0; i < nb_arg; ++i)
		ptr_arg[i] = arg[i];
	ptr_arg[nb_arg] = NULL;

	sprintf(path_exe, "%s/%s", path, node->name_file);
	execv(path_exe, ptr_arg);
}

void node_free(Node* node)
{
	if(node == NULL)
		return;

	Node* node_temp_suiv = node->suiv;

	free(node->name);
	free(node->bin);
	free(node->name_file);
	free(node->args);

	port_free(node->publish);
	port_free(node->listen);

	free(node);

	node_free(node_temp_suiv);
}


void port_free(Port* port)
{
	if(port == NULL)
		return;

	Port* port_temp_suiv = port->suiv;
	free(port->nom_du_port);
	free(port->nom_du_fifo);

	free(port);

	port_free(port_temp_suiv);
}
