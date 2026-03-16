#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

using namespace std;

struct Nodo;
struct Subnodo;
struct Arco;

struct Metadatos
{
    std::unordered_map<std::string, std::string> encabezado;
    std::vector<Nodo*> nodos_iniciales;
    std::vector<Nodo*> nodos_termino;
};

struct Subnodo
{
    int id_secundario = 0;             // Identificador del nodo
    int tipo = 0;                      // tipo de nodo, 1:entrante, 2:saliente
    std::vector<Arco*> saliente;        // Aristas que salen del nodo
    std::vector<Arco*> entrante;        // Aristas que entran al nodo
    Subnodo *nodo_reciproco = nullptr; // subnodo reciproco
};

// Estructura para representar un nodo
struct Nodo
{
    int index = 0;                 // Indice contiguo del nodo (0 a N-1)
    int id = 0;                    // Identificador original del nodo
    std::vector<Arco*> saliente;    // Aristas que salen del nodo
    std::vector<Arco*> entrante;    // Aristas que entran al nodo
    std::vector<std::unique_ptr<Subnodo>> subnodos; // Nodos fantasma
};

// Estructura para representar una arista
struct Arco
{
    int id = 0;                         // Identificador contiguo de la arista
    double costo_recorrido = 0;         // Costo de recorrer la arista
    double costo_recoleccion = 0;       // Costo de recolectar en la arista
    int veces_recorrida = 0;            // Veces que se recorre la arista
    bool bidireccional = 0;             // Indica si la arista es bidireccional
    bool obligatoria = true;            // Indica si la arista es obligatoria
    Arco *arco_reciproco = nullptr;     // Puntero al arco reciproco (via contraria de la misma calle)
    Nodo *origen = nullptr;             // Nodo de origen
    Nodo *destino = nullptr;            // Nodo de destino
};

// Estructura para representar un grafo
class Graph
{
public:
    std::vector<std::unique_ptr<Nodo>> nodos;                   // Nodos guardados de forma contigua
    std::unordered_map<int, Nodo*> mapa_nodos;                  // Mapeo Rápido desde ID original a puntero
    std::vector<std::unique_ptr<Arco>> arcos;                   // Aristas contiguas (indexadas por id)
    std::vector<std::vector<double>> informacion_heuristica;    // Matriz de Nodos x Arcos
    Metadatos metadatos;                                        // Metadatos del grafo

    // Helper functions
    void init_heuristica(size_t num_nodos, size_t num_arcos) {
        informacion_heuristica.assign(num_nodos, std::vector<double>(num_arcos, 0.0));
    }

    Nodo* get_or_create_nodo(int id_original) {
        auto it = mapa_nodos.find(id_original);
        if (it != mapa_nodos.end()) {
            return it->second;
        }
        auto nuevo_nodo = std::make_unique<Nodo>();
        nuevo_nodo->id = id_original;
        nuevo_nodo->index = nodos.size();
        Nodo* ptr = nuevo_nodo.get();
        mapa_nodos[id_original] = ptr;
        nodos.push_back(std::move(nuevo_nodo));
        return ptr;
    }
};

#endif
