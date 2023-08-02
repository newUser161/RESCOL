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
ACO::ACO(Graph *graph, int num_hormigas, float alfa, float beta, float rho, float tau, bool param_debug)
{
    debug = param_debug;
    set_parametros(alfa, beta, rho, tau);
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
        for (auto &par : graph->arcos)
            hormiga.arcosVisitados[par.second] = 0;            

        // Añade la hormiga a la lista de hormigas
        hormigas.push_back(hormiga);
    }

    // Inicializa las feromonas.
    for (auto &par : graph->arcos)
    {
        Arco *arco = par.second;
        Feromona feromona_inicial = {arco->origen, arco->destino, tau};
        feromonas[arco] = feromona_inicial;
        for (auto &hormiga : hormigas)
            hormiga.feromonas_locales[arco] = feromona_inicial;

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
    int promedio_movil = 0;
    while (iteraciones < iteraciones_max)
    {
        iterar();
        mejor_solucion = guardar_mejor_solucion();
        
        // Promedio movil
        for (auto &hormiga : hormigas)
            promedio_movil = (promedio_movil * iteraciones + hormiga.costo_camino) / (iteraciones + 1);
        cout << "Iteracion: " << iteraciones << " Mejor costo: " << mejor_solucion.costo_camino << " Promedio movil: " << promedio_movil << endl;
        
        limpiar();
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
    for (auto &hormiga : hormigas) 
        construirSolucion(hormiga);
    // evaporar feromonas
    for (auto i = feromonas.begin(); i != feromonas.end(); i++)
    {
        i->second.cantidad *= (1 - rho);
    }

    // Actualiza las feromonas.
    for (Hormiga &hormiga : hormigas)
    {
        for (auto &par : hormiga.arcosVisitados)
        {
            Arco *a = par.first;
            int pasadas = par.second;            
            feromonas.at(a).cantidad += (tau / ((hormiga.costo_camino + hormiga.longitud_camino) * pasadas));
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
        if (debug)
            cout << "Hormiga numero " << hormiga.id << " en el nodo " << actual->id << endl;
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
    double r = generar_numero_aleatorio(0, 1.00);
    double acumulado = 0.0;
    double cantidad = 0.0;
    double tau_eta = 0.0;

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
        cantidad = hormiga.feromonas_locales[arco].cantidad; 
        if (arco->obligatoria == true){
            tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);                
        }  else {
            tau_eta = 1;
        }
        probabilidad[arco] = tau_eta;
        total += tau_eta;
        if (debug)
            cout << "arco:" << arco->origen->id << " " << arco->destino->id << " tau_eta: " << tau_eta << endl;
    }
    for (auto &p : probabilidad)
    {
        p.second /= total;
        if (debug)
            cout << "arco:" << p.first->origen->id << " " << p.first->destino->id << " probabilidad: " << p.second << endl;
    }
    for (auto &p : probabilidad)
    {
        acumulado += p.second;
        if (r <= acumulado)
        {
            nodo = p.first->destino;
            break;
        }
    }
    if (debug)
        cout << "r: " << r << endl;
    if (debug)
        cout << "nodo elegido: " << nodo->id << endl;

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
    arco->veces_recorrida += 1;
    hormiga.arcosVisitados[arco] += 1;
    hormiga.camino.push_back(*arco);
    if (hormiga.feromonas_locales[arco].cantidad < umbral_inferior)
    {
        hormiga.feromonas_locales[arco].cantidad = umbral_inferior;
    } else {
        hormiga.feromonas_locales[arco].cantidad *= (1-rho_secundario); 
    }
    hormiga.nodo_actual = nodo;
    if (arco->veces_recorrida == 1)
    {
        hormiga.costo_camino += arco->costo_recorrido + arco->costo_recoleccion;
    } else {
        hormiga.costo_camino += arco->costo_recorrido;
    }
    
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
        if (par.second == 0 && par.first->obligatoria == true) 
        {
            completo = false;
            break;
        }
        else
            completo = true;
    }

    // El nodo actual es uno de los nodos finales
    bool termino = false;
    for (auto &nodo_final : grafo->metadatos.nodos_termino)
    {
        if (hormiga.nodo_actual->id == nodo_final.id)
            termino = true;
    }
    return (completo && termino);
}

/*
    Muestra la solución
    Este método muestra la solución, mostrando el camino y el costo de cada hormiga.
*/
void ACO::mostrar_solucion(bool show_solucion)
{
    for (int i = 0; i < 161; i++)
        cout << "-";
    cout << endl;
    cout << "✨🏆 Mejor resultado 🏆✨" << endl;
    cout << endl;
    cout << "Mejor hormiga: " << mejor_solucion.id << " 🐜🥇" << endl;
    cout << "Mejor costo: " << mejor_solucion.costo_camino << " ⏩" << endl;
    cout << "Mejor longitud: " << mejor_solucion.longitud_camino << " ⚡" << endl;
    cout << "La solucion es: " << endl;
    if (show_solucion){
        for (auto &arco : mejor_solucion.camino)
        {
            cout << "(" << arco.origen->id << "," << arco.destino->id << ")";
            cout << " -> ";
        }
        cout << " 🏁 ";
        cout << endl;
        for (int i = 0; i < 161; i++)
            cout << "-";
        cout << endl;
        cout << endl;
    }
}

/*
    Este método guarda la mejor solución, es decir, la hormiga con el mejor(menor) costo y longitud de camino.

    Retorna:
    - Hormiga: La mejor hormiga
*/
Hormiga ACO::guardar_mejor_solucion()
{
    Hormiga mejor_hormiga;
    int mejor_costo = std::numeric_limits<int>::max();
    int mejor_longitud = std::numeric_limits<int>::max();

    for (auto &hormiga : hormigas)
    {
        if (hormiga.costo_camino < mejor_costo && hormiga.longitud_camino < mejor_longitud)
            mejor_hormiga = hormiga;
    }
    return mejor_hormiga;

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
        for (auto &par : grafo->arcos)
            hormiga.arcosVisitados[par.second] = 0;
        hormiga.longitud_camino = 0;
        hormiga.costo_camino = 0;
        hormiga.feromonas_locales = feromonas;
    }
}

void ACO::set_parametros(float alfa, float beta, float tau, float rho)
{
    this->alfa = alfa;
    this->beta = beta;
    this->tau = tau;
    this->rho = rho;
}