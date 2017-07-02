#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "fastsim_c.h"
#include "util_node.h"


int main(int argc, char* argv[])
{
	int port_command_out;
	int stop = 0;
	char commande[TAILLE_MSG];

	if( strcmp(argv[1], "--publish") == 0 )
	{
		port_command_out = open(argv[3], O_WRONLY);
		if( port_command_out < 0 )
		{
			fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[3]);
			exit(EXIT_FAILURE);
		}
	}

	usleep(100000);

	do
	{
		printf("Commande > ");
		fgets( commande, TAILLE_MSG, stdin);

		if( commande[0] == 'q')
			stop = 1;
		
		write(port_command_out, commande, TAILLE_MSG);
		
		usleep(500);
	} while(stop == 0);

	close(port_command_out);

	return 0;
}
