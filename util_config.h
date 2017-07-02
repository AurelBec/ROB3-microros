#ifndef CONFIG_H
#define CONFIG_H

#include "util_fifo.h"


typedef struct _config Config ;
struct _config
{
	Node* node;
	Fifo* fifo;
	char path[TAILLE_MSG];
};

Config* config_new ();
Node* config_lecture ( const char* fname);
void config_verif_exe ( Config* config);
void config_verif_fifo (Config* config);
int config_nb_node (Config* config);
void config_add_fifo (Config* config, Port* port, const char port_type);
void config_free (Config* config);

#endif
