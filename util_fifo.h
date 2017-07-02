#ifndef FIFO_H
#define FIFO_H


typedef struct _fifo Fifo;
struct _fifo
{
	char *name;
	int nb_publish;
	int nb_listen;

	Fifo *suiv;
};

Fifo* fifo_new (const char* name);
int fifo_is_present (Fifo* fifo, const char* name);
void fifo_add_port (Fifo* fifo, const char* name, const char occ_type);
void fifo_free (Fifo* fifo);
void fifo_creer (Fifo* fifo);

#endif
