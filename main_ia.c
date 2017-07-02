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

int main(int argc, char* argv[])
{
	char buffer[TAILLE_MSG];

	int bumper_r = 0; // Right bumper (1 = active, 0 = inactive)
	int bumper_l = 0; // Left bumper (1 = active, 0 = inactive)
	int laser_l = 0;
	int laser_r = 0;
	int laser_c = 0;

	float v_coeff = 1;
	int collision_r = 0;
	int collision_l = 0;

	int i = 0;
	/*while( i < argc)
		printf("%s ", argv[i++]);*/

	int port_command = 1, port_bumper_r = -1, port_bumper_l = -1, port_laser_r = -1, port_laser_l = -1, port_laser_c = -1;

	i = argc;
	while( i > 0)
	{
		if( strcmp(argv[i-2], "--listen") == 0)
		{
			if(strcmp(argv[i-1], "bumper_r") == 0)
			{
				port_bumper_r = open(argv[i], O_RDONLY);
				if( port_bumper_r < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i-1], "bumper_l") == 0)
			{
				port_bumper_l = open(argv[i], O_RDONLY);
				if( port_bumper_l < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i-1], "laser_r") == 0)
			{
				port_laser_r = open(argv[i], O_RDONLY);
				if( port_laser_r < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i-1], "laser_c") == 0)
			{
				port_laser_c = open(argv[i], O_RDONLY);
				if( port_laser_c < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i-1], "laser_l") == 0)
			{
				port_laser_l = open(argv[i], O_RDONLY);
				if( port_laser_l < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}

			i -= 2;
		}

		if( strcmp(argv[i-2], "--publish") == 0)
		{
			if(strcmp(argv[i-1], "commande_out") == 0)
			{
				port_command = open(argv[i], O_WRONLY );

				if( port_command < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i]);
					exit(EXIT_FAILURE);
				}
			}

			i -= 2;
		}

		--i;
	}

	usleep(100000);
	printf("Commande > AUTO\n");

	while(1)
	{
		read(port_bumper_r, buffer, TAILLE_MSG);
		bumper_r = atoi(buffer);

		read(port_bumper_l, buffer, TAILLE_MSG);
		bumper_l = atoi(buffer);


		read(port_laser_r, buffer, TAILLE_MSG);
		laser_r = atoi(buffer);

		read(port_laser_c, buffer, TAILLE_MSG);
		laser_c = atoi(buffer);

		read(port_laser_l, buffer, TAILLE_MSG);
		laser_l = atoi(buffer);


		printf("%d %d %d | %d %d\n", laser_l, laser_c, laser_r, bumper_l, bumper_r);


		//if( laser_c < 20 && laser_c != -1)
			//sprintf(buffer, "set_v %f %f ", -1.0, 2.0);		
		/*else*/ 

		/*if( bumper_l == 1)
			sprintf(buffer, "set_v %f %f ", -2.0, 1.0);
		else if( bumper_r == 1)
			sprintf(buffer, "set_v %f %f ", 1.0, -2.0);
		else if( (laser_r < 50 && laser_r != -1) || (laser_l < 50 && laser_l != -1) || (laser_c < 25 && laser_c != -1) )
			sprintf(buffer, "set_v %f %f ", -1.0, 1.5);
		else
			sprintf(buffer, "set_v %lf %lf ", 1.5, 1.55);*/

	


		if( bumper_r == 1)
		{
			collision_r = 70;
		}
		else if(bumper_l == 1)
		{
			collision_l = 70;
		}


		if( collision_r > 30 || collision_l > 30)
		{
			sprintf(buffer, "set_v %lf %lf ", -0.5, -0.5);
			collision_r --;
			collision_l --;
		}
		else if( collision_r > 0)
		{
			sprintf(buffer, "set_v %lf %lf ", 0.5, -0.6);
			collision_r --;
		}
		else if( collision_l > 0)
		{
			sprintf(buffer, "set_v %lf %lf ", -0.6, 0.5);
			collision_l --;
		}
		else
		{
			sprintf(buffer, "set_v %lf %lf ", 0.8, 0.8);
		}


		write(port_command, buffer, TAILLE_MSG);

		usleep(100);
	}

	return 0;
}
