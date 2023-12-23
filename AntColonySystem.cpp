#include "AntColonySystem.h"
#include "aco.h"
#include <iostream>
#include "helpers.h"

AntColonySystem::AntColonySystem(Graph *instancia, ACOArgs parametros) : ACO(instancia, parametros)
{
    set_parametrosACS(parametros);
    inicializar_feromonas();
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
        cout << "Iteracion: " << iteraciones << endl;
        iterar();
        limpiar();
        iteraciones++;
    }
};

void AntColonySystem::iterar()
{

    for (auto &hormiga : hormigas)
    {
        acumulador_tiempo = 0;
        Nodo *actual = nullptr;
        while (!ACO::solucionCompleta(hormiga))
        {
            auto start = std::chrono::high_resolution_clock::now();
            timeout_flag = false;
            actual = hormiga.nodo_actual;
            if (debug)
                cout << "Hormiga numero " << hormiga.id << " en el nodo " << actual->id << endl;
            Nodo *siguiente = eligeSiguiente(hormiga);
            visitar(hormiga, siguiente);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            acumulador_tiempo += duration.count();
            if (acumulador_tiempo > timeout)
            {
                cout << "Timeout en hormiga " << hormiga.id << endl;
                timeout_flag = true;
                timeout_flag_global = true;
                hormiga.solucion_valida = false;
                break;
            }
        }
        if (!timeout_flag)
        {

            if (usarMatrizSecundaria)
            {
                buscarSalida(hormiga);
            }
            file << "Epoca: " << epoca_actual << ", Evaluacion: " << evaluaciones << ", Mejor costo: " << mejor_costo << endl;

            hormiga.camino_final.insert(hormiga.camino_final.end(), hormiga.camino_tour.begin(), hormiga.camino_tour.end());
            hormiga.camino_final.insert(hormiga.camino_final.end(), hormiga.camino_salida.begin(), hormiga.camino_salida.end());
            hormiga.longitud_camino_final = hormiga.longitud_camino_tour + hormiga.longitud_camino_salida;

            evaluaciones++;
        }
    }
    mejor_solucion = guardar_mejor_solucion_iteracion();
    
    // Actualiza las feromonas.
    for (auto &par : mejor_solucion.arcos_visitados_tour)
    {

        feromonas.at(par.first).cantidad += (((1 - rho) * feromonas.at(par.first).cantidad) + (rho * (par.first->veces_recorrida / mejor_solucion.costo_camino)));
        if (feromonas.at(par.first).cantidad < umbral_inferior)
        {
            feromonas.at(par.first).cantidad = umbral_inferior;
        }
    }
    for (auto &arco : grafo->arcos)
        arco.second->veces_recorrida = 0;
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
        feromonas_salida[arco] = feromona_inicial;
        feromonas[arco] = feromona_inicial;
        for (auto &hormiga : hormigas)
            hormiga.feromonas_locales[arco] = feromona_inicial;
    }
}

void AntColonySystem::set_parametrosACS(ACOArgs parametros)
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
    double cantidad2 = 0.0;
    double tau_eta = 0.0;

    Nodo *nodo = nullptr;
    std::unordered_map<Arco *, double> probabilidad;
    if (q < q_0)
    {
        double cantidad_max = 0;
        // Seleccionar el arco con mas feromonas 
        for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id]){
            cantidad = i.second;
            cantidad2 = feromonas[i.first].cantidad;
            double calculo = cantidad2 * pow(cantidad,beta);

            if (cantidad2 > cantidad_max){
                cantidad_max = cantidad;
                nodo = i.first->destino;
            }
        }
    }
    else
    {
        for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id])
        {
            if (!hormiga.camino_tour.empty())
            {
                // si no esta vacio, se empiezan a comprobar los casos
                if (i.first->bidireccional == true)
                { // si es bidireccional, se comprueba que el siguiente arco no sea una vuelta en U o si es la unica opcion, en este ultimo caso, se agrega a las probabilidades de paso
                    if ((hormiga.camino_tour.back().id != i.first->arco_reciproco->id) || (grafo->informacion_heuristica[hormiga.nodo_actual->id].size() == 1))
                    {

                        Arco *arco = nullptr;
                        arco = i.first;
                        // if (arco->veces_recorrida <= 4)
                        {

                            cantidad = hormiga.feromonas_locales[arco].cantidad;
                            tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
                            probabilidad[arco] = tau_eta;
                            total += tau_eta;
                            if (debug)
                                cout << "arco:" << arco->origen->id << " " << arco->destino->id << " tau_eta: " << tau_eta << endl;
                        }
                    }
                }
                else
                { // si no es bidireccional, se agrega a las probabilidades de paso
                    Arco *arco = nullptr;
                    arco = i.first;
                    // if (arco->veces_recorrida <= 4)
                    {

                        cantidad = hormiga.feromonas_locales[arco].cantidad;
                        tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
                        total += tau_eta;
                        probabilidad[arco] = tau_eta;
                    }
                }
            }
            else
            {
                // si el camino esta vacio, simplemente se agrega el primero que encuentre sin comprobaciones extra
                Arco *arco = nullptr;
                arco = i.first;
                cantidad = hormiga.feromonas_locales[arco].cantidad;
                tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
                probabilidad[arco] = tau_eta;
                total += tau_eta;
                if (debug)
                    cout << "arco:" << arco->origen->id << " " << arco->destino->id << " tau_eta: " << tau_eta << endl;
            }
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
    {/*
        */
        if (hormiga.feromonas_locales[arco].cantidad < umbral_inferior)
        {
            hormiga.feromonas_locales[arco].cantidad = umbral_inferior;
        }
        else
        {
            hormiga.feromonas_locales[arco].cantidad *= (1 - rho);
        }
        //feromonas[arco].cantidad = ((1-csi)*feromonas[arco].cantidad)+ (csi*tau) ;
        hormiga.costo_camino += arco->costo_recorrido + arco->costo_recoleccion;
    }
    else
    {
        /*
        */
        if (hormiga.feromonas_locales[arco].cantidad < umbral_inferior)
        {
            hormiga.feromonas_locales[arco].cantidad = umbral_inferior;
        }
        else
        {
            hormiga.feromonas_locales[arco].cantidad *= (1 - rho_secundario);
        }
        //feromonas[arco].cantidad = ((1-csi)*feromonas[arco].cantidad)+ (csi*tau) ;
        hormiga.costo_camino += arco->costo_recorrido;
    }
    

    return;
}
