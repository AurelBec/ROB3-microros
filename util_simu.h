#ifndef SIMU_H
#define SIMU_H

#include "util_node.h"

typedef robot_t Robot;
typedef map_t Map;
typedef display_t Display;

typedef struct
{
  char chaine[TAILLE_MSG];
  int port;
  int stop;
  float vr;
  float vl;
  pthread_mutex_t *mutex;
} Command_Robot;

void* update_vitesse ( void* command);

#endif