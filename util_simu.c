#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include "fastsim_c.h"
#include "util_simu.h"
#include "util_node.h"
#include "util_fifo.h"

void* update_vitesse( void* command_temp)
{
	char buffer_cmd[TAILLE_MSG];
	char cmd[TAILLE_MSG];
	float vit_temp[2] = {0, 0};

	Command_Robot* command = (Command_Robot*) command_temp;

	do
	{
		pthread_mutex_lock( command->mutex);

		memset(buffer_cmd, 0, sizeof(buffer_cmd));
		read( command->port, buffer_cmd, TAILLE_MSG);

		sscanf(buffer_cmd, "%s %f %f", cmd, &vit_temp[0], &vit_temp[1]);

		//Lecture commande
		if( strcmp(cmd, "forward") == 0)
		{
			command->vr = vit_temp[0];
			command->vl = vit_temp[0];
		}
		else if( strcmp(cmd, "backward") == 0)
		{
			command->vr = - vit_temp[0];
			command->vl = - vit_temp[0];
		}
		else if( strcmp(cmd, "cw") == 0)
		{
			command->vr = - vit_temp[0];
			command->vl = vit_temp[0];
		}
		else if( strcmp(cmd, "ccw") == 0)
		{
			command->vr = vit_temp[0];
			command->vl = - vit_temp[0];
		}
		else if( strcmp(cmd, "stop") == 0)
		{
			command->vr = 0;
			command->vl = 0;
		}
		else if( strcmp(cmd, "set_v") == 0)
		{
			command->vr = vit_temp[0];
			command->vl = vit_temp[1];
		}
		else if (strcmp(cmd, "quit") == 0)
		{
			fprintf(stderr, "WAR : Fin de la simualtion\n");
			command->stop = 1;
			break;
		}
		else
		{
			fprintf(stderr, "WAR : Commande inconnue\n");
			sleep(1);
		}

		pthread_mutex_unlock( command->mutex);

		usleep(100);

	} while(command->stop == 0);//((Command_Robot*)command)->chaine[0] != 'q');

	pthread_exit(NULL);
}