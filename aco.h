#ifndef ACO_H
#define ACO_H

#include "graph.h"
#include <math.h>
#include <vector>
#include <unordered_map>
#include "config.h"
#include <fstream>


// Representación de las Feromonas
struct Feromona
{
    Nodo *nodo_inicial = nullptr;  // Nodo inicial
    Nodo *nodo_terminal = nullptr; // Nodo terminal
    double cantidad = 0.0;         // Cantidad de feromonas
};

// Representación de una Hormiga
struct Hormiga
{
    Nodo *nodo_actual = nullptr;                            // Nodo actual
    std::vector<Arco> camino;                               // Camino recorrido
    std::unordered_map<Arco *, int> arcosVisitados;         // Aristas visitadas
    double longitud_camino = 0.0;                           // Longitud del camino, cuantas aristas se ha recorrido
    double costo_camino = 0.0;                              // Costo del camino, costo asociado a la recolección y recorrido
    int id = 0;                                             // Identificador de la hormiga
    std::unordered_map<Arco *, Feromona> feromonas_locales; // Feromonas locales de la hormiga
};

class ACO
{
protected:
    virtual void iterar();                          // Itera el algoritmo
    virtual void inicializar_feromonas() = 0;       // Itera el algoritmo
    Hormiga guardar_mejor_solucion_iteracion();     // Guarda la mejor solución
    Hormiga mejor_solucion;                         // Mejor solución
    void limpiar();                                 // Limpia la memoria y datos del algoritmo
    std::unordered_map<Arco *, Feromona> feromonas; // Feromonas
    std::vector<Hormiga> hormigas;                  // Hormigas
    void construirSolucion(Hormiga &hormiga);       // Construye la solución para una hormiga
    Graph *grafo = nullptr;                         // Grafo
    char filename[100];
    std::ofstream file;
    std::string nombre_archivo_salida;




public:
    std::string nombre_instancia;   // Nombre de la instancia
    float alfa;                        // Parámetro alfa
    int metodo;                        // Método (0: antsystem, 1: minmax)
    float beta;                       // Parámetro beta
    float rho;                               // Parámetro rho, asociado a la evaporacion de feromonas
    float tau;                               // Parámetro rho, asociado a la evaporacion de feromonas
    float rho_secundario;    // Parámetro rho, asociado a la evaporacion de feromonas
    int iteraciones;            // Cantidad de iteraciones, asociada a la funcion resolver y al criterio de parada
    int iteraciones_max;    // Cantidad de iteraciones maximas
    bool debug;                       // Flag que muestra o no informacion de debug como los caminos de las hormigas
    double umbral_inferior; // Umbral inferior para las feromonas
    int num_hormigas;          // Numero de hormigas
    int epocas;                        // Numero de epocas
    int epoca_actual;            // Numero de epocas
    

    ACO(Graph *instancia, ACOArgs parametros_base); // Constructor;
    //ACO(Graph *instancia, ParametrosACOBase parametros_base); // Constructor;
    void reset();

    virtual void resolver() = 0; // Resuelve el problema

    void mostrar_solucion(bool show_solucion);                         // Muestra la solución
    void set_mejor_feromonas();                                        // Setea las feromonas segun #hormigas/longitud_mejor_camino
    void abrir_file();
    void cerrar_file();
    std::string get_filename();
    void set_filename(std::string filename);
    Hormiga get_mejor_solucion();
    void exportar_solucion();
    void exportar_mapa_resultados();

private:

    Hormiga mejor_hormiga;                                // Mejor solución de la iteracion actual
    int mejor_costo = std::numeric_limits<int>::max();    // Mejor costo de la iteracion actual
    int mejor_longitud = std::numeric_limits<int>::max(); // Mejor costo de la iteracion actual

    int evaluaciones = 0; // Evaluaciones de la funcion objetivo
    Nodo *eligeSiguiente(Hormiga &hormiga);     // Elige el siguiente nodo
    void visitar(Hormiga &hormiga, Nodo *nodo); // Visita el nodo siguiente
    bool solucionCompleta(Hormiga &hormiga);    // Verifica si la solución es completa
    void set_parametros(ACOArgs parametros_base);
    
};

#endif
