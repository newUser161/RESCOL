#ifndef ACO_H
#define ACO_H

#include "graph.h"
#include <vector>
#include <unordered_map>

// Representación de una Hormiga
struct Hormiga
{
    Nodo *nodo_actual = nullptr;                    // Nodo actual
    std::vector<Arco> camino;                       // Camino recorrido
    std::unordered_map<Arco *, int> arcosVisitados; // Aristas visitadas
    double longitud_camino = 0.0;                   // Longitud del camino, cuantas aristas se ha recorrido
    double costo_camino = 0.0;                      // Costo del camino, costo asociado a la recolección y recorrido
    int id = 0;                                     // Identificador de la hormiga
};

// Representación de las Feromonas
struct Feromona
{
    Nodo *nodo_inicial = nullptr;  // Nodo inicial
    Nodo *nodo_terminal = nullptr; // Nodo terminal
    double cantidad = 0.0;         // Cantidad de feromonas
};

class ACO
{
public:
    // Constructor, destructores y otros métodos.
    ACO(Graph *graph, int num_hormigas, bool debug); // Constructor
    void resolver(int iteraciones_max);  // Resuelve el problema
    void mostrar_solucion(bool show_solucion);             // Muestra la solución
    void limpiar();                      // Limpia la memoria y datos del algoritmo
    

private:
    Graph *grafo = nullptr; // Grafo

    float alfa = 1.0;    // Parámetro alfa
    float beta = 2.0;    // Parámetro beta
    float rho = 0.5;     // Parámetro rho, asociado a la evaporacion de feromonas
    float tau = 1.0;     // Parámetro tau, asociado a las feromonas iniciales
    int iteraciones = 0; // Cantidad de iteraciones, asociada a la funcion resolver y al criterio de parada
    bool debug = false;  // Flag que muestra o no informacion de debug como los caminos de las hormigas
    std::unordered_map<Arco *, Feromona> feromonas; // Feromonas
    std::vector<Hormiga> hormigas;                  // Hormigas
    Hormiga mejor_solucion;                         // Mejor solución
    

    Nodo *eligeSiguiente(Hormiga &hormiga);     // Elige el siguiente nodo
    void construirSolucion(Hormiga &hormiga);   // Construye la solución para una hormiga
    void visitar(Hormiga &hormiga, Nodo *nodo); // Visita el nodo siguiente
    bool solucionCompleta(Hormiga &hormiga);    // Verifica si la solución es completa
    void iterar();                              // Itera el algoritmo
    Hormiga guardar_mejor_solucion();              // Guarda la mejor solución
};

#endif
