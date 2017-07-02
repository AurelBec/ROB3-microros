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
	char map_name[TAILLE_MSG] = "\0";
	char buffer[TAILLE_MSG];

	int bumper_r = 0; // Right bumper (1 = active, 0 = inactive)
	int bumper_l = 0; // Left bumper (1 = active, 0 = inactive)
	int laser_r = 0;
	int laser_l = 0;
	int laser_c = 0;
	
	int i = 1;
	int robot_pos[] = {-1, -1};
	int port_command = -1, port_bumper_r = -1, port_bumper_l = -1, port_laser_r = -1, port_laser_l = -1, port_laser_c = -1;

	while( i < argc)
	{
		if( strcmp( argv[i], "--map") == 0)
		{
			if( i+1 >= argc)
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--map\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+1], '-') )
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--map\'\n");
				exit(EXIT_FAILURE);
			}


			strcpy(map_name, argv[i+1]);

			i++;
		}

		if( strcmp( argv[i], "--init-pos") == 0)
		{
			if( i+2 >= argc)
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--init-pos\'\n");
				exit(EXIT_FAILURE);
			}
			if( strchr(argv[i+2], '-') )
			{
				fprintf(stderr,"ERR : Pas assez d'arguments apres \'--init-pos\'\n");
				exit(EXIT_FAILURE);
			}

			robot_pos[0] = atoi(argv[i+1]);
			robot_pos[1] = atoi(argv[i+2]);

			i += 2;
		}
		
		if( strcmp(argv[i], "--listen") == 0)
		{
			if(strcmp(argv[i+1], "commande") == 0)
			{
				port_command = open(argv[i+2], O_RDONLY );
				if( port_command < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i+2]);
					exit(EXIT_FAILURE);
				}
			}

			i += 2;
		}

		if( strcmp(argv[i], "--publish") == 0)
		{
			if(strcmp(argv[i+1], "bumper_r") == 0)
			{
				port_bumper_r = open(argv[i+2], O_WRONLY);
				if( port_bumper_r < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i+2]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i+1], "bumper_l") == 0)
			{
				port_bumper_l = open(argv[i+2], O_WRONLY);
				if( port_bumper_l < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i+2]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i+1], "laser_r") == 0)
			{
				port_laser_r = open(argv[i+2], O_WRONLY);
				if( port_laser_r < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i+2]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i+1], "laser_c") == 0)
			{
				port_laser_c = open(argv[i+2], O_WRONLY);
				if( port_laser_c < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i+2]);
					exit(EXIT_FAILURE);
				}
			}

			if(strcmp(argv[i+1], "laser_l") == 0)
			{
				port_laser_l = open(argv[i+2], O_WRONLY);
				if( port_laser_l < 0 )
				{
					fprintf(stderr, "ERR : Impossible d'ouvrir le tube nommé \"%s\"\n", argv[i+2]);
					exit(EXIT_FAILURE);
				}
			}
			i += 2;
		}
		
		++i;
	}


	Robot* robot = new_robot(robot_pos[0], robot_pos[1]);
	Map* map = new_map(map_name);
	Display* display = new_display(map, robot);

	pthread_t thread_commande;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	Command_Robot command = {.port = port_command, .mutex = &mutex, .stop = 0};
	
	//Lancement lecture commande
	pthread_create( &thread_commande, NULL, update_vitesse, &command);

	do
	{
		bumper_l =  get_right_bumper(robot);
		bumper_r =  get_left_bumper(robot);
		laser_r = get_laser(robot, 0);
		laser_c = get_laser(robot, 2);
		laser_l = get_laser(robot, 1);
		
		//Envoi bumper
		if(port_bumper_r != -1)
		{
			sprintf(buffer, "%d", bumper_r);
			write(port_bumper_r, buffer, TAILLE_MSG);
		}

		if(port_bumper_l != -1)
		{
			sprintf(buffer, "%d", bumper_l);
			write(port_bumper_l, buffer, TAILLE_MSG);
		}

		if(port_laser_r != -1)
		{
			sprintf(buffer, "%d", laser_r);
			write(port_laser_r, buffer, TAILLE_MSG);
		}

		if(port_laser_c != -1)
		{
			sprintf(buffer, "%d", laser_c);
			write(port_laser_c, buffer, TAILLE_MSG);
		}

		if(port_laser_l != -1)
		{
			sprintf(buffer, "%d", laser_l);
			write(port_laser_l, buffer, TAILLE_MSG);
		}
		
		//printf("timestep %d : left bump = %d   right bump = %d  vr = %f vl = %f\n", stop_debug, bumper_l, bumper_r, command.vr, command.vl);

		move_robot(robot, map, command.vr, command.vl);
		update_display(display);		


		// Sleep to make the simulation slow enough for humans
		usleep(5000); // 500

	} while(command.stop == 0); //&& stop debug < VAL

	pthread_join( thread_commande, NULL);
	pthread_exit(NULL);


	delete_display(display);
	delete_map(map);
	delete_robot(robot);

	return 0;
}
