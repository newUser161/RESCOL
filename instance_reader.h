#ifndef INSTANCE_READER_H
#define INSTANCE_READER_H

#include "graph.h" 
#include <string>
using namespace std;
Graph leerInstancia(const std::string &filename, bool leer_restricciones, bool leer_coordenadas, bool irace);

#endif
