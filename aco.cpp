#include <iostream>
#include <algorithm>
#include <math.h>

#include "aco.h"
#include "graph.h"
#include "helpers.h"

using namespace std;

/* Constructor de la clase ACO
    Esta clase implementa la metaheuristica ACO para resolver el problema de recoleccion de residuos domiciliarios.
    El flujo de la metaheuristica es el siguiente:
    - Se inicializan las hormigas en un nodo inicial aleatorio dentro del conjunto de nodos iniciales.
    - Se inicializan las feromonas segun el parametro Tau, que es 1 por defecto TODO: hay una formula para establecer el mejor valor. ref: aco-book.
    - Se construye una solución para cada hormiga.
        + Se elige el siguiente nodo a visitar para cada hormiga.
        + Se visita el nodo elegido.
    - Se evapora las feromonas.
    - Se actualizan las feromonas.
    - Se repite el proceso hasta que se cumpla el criterio de parada.

    Parámetros:
        - graph: Puntero al grafo que se utilizará para resolver el problema
        - num_hormigas: Número de hormigas que se utilizarán para resolver el problema
*/
ACO::ACO(Graph *graph, int num_hormigas, bool param_debug)
{
    debug = param_debug;
    // Inicializa las hormigas.
    for (int i = 0; i < num_hormigas; i++)
    {
        // Crea una nueva hormiga
        Hormiga hormiga;
        hormiga.id = i;       

        // Establece la posición inicial de la hormiga de manera aleatorio entre la cantidad de nodos iniciales permitidos.
        int nodo_inicial = int(generar_numero_aleatorio(1, graph->metadatos.nodos_iniciales.size() - 1));
        hormiga.nodo_actual = &graph->metadatos.nodos_iniciales[nodo_inicial];

        // Inicializa un mapa para que la hormiga lleve la cuenta de las pasadas por los arcos.
        for (auto &par : graph->arcos) hormiga.arcosVisitados[par.second] = 0;

        // Añade la hormiga a la lista de hormigas
        hormigas.push_back(hormiga);
    }

    // Inicializa las feromonas.
    for (auto &par : graph->arcos)
    {
        Arco *arco = par.second; 
        Feromona feromona_inicial = {arco->origen, arco->destino, tau};
        feromonas[arco] = feromona_inicial;
    }

    // Establece el grafo.
    grafo = graph;
}

/* Resuelve el problema
    Este método resuelve el problema simplemente iterando el algoritmo hasta que se cumpla el criterio de parada.
    Algunas alternativas de mejora son:    
    - Establecer un criterio de parada basado en la calidad de las soluciones, mientras menos mejor.
    - Establecer un criterio de parada basado en el tiempo de ejecución.
    - Establecer un criterio de parada basado en la cantidad de iteraciones sin mejora, de manera local, global y/o por hormiga.    
    + Mas opciones en aco-book.

    Parámetros:
    - iteraciones_max: Número máximo de iteraciones que realizará el algoritmo
*/
void ACO::resolver(int iteraciones_max)
{    
    while (iteraciones < iteraciones_max)
    {
        iterar();
        iteraciones++;
    }
}

/*
    Itera el algoritmo
    Este método itera el algoritmo, moviendo todas las hormigas, evaporando las feromonas y actualizando las feromonas.
*/
void ACO::iterar()
{
    // Mueve todas las hormigas.
    for (auto &hormiga : hormigas) construirSolucion(hormiga);

    // evaporar feromonas
    for (auto &par : feromonas) par.second.cantidad *= (1 - rho);

    // Actualiza las feromonas.
    for (Hormiga &hormiga : hormigas)
    {
        for (auto &par : hormiga.arcosVisitados)
        {
            Arco *a = par.first;
            int pasadas = par.second;            
            feromonas[a].cantidad += (tau / ((hormiga.costo_camino + hormiga.longitud_camino) * pasadas));
        }
    }
}

/*
    Construye la solución para una hormiga
    Este método construye la solución para una hormiga, moviendola por el grafo hasta que se cumpla el criterio de parada.

    Parámetros:
    - hormiga: Referencia a la hormiga que se está moviendo
*/
void ACO::construirSolucion(Hormiga &hormiga)
{
    Nodo *actual = nullptr;
    while (!solucionCompleta(hormiga))
    {
        actual = hormiga.nodo_actual;
        if (debug) cout << "Hormiga numero "<< hormiga.id << " en el nodo " << actual->id << endl;
        Nodo *siguiente = eligeSiguiente(hormiga);
        visitar(hormiga, siguiente);
    }
}

/*
    Elige el siguiente nodo
    Este método elige el siguiente nodo a visitar para una hormiga segun una regla de transicion pseudo aleatoria proporcional.
    El primer for recorre los nodos adyacentes al nodo actual de la hormiga
    El segundo for recorre los arcos buscando si el arco es el que conecta el nodo actual con el nodo adyacente.
    Cuando se encuentra el arco, se calcula la probabilidad de transición, se normaliza y se elige el nodo siguiente.

    Parámetros:
    - hormiga: Referencia a la hormiga que se está moviendo

    Retorna:
    - Nodo: Puntero al nodo elegido
*/
Nodo *ACO::eligeSiguiente(Hormiga &hormiga)
{
    double total = 0.0;
    Nodo *nodo = nullptr;
    std::unordered_map<Arco *, double> probabilidad;

    for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id])
    {
        Arco *arco = nullptr;
        for (auto &a : hormiga.arcosVisitados)
        {
            if (a.first->origen->id == hormiga.nodo_actual->id && a.first->destino->id == i.first)
            {
                arco = a.first;
                break;
            }
        }
        double tau_eta = pow(feromonas.at(arco).cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
        probabilidad[arco] = tau_eta;
        total += tau_eta;
        if (debug) cout << "arco:" << arco->origen->id << " " << arco->destino->id << " tau_eta: " << tau_eta << endl;
    }
    for (auto &p : probabilidad)
    {
        p.second /= total;
        if (debug) cout << "arco:" << p.first->origen->id << " " << p.first->destino->id << " probabilidad: " << p.second << endl;
    }
    double r = generar_numero_aleatorio(0, 1.00);
    double acumulado = 0.0;
    for (auto &p : probabilidad)
    {
        acumulado += p.second;
        if (r <= acumulado)
        {
            nodo = p.first->destino;
            break;
        }
    }
    if (debug) cout << "r: " << r << endl;
    if (debug)  cout << "nodo elegido: " << nodo->id << endl;

    return nodo;
}

/*
    Visita el nodo siguiente
    Este método visita el nodo siguiente para una hormiga, actualizando el camino, el costo y la longitud del camino.

    Parámetros:
    - hormiga: Referencia a la hormiga que se está moviendo
    - nodo: Puntero al nodo que se va a visitar
*/
void ACO::visitar(Hormiga &hormiga, Nodo *nodo)
{    
    Arco *arco = nullptr;
    for (auto &a : hormiga.arcosVisitados)
    {
        if (a.first->origen->id == hormiga.nodo_actual->id && a.first->destino->id == nodo->id)
        {
            arco = a.first;
            break;
        }
    }

    hormiga.arcosVisitados[arco] += 1;
    hormiga.camino.push_back(*arco);
    hormiga.nodo_actual = nodo;
    hormiga.costo_camino += arco->costo_recoleccion + arco->costo_recorrido;
    hormiga.longitud_camino += 1;
    return;
}
/*
    Verifica si la solución es completa
    Este método verifica si la solución es completa, es decir, si se han visitado todos los arcos al menos una vez y si el nodo actual es uno de los nodos finales.

    Parámetros:
    - hormiga: Referencia a la hormiga que se está moviendo

    Retorna:
    - bool: True si la solución es completa, False en caso contrario.
*/
bool ACO::solucionCompleta(Hormiga &hormiga)
{    
    // Los arcos se han visitado al menos una vez
    bool completo = false;
    for (auto &par : hormiga.arcosVisitados)
    {
        if (par.second == 0)
        {
            completo = false;
            break;
        }
        else completo = true; 
    }

    // El nodo actual es uno de los nodos finales
    bool termino = false;
    for (auto &nodo_final : grafo->metadatos.nodos_termino)
    {
        if (hormiga.nodo_actual->id == nodo_final.id) termino = true;
    }
    return (completo && termino);
}

/*
    Muestra la solución
    Este método muestra la solución, mostrando el camino y el costo de cada hormiga.
*/
void ACO::mostrar_solucion()
{
    int mejor_hormiga = -1;    
    int mejor_costo = std::numeric_limits<int>::max();
    int mejor_longitud = std::numeric_limits<int>::max();
    
    
    for (auto &hormiga : hormigas)
    {
        if (hormiga.costo_camino < mejor_costo && hormiga.longitud_camino < mejor_longitud)
        {
            mejor_hormiga = hormiga.id;
            mejor_costo = hormiga.costo_camino;
            mejor_longitud = hormiga.longitud_camino;
        }

        cout << "Hormiga " << hormiga.id << endl;
        if (debug){
            cout << "Camino: ";
            for (auto& arco : hormiga.camino) {
                cout << "(" << arco.origen->id << "," << arco.destino->id << ") ";
            }
            cout << endl;
        }

        cout << "Costo: " << hormiga.costo_camino << endl;
        cout << "Longitud: " << hormiga.longitud_camino << endl;
        cout << endl;
    }
    for (int i = 0; i < 161; i++) cout << "-";
    cout << endl;
    cout << "✨🏆 Mejores resultados 🏆✨" << endl;
    cout << endl;
    cout << "Mejor hormiga: " << mejor_hormiga << " 🐜🥇" << endl;
    cout << "Mejor costo: " << mejor_costo << " ⏩" <<endl;
    cout << "Mejor longitud: " << mejor_longitud<< " ⚡" << endl;
    for (int i = 0; i < 161; i++) cout << "-";
    cout << endl;
    cout << endl;
}

/*
    Limpia la memoria y datos del algoritmo
    Este método limpia la memoria y datos del algoritmo, dejando el algoritmo en su estado inicial.
*/
void ACO::limpiar()
{
    for (auto &hormiga : hormigas)
    {
        hormiga.camino.clear();
        hormiga.arcosVisitados.clear();
        hormiga.longitud_camino = 0;
        hormiga.costo_camino = 0;
    }
    iteraciones = 0;
}