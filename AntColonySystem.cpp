#include "AntColonySystem.h"
#include "aco.h"
#include <iostream>
#include "helpers.h"

AntColonySystem::AntColonySystem(Graph *instancia, ACSArgs parametros) : ACO(instancia, parametros)
{
    set_parametrosACS(parametros);
    inicializar_feromonas();
    precomputarListaInformacionHeuristica();
}

/* Resuelve el problema
    Este método resuelve el problema simplemente iterando el algoritmo hasta que se cumpla el criterio de parada.
    Algunas alternativas de mejora son:
    - Establecer un criterio de parada basado en la calidad de las soluciones, mientras menos mejor.
    - Establecer un criterio de parada basado en el tiempo de ejecución.
    - Establecer un criterio de parada basado en la cantidad de iteraciones sin mejora, de manera local, global y/o por hormiga.
    + Mas opciones en aco-book.
*/
void AntColonySystem::resolver()
{
    while (iteraciones < iteraciones_max)
    {
        iterar();
        limpiar();
        iteraciones++;
    }
};

void AntColonySystem::iterar()
{

    for (auto &hormiga : hormigas)
    {
        Nodo *actual = nullptr;
        while (!ACO::solucionCompleta(hormiga))
        {
            actual = hormiga.nodo_actual;
            if (debug)
                cout << "Hormiga numero " << hormiga.id << " en el nodo " << actual->id << endl;
            Nodo *siguiente = eligeSiguiente(hormiga);
            visitar(hormiga, siguiente);
        }
        file << "Epoca: " << epoca_actual << ", Evaluacion: " << evaluaciones << ", Mejor costo: " << mejor_costo << endl;
        // cout << "Epoca: " << epoca_actual << ", Evaluacion: " << evaluaciones << ", Mejor costo: " << mejor_costo << endl;
        evaluaciones++;
    }
    mejor_solucion = guardar_mejor_solucion_iteracion();

    // Actualiza las feromonas.
    for (auto &par : mejor_solucion.arcos_visitados_tour)
    {

        feromonas.at(par.first).cantidad += (((1 - rho) * feromonas.at(par.first).cantidad) + (rho * (par.first->veces_recorrida / mejor_solucion.costo_camino)));
        cout << "cantidad: " << feromonas.at(par.first).cantidad << endl;
    }
    recalcularListaInformacionHeuristica();
}

/*
    Guarda la mejor solución de la iteración
    Este método guarda la mejor solución de la iteración, para luego ser utilizada en la actualización de feromonas.
*/
void AntColonySystem::inicializar_feromonas()
{

    for (auto &par : grafo->arcos)
    {
        Arco *arco = par.second;
        Feromona feromona_inicial = {arco->origen, arco->destino, tau};
        feromonas[arco] = feromona_inicial;
        for (auto &hormiga : hormigas)
            hormiga.feromonas_locales[arco] = feromona_inicial;
    }
}

void AntColonySystem::set_parametrosACS(ACSArgs parametros)
{
    q_0 = parametros.q_0;
    csi = parametros.csi;
    alfa = 1;
    tau = 1; // grafo->nodos.size(); // TODO: Revisar esto
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
Nodo *AntColonySystem::eligeSiguiente(Hormiga &hormiga)
{
    double total = 0.0;
    double r = generar_numero_aleatorio(0, 1.00);
    double q = generar_numero_aleatorio(0, 1.00);
    double acumulado = 0.0;
    double cantidad = 0.0;
    double tau_eta = 0.0;

    Nodo *nodo = nullptr;
    std::unordered_map<Arco *, double> probabilidad;
    if (q < q_0)
    {
        // seleccionar primera entrada de la lista ordenada por calidad, esta lista se crea y ordena en el constructor de la hormiga para ACS
        auto &contenedor = mapaACS[hormiga.nodo_actual->id];
        for (auto it = contenedor.begin(); it != contenedor.end(); ++it)
        {
            Arco *arco = it->first;
            nodo = contenedor.begin().base()->first->destino;
        }
    }
    else
    {
        for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id])
        {
            Arco *arco = nullptr;
            arco = i.first;
            cantidad = hormiga.feromonas_locales[arco].cantidad;
            if (arco->obligatoria == true)
            {
                tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
            }
            else
            {
                tau_eta = 1;
            }
            probabilidad[arco] = tau_eta;
            total += tau_eta;
            if (debug)
                cout << "arco:" << arco->origen->id << " " << arco->destino->id << " tau_eta: " << tau_eta << endl;
        }
        for (auto &p : probabilidad)
        {
            acumulado += p.second / total;
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
    }
    if (nodo == nullptr)
    {
        cout << "No hay nodos disponibles" << endl;
    }
    return nodo;
}

/*
    Visita el nodo siguiente
    Este método visita el nodo siguiente para una hormiga, actualizando el camino, el costo y la longitud del camino.

    Parámetros:
    - hormiga: Referencia a la hormiga que se está moviendo
    - nodo: Puntero al nodo que se va a visitar
*/
void AntColonySystem::visitar(Hormiga &hormiga, Nodo *nodo)
{

    Arco *arco = nullptr;

    for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id])
    {
        if (i.first->destino->id == nodo->id)
        {
            arco = i.first;
            break;
        }
    }
    arco->veces_recorrida += 1;
    hormiga.arcos_visitados_tour[arco] += 1;
    hormiga.camino_tour.push_back(*arco);
    hormiga.nodo_actual = nodo;
    hormiga.longitud_camino_tour += 1;

    if (arco->veces_recorrida == 1)
    {
        hormiga.feromonas_locales[arco].cantidad *= (1 - rho_secundario);
        hormiga.costo_camino += arco->costo_recorrido + arco->costo_recoleccion;
    }
    else
    {
        hormiga.feromonas_locales[arco].cantidad *= (1 - rho_secundario);
        hormiga.costo_camino += arco->costo_recorrido;
    }

    return;
}

void AntColonySystem::precomputarListaInformacionHeuristica()
{
    // Cambiamos el tipo a un vector de pares
    std::unordered_map<int, std::vector<std::pair<Arco *, ArcoInfo>>> mapaACStemp;

    for (auto &par : grafo->informacion_heuristica)
    {
        int id_nodo = par.first;

        // Inicializamos el vector para este nodo
        std::vector<std::pair<Arco *, ArcoInfo>> vec;

        // Llenamos el vector
        for (auto &par2 : grafo->informacion_heuristica[id_nodo])
        {
            ArcoInfo info = {par2.second, false, 0};
            vec.push_back({par2.first, info});
        }

        // Ordenamos el vector según los valores de calidad
        std::sort(vec.begin(), vec.end(), [](const auto &a, const auto &b)
                  { return a.second.calidad > b.second.calidad; });

        // Asignamos el vector ordenado al nodo correspondiente
        mapaACStemp[id_nodo] = vec;
    }
    mapaACS = mapaACStemp;
}

void AntColonySystem::recalcularListaInformacionHeuristica()
{
    for (auto &par : mapaACS)
    {
        int id_nodo = par.first;
        std::vector<std::pair<Arco *, ArcoInfo>> vec;
        vec = mapaACS[id_nodo];
        std::sort(vec.begin(), vec.end(), [](const auto &a, const auto &b)
                  { return a.second.calidad > b.second.calidad; });
    }
}
