#ifndef NODE_H
#define NODE_H

////////////////////////////
// CONTIENT LES FONCTIONS DE CREATION ET REMPLISSAGE DES NODES ET PORTS
////////////////////////////


#define TAILLE_MSG 256
#define STR_SIZEOF(str) (int) ((strlen(str)+1) * sizeof(char))

typedef struct _port Port;
struct _port
{
	char *nom_du_port;
	char *nom_du_fifo;

	Port *suiv;
};

typedef struct _node Node;
struct _node
{
	char *name;
	char *bin;
	char *name_file;
	char *args;

	Port *listen;
	Port *publish;

	Node *suiv;
};

Node* node_new ();
void node_add_name (Node* node, const char* buffer);
void node_add_bin (Node* node, const char* buffer);
void node_add_port (Node *node, const char* buffer, const char port_type);
void node_add_args (Node* node, const char* buffer);
void node_print (Node* node);
void node_exec (Node* node, const char* path);
void node_free (Node* node);

Port* port_new ();
void port_print (Port* port, const char* port_type);
void port_free (Port* port);

#endif
