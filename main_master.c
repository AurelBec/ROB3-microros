#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "fastsim_c.h"
#include "util_node.h"
#include "util_fifo.h"
#include "util_config.h"


int main(int argc, char* argv[])
{
	system("rm -rf *.fifo");

	if( argc != 2)
	{
		fprintf(stderr, "ERR : Nombre d'arguments incorrect !\n  > %s \"fichier_de_config.txt\"\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//Creation de la config
	Config *config = config_new();
	config->node = config_lecture(argv[1]);
	getcwd(config->path, TAILLE_MSG);

	printf("\n-- CONFIG --\n");
	node_print(config->node);

	//Verification de la config
	printf("\n-- VERIFS --\n");
	config_verif_exe(config);
	config_verif_fifo(config);

	fifo_creer(config->fifo);

	//Execution des nodes
	printf("\n-- DEBUT SIMULATION --\n");
	Node* node = config->node;
	int pid, nb_nodes = 0;

	while (node != NULL)
	{
		pid = fork();
		nb_nodes ++;

		if( pid < 0)
		{
			fprintf(stderr, "ERR : Impossible de creer le processus\n");
			exit(EXIT_FAILURE);
		}
		else if( pid == 0)
		{
			node_exec(node, config->path);
			exit(EXIT_SUCCESS);
		}
		node = node->suiv;
	}

	//Fin du programme
	int i;
	for( i = 0; i < nb_nodes; i++)
		wait(NULL);

	config_free(config);
	system("rm -rf *.fifo");

	return 0;
}

