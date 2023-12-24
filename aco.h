#ifndef ACO_H
#define ACO_H

#include "graph.h"
#include <math.h>
#include <vector>
#include <unordered_map>
#include "config.h"
#include <fstream>
#include "oscilador.h"

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
    std::vector<Arco> camino_tour;                          // Camino recorrido
    std::vector<Arco> camino_salida;                        // Camino recorrido
    std::vector<Arco> camino_final;                         // Camino recorrido
    std::unordered_map<Arco *, int> arcos_visitados_tour;   // Aristas visitadas
    std::unordered_map<Arco *, int> arcos_visitados_salida; // Aristas visitadas
    std::unordered_map<Arco *, int> arcos_visitados_final;  // Aristas visitadas
    double longitud_camino_tour = 0.0;
    double longitud_camino_salida = 0.0;
    double longitud_camino_final = 0.0;
    int saltosTour = 0;                                     // Cantidad de saltos que toma terminar un tour completamente
    int saltosSalida = 0;                                   // Cantidad de saltos desde el termino del tour hasta la salida                       // Longitud del camino, cuantas aristas se ha recorrido
    double costo_camino = 0.0;                              // Costo del camino, costo asociado a la recolección y recorrido
    bool solucion_valida = true;                              // Costo del camino, costo asociado a la recolección y recorrido
    int id = 0;                                             // Identificador de la hormiga
    std::unordered_map<Arco *, Feromona> feromonas_locales; // Feromonas locales de la hormiga
    std::unordered_map<Arco*, int> arcos_no_visitados;        // Aristas no visitadas
};

class ACO
{
protected:
    Oscilador oscilador;

    virtual void iterar();                                 // Itera el algoritmo
    virtual void inicializar_feromonas() = 0;              // Itera el algoritmo
    Hormiga guardar_mejor_solucion_iteracion();            // Guarda la mejor solución
    bool solucionCompleta(Hormiga &hormiga);               // Verifica si la solución es completa
    bool enNodoTerminal(Hormiga &hormiga);                 // Verifica si la hormiga esta en un nodo final
    Hormiga mejor_solucion;                                // Mejor solución
    void limpiar();                                        // Limpia la memoria y datos del algoritmo
    void limpiar_rastro();                                 // Limpia la memoria y datos del algoritmo
    std::unordered_map<Arco *, Feromona> feromonas;        // Feromonas
    std::unordered_map<Arco *, Feromona> feromonas_salida; // Feromonas
    std::vector<Hormiga> hormigas;     
    std::string nombre_metodo;
    std::string nombre_instancia_salida;
    void construirSolucion(Hormiga &hormiga);              // Construye la solución para una hormiga    
    int evaluaciones = 0;                                  // Evaluaciones de la funcion objetivo
    virtual Nodo *eligeSiguiente(Hormiga &hormiga);        // Elige el siguiente nodo
    virtual void buscarSalida(Hormiga &hormiga);           // busca la salida
    virtual void visitar(Hormiga &hormiga, Nodo *nodo);    // Visita el nodo siguiente
    int mejor_costo = std::numeric_limits<int>::max();     // Mejor costo de la iteracion actual
    Graph *grafo = nullptr;                                // Grafo
    char filename[100];
    std::ofstream file;
    std::string nombre_archivo_salida;
    std::filesystem::path directorio_salida;
    void set_parametros(ACOArgs parametros_base);
    float timeout = 600000 ;
    
    bool timeout_flag = false;
    bool timeout_flag_global = false;

    int usar_oscilador = 0;
    int valor_limitador = 999999;
    bool usar_bd = false;
    bool usar_iteraciones = false;
    bool usar_evaluaciones = false;

public:
    std::string nombre_instancia; // Nombre de la instancia
    std::string ruta_archivo_salida_csv;
    std::string ruta_archivo_config_salida_csv;
    float alfa;                   // Parámetro alfa
    int metodo;                   // Método (0: antsystem, 1: minmax, 3: ACS)
    float beta;                   // Parámetro beta
    float beta_salida;                   // Parámetro beta
    float rho;                    // Parámetro rho, asociado a la evaporacion de feromonas
    float tau;                    // Parámetro rho, asociado a la evaporacion de feromonas
    float rho_secundario;         // Parámetro rho, asociado a la evaporacion de feromonas
    float rho_salida;             // Parámetro rho de salida, asociado a la evaporacion de feromonas de la matriz de salida, deberia se un valor muy pequeño
    int iteraciones = 0;          // Cantidad de iteraciones, asociada a la funcion resolver y al criterio de parada
    int iteraciones_max;          // Cantidad de iteraciones maximas ||  = numevaluaciones / hormigas;    
    int evaluaciones_maximas;    // Cantidad de evaluaciones maximas
    bool debug = false;        // Flag que muestra o no informacion de debug como los caminos de las hormigas
    double umbral_inferior;    // Umbral inferior para las feromonas
    int num_hormigas;          // Numero de hormigas
    int epocas;                // Numero de epocas
    int epoca_actual = 0;      // Numero de epocas
    bool usarMatrizSecundaria; // Flag que controla el uso general de la matriz de salida, se pasa por parametros
    float acumulador_tiempo = 0;

    

    ACO(Graph *instancia, ACOArgs parametros_base); // Constructor;
    // ACO(Graph *instancia, ParametrosACOBase parametros_base); // Constructor;
    void reset();

    virtual void resolver() = 0; // Resuelve el problema

    void mostrar_solucion(bool show_solucion); // Muestra la solución
    void set_mejor_feromonas();                // Setea las feromonas segun #hormigas/longitud_mejor_camino
    void abrir_file();
    void cerrar_file();
    std::string get_filename();
    void set_filename(std::string filename);
    Hormiga get_mejor_solucion();
    void exportar_solucion(std::chrono::microseconds duration, ACOArgs parametros_base);
    void exportar_mapa_resultados();

private:
    Hormiga mejor_hormiga; // Mejor solución de la iteracion actual

    int mejor_longitud = std::numeric_limits<int>::max(); // Mejor costo de la iteracion actual
};

#endif
