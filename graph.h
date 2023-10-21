#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

struct Nodo;
struct Subnodo;
struct Arco;

struct Metadatos
{
    std::unordered_map<std::string, std::string> encabezado;
    std::vector<Nodo> nodos_iniciales;
    std::vector<Nodo> nodos_termino;
};

struct Subnodo
{
    int id = 0;                 // Identificador del nodo
    int id_secundario = 0;                 // Identificador del nodo
    int tipo = 0;                 // tipo de nodo, 1:entrante, 2:saliente
    std::vector<Arco> saliente; // Aristas que salen del nodo
    std::vector<Arco> entrante; // Aristas que entran al nodo
    Subnodo *nodo_reciproco = nullptr; // subnodo reciproco

    
};

// Estructura para representar un nodo
struct Nodo
{
    int id = 0;                 // Identificador del nodo
    double x = 0.0, y = 0.0;    // Coordenadas del nodo
    int grado = 0;              // Grado del nodo    
    std::vector<Arco> saliente; // Aristas que salen del nodo
    std::vector<Arco> entrante; // Aristas que entran al nodo
    std::vector<Subnodo> subnodos; // Nodos fantasma
    
};



// Estructura para representar una arista
struct Arco
{
    int id = 0;                // Identificador de la arista
    double costo_recorrido = 0;   // Costo de recorrer la arista
    double costo_recoleccion = 0; // Costo de recolectar en la arista
    int veces_recorrida = 0;   // Veces que se recorre la arista
    bool obligatoria = 0;      // Indica si la arista es obligatoria
    bool bidireccional = 0;    // Indica si la arista es bidireccional
    int id_bidireccional = 0;    // Indica si la arista es bidireccional
    Arco * arco_reciproco = nullptr;    // Indica si la arista es bidireccional
    Nodo *origen = nullptr;    // Nodo de origen
    Nodo *destino = nullptr;   // Nodo de destino
    Subnodo *origen_interno = nullptr;    // Nodo de origen
    Subnodo *destino_interno = nullptr;   // Nodo de destino
};

// Estructura para representar un grafo
struct Graph
{
    std::map<int, Nodo> nodos;                                                       // Mapa de nodos del grafo
    std::unordered_map<int, Arco *> arcos;                                           // Mapa de aristas del grafo
    std::vector<Arco *> vector_arcos;                                                // Vector de aristas del grafo
    std::unordered_map<int, std::unordered_map<Arco *, double>> informacion_heuristica; // Información heurística
    Metadatos metadatos;                                                             // Metadatos del grafo
};

#endif
