#include <iostream>
#include <algorithm>
#include <random>
#include <math.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>
#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <climits>

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
ACO::ACO(Graph *instancia, ACOArgs parametros_base)
{
    set_parametros(parametros_base);
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_c);

    instancia->metadatos.encabezado["METODO"] = metodo == 0 ? "AntSystem" : metodo == 1 ? "MaxMin"
                                                                                        : "ACS";
    nombre_metodo = instancia->metadatos.encabezado["METODO"];
    std::string prefijo = instancia->metadatos.encabezado["NOMBRE"] + "-" + instancia->metadatos.encabezado["METODO"] + "-";
    nombre_instancia_salida = instancia->metadatos.encabezado["NOMBRE"];

    // Construir la ruta al archivo en la carpeta "output"
    std::strftime(filename, 100, "%Y%m%d%H%M%S", now_tm); // Formato: -AAAAMMDDHHMMSS

    directorio_salida = ("Output/" + prefijo + std::string(filename));
    std::filesystem::create_directories(directorio_salida);
    std::string nombre_archivo_salida = directorio_salida.string() + "/" + std::string(filename) + ".txt";
    set_filename(nombre_archivo_salida);

    // Inicializa las hormigas.
    for (int i = 0; i < num_hormigas; i++)
    {
        // Crea una nueva hormiga
        Hormiga hormiga;
        hormiga.id = i;

        // Establece la posición inicial de la hormiga de manera aleatorio entre la cantidad de nodos iniciales permitidos.
        int nodo_inicial = int(generar_numero_aleatorio(1, instancia->metadatos.nodos_iniciales.size() - 1));
        hormiga.nodo_actual = &instancia->metadatos.nodos_iniciales[nodo_inicial];

        // Inicializa un mapa para que la hormiga lleve la cuenta de las pasadas por los arcos.
        for (auto &par : instancia->arcos)
        {
            hormiga.arcos_visitados_tour[par.second] = 0;
            hormiga.arcos_visitados_salida[par.second] = 0;
        }

        hormiga.arcos_no_visitados = hormiga.arcos_visitados_tour;

        // Añade la hormiga a la lista de hormigas
        hormigas.push_back(hormiga);
    }

    // Establece el grafo.
    grafo = instancia;
}

/*
    Itera el algoritmo
    Este método itera el algoritmo, moviendo todas las hormigas, evaporando las feromonas y actualizando las feromonas.
*/
void ACO::iterar()
{
    // Mueve todas las hormigas.
    for (auto &hormiga : hormigas)
    {
        construirSolucion(hormiga);
        limpiar_rastro();
        
        /*
        if (timeout_flag){
            continue;        
        }*/
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
    Nodo *siguiente = nullptr;
    acumulador_tiempo = 0;

    while (!solucionCompleta(hormiga))
    {
        auto start = std::chrono::high_resolution_clock::now();
        timeout_flag = false;
        actual = hormiga.nodo_actual;
        if (debug)
        {
            cout << "Hormiga numero " << hormiga.id << " en el nodo " << actual->id << endl;
        }
        siguiente = eligeSiguiente(hormiga);
        visitar(hormiga, siguiente);
        if (usar_oscilador == 1)
        {
            oscilador.oscilar();
        } else if (usar_oscilador == 2){
            oscilador.oscilar_caotico();
        } 
        //cout << "alfa: " << ACO::alfa << " beta: " << ACO::beta << " rho: " << ACO::rho << " tau: " << ACO::tau << endl;
        auto stop = std::chrono::high_resolution_clock::now();        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        acumulador_tiempo += duration.count();
        //cout <<"Iteracion: "<< iteraciones << " hormiga: "<< hormiga.id << " nodo: " << hormiga.nodo_actual->id << endl;
        /*
        if (acumulador_tiempo > timeout)
        {
            cout << "Timeout en hormiga " << hormiga.id << endl;
            timeout_flag = true;
            timeout_flag_global = true;
            hormiga.solucion_valida = false;
            break;
        }*/
    }
    //if (!timeout_flag)
    {
        if (usarMatrizSecundaria)
        {
            buscarSalida(hormiga);
        }
        
        file << "Epoca: " << epoca_actual << ", Evaluacion: " << evaluaciones << ", Mejor costo: " << mejor_costo << endl;

        hormiga.camino_final.insert(hormiga.camino_final.end(), hormiga.camino_tour.begin(), hormiga.camino_tour.end());
        hormiga.camino_final.insert(hormiga.camino_final.end(), hormiga.camino_salida.begin(), hormiga.camino_salida.end());
        hormiga.longitud_camino_final = hormiga.longitud_camino_tour + hormiga.longitud_camino_salida;
        hormiga.saltosSalida = hormiga.longitud_camino_final - hormiga.longitud_camino_tour;

        evaluaciones++;
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
        if (!hormiga.camino_tour.empty())
        {
            // si no esta vacio, se empiezan a comprobar los casos
            if (i.first->bidireccional == true)
            { // si es bidireccional, se comprueba que el siguiente arco no sea una vuelta en U o si es la unica opcion, en este ultimo caso, se agrega a las probabilidades de paso
                if ((hormiga.camino_tour.back().id != i.first->arco_reciproco->id) || (grafo->informacion_heuristica[hormiga.nodo_actual->id].size() == 1))
                {

                    Arco *arco = nullptr;
                    arco = i.first;
                    if (arco->veces_recorrida <= valor_limitador)
                    {

                        cantidad = hormiga.feromonas_locales[arco].cantidad;
                        tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
                        probabilidad[arco] = tau_eta;
                        total += tau_eta;
                        if (debug)
                            cout << "arco:" << arco->origen->id << " " << arco->destino->id << " tau_eta: " << tau_eta << endl;
                            
                    }else {
                        continue;
                    }

                    
                }
            }
            else
            { // si no es bidireccional, se agrega a las probabilidades de paso
                Arco *arco = nullptr;
                arco = i.first;
                if (arco->veces_recorrida <= valor_limitador)
                {

                    cantidad = hormiga.feromonas_locales[arco].cantidad;
                    tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
                    total += tau_eta;
                    probabilidad[arco] = tau_eta;
                } else {
                    continue;
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
    
    if (total == 0){ // si nos quedamos sin pasadas se elige cualquiera segun el metodo normal
        for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id]){
            // si no esta vacio, se empiezan a comprobar los casos
            if (i.first->bidireccional == true)
            { // si es bidireccional, se comprueba que el siguiente arco no sea una vuelta en U o si es la unica opcion, en este ultimo caso, se agrega a las probabilidades de paso
                if ((hormiga.camino_tour.back().id != i.first->arco_reciproco->id) || (grafo->informacion_heuristica[hormiga.nodo_actual->id].size() == 1))
                {

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
            else
            { // si no es bidireccional, se agrega a las probabilidades de paso
                Arco *arco = nullptr;
                arco = i.first;

                cantidad = hormiga.feromonas_locales[arco].cantidad;
                tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta);
                total += tau_eta;
                probabilidad[arco] = tau_eta;
                
            }
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
    if (nodo == nullptr){
        cout << "Nodo actual sin salida: " << hormiga.nodo_actual->id<< endl; // usar exepciones o algo try catch
        cout << "nodo nulo" << endl; // usar exepciones o algo try catch
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
void ACO::visitar(Hormiga &hormiga, Nodo *nodo)
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

    hormiga.arcos_no_visitados.erase(arco);
    if (arco->bidireccional) {
        hormiga.arcos_no_visitados.erase(arco->arco_reciproco);
    }

    hormiga.nodo_actual = nodo;
    hormiga.longitud_camino_tour += 1;

    if (arco->veces_recorrida == 1)
    {
        if (hormiga.feromonas_locales[arco].cantidad < umbral_inferior)
        {
            hormiga.feromonas_locales[arco].cantidad = umbral_inferior;
        }
        else
        {
            hormiga.feromonas_locales[arco].cantidad *= (1 - rho);
        }
        hormiga.costo_camino += arco->costo_recorrido + arco->costo_recoleccion;
    }
    else
    {
        if (hormiga.feromonas_locales[arco].cantidad < umbral_inferior)
        {
            hormiga.feromonas_locales[arco].cantidad = umbral_inferior;
        }
        else
        {
            hormiga.feromonas_locales[arco].cantidad *= (1 - rho_secundario);
        }
        hormiga.costo_camino += arco->costo_recorrido;
    }

    return;
}

void ACO::buscarSalida(Hormiga &hormiga)
{

    while (!enNodoTerminal(hormiga)) //
    {
        Nodo *nodo = nullptr;
        std::unordered_map<Arco *, double> probabilidad;
        double total = 0.0;
        double r = generar_numero_aleatorio(0, 1.00);
        double acumulado = 0.0;
        double cantidad = 0.0;
        double tau_eta = 0.0;

        for (auto i : grafo->informacion_heuristica[hormiga.nodo_actual->id])
        {
            Arco *arco = nullptr;
            arco = i.first;
            cantidad = feromonas_salida[arco].cantidad;
            tau_eta = pow(cantidad, alfa) * pow(grafo->informacion_heuristica[hormiga.nodo_actual->id][i.first], beta_salida);
            probabilidad[arco] = tau_eta;
            total += tau_eta;
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
        hormiga.arcos_visitados_salida[arco] += 1;
        hormiga.camino_salida.push_back(*arco);
        hormiga.nodo_actual = nodo;
        hormiga.longitud_camino_salida += 1;
        hormiga.costo_camino += arco->costo_recorrido;
    }

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
    /*
    bool completo = false;
    for (auto &par : hormiga.arcos_visitados_tour)
    {
        if (par.second == 0)
        {
            if (par.first->bidireccional == true)
            {
                auto it = hormiga.arcos_visitados_tour.find(par.first->arco_reciproco);
                if (it != hormiga.arcos_visitados_tour.end())
                {
                    if (it->second == 0)
                    {
                        completo = false;
                        break;
                    }
                }
            }
            else
            {
                completo = false;
                break;
            }
        }
        else
        {
            completo = true;
        }
    }
    */
    bool completo = hormiga.arcos_no_visitados.empty();
    if (!usarMatrizSecundaria)
    {
        bool terminado = enNodoTerminal(hormiga);
        return (completo && terminado);
    }

    return completo;
}

/*
    Verifica si la hormiga esta en un nodo final

    Parámetros:
    - hormiga: Referencia a la hormiga que se está moviendo

    Retorna:
    - bool: True si la hormiga esta en un nodo final
*/
bool ACO::enNodoTerminal(Hormiga &hormiga)
{
    // El nodo actual es uno de los nodos finales
    bool termino = false;
    for (auto &nodo_final : grafo->metadatos.nodos_termino)
    {
        if (hormiga.nodo_actual->id == nodo_final.id)
        {
            termino = true;
            break;
        }
    }

    return termino;
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
    if (timeout_flag_global)
    {
        cout << "Mejor costo: " << "inf" << " ⏩" << endl;
    } else {
        cout << "Mejor costo: " << mejor_solucion.costo_camino << " ⏩" << endl;
    }
    cout << "Mejor longitud: " << mejor_solucion.longitud_camino_final << " ⚡" << endl;

    if (show_solucion)
    {
        cout << "La solución es: " << endl;
        {
            for (auto &arco : mejor_solucion.camino_final)
            {
                cout << arco.origen->id << " -> ";
            }
            cout << mejor_solucion.camino_final.back().destino->id;
            cout << " -> 🏁 ";
            cout << endl;
            for (int i = 0; i < 161; i++)
                cout << "-";
            cout << endl;
            cout << endl;
        }
    }
}

void ACO::exportar_solucion(std::chrono::microseconds duration, ACOArgs parametros_base)
{
    // suma de costos de recoleccion de los arcos
    int suma_recoleccion = 0;
    int suma_recorrer = 0;
    int costo_pesos_pasada = 0;
    for (auto &arco : grafo->arcos)
    {
        suma_recoleccion += arco.second->costo_recoleccion;
    }
    for (auto &arco : mejor_solucion.camino_final)
    {
        suma_recorrer += arco.costo_recorrido;
    }
    costo_pesos_pasada = suma_recorrer - suma_recoleccion;



    std::string resultados_csv = "resultados_csv.txt";
    ruta_archivo_salida_csv = directorio_salida.string() + "/" + resultados_csv;
    std::ofstream archivo_salida_csv(ruta_archivo_salida_csv);
    archivo_salida_csv << nombre_instancia_salida << ",";
    archivo_salida_csv << nombre_metodo << ",";
    //archivo_salida_csv << costo_pesos_pasada << ",";
    archivo_salida_csv << mejor_solucion.costo_camino << ",";
    archivo_salida_csv << mejor_solucion.longitud_camino_final << ",";
    archivo_salida_csv << mejor_solucion.camino_tour.front().origen->id << ",";
    if (mejor_solucion.camino_salida.empty())
        archivo_salida_csv << mejor_solucion.camino_tour.back().destino->id << ",";
    else
        archivo_salida_csv << mejor_solucion.camino_salida.back().destino->id << ",";
    archivo_salida_csv << duration.count() << ",";
    archivo_salida_csv << mejor_solucion.longitud_camino_tour << ",";     //
    archivo_salida_csv << mejor_solucion.saltosSalida << ","; //
    archivo_salida_csv << grafo->arcos.size() << ",";
    archivo_salida_csv << suma_recoleccion << ",";
    archivo_salida_csv << suma_recorrer << endl;
    archivo_salida_csv.close();


    std::string config_csv = "config_csv.txt";
    ruta_archivo_config_salida_csv = directorio_salida.string() + "/" + config_csv;
    std::ofstream archivo_config_salida_csv(ruta_archivo_config_salida_csv);
    archivo_config_salida_csv << usarMatrizSecundaria << ",";
    archivo_config_salida_csv << parametros_base.oscilador << ",";
    archivo_config_salida_csv << parametros_base.limitador << ",";
    archivo_config_salida_csv << parametros_base.valor_limitador << ",";
    archivo_config_salida_csv << parametros_base.beta_0 << ",";
    archivo_config_salida_csv << alfa << ",";
    archivo_config_salida_csv << beta << ",";
    archivo_config_salida_csv << beta_salida << ",";
    archivo_config_salida_csv << rho << ",";
    archivo_config_salida_csv << rho_secundario << ",";
    archivo_config_salida_csv << rho_salida << ",";
    archivo_config_salida_csv << tau << ",";
    if (usar_iteraciones){
        archivo_config_salida_csv << parametros_base.iteraciones_max << ",";
        archivo_config_salida_csv << "-1" << ",";
    } else {
        archivo_config_salida_csv << "-1" << ",";
        archivo_config_salida_csv << parametros_base.evaluaciones_maximas << ",";
    }
    archivo_config_salida_csv << parametros_base.umbral_inferior << ",";
    archivo_config_salida_csv << parametros_base.num_hormigas << ",";
    archivo_config_salida_csv << parametros_base.epocas << ",";
    if (parametros_base.full_aleatorio){
        archivo_config_salida_csv << "-1" << ",";
    } else {
        archivo_config_salida_csv << parametros_base.semilla << ",";
    }
    archivo_config_salida_csv << parametros_base.umbral_superior << ",";
    archivo_config_salida_csv << parametros_base.umbral_sin_mejora_limite << ",";
    archivo_config_salida_csv << parametros_base.a << ",";
    archivo_config_salida_csv << parametros_base.q_0 << ",";
    archivo_config_salida_csv << parametros_base.csi << ",";
    archivo_config_salida_csv << parametros_base.usar_iteraciones << ",";
    archivo_config_salida_csv << parametros_base.usar_evaluaciones << ",";
    archivo_config_salida_csv << parametros_base.irace << ",";
    archivo_config_salida_csv << parametros_base.silence << ",";
    archivo_config_salida_csv << parametros_base.full_aleatorio << endl;
    archivo_config_salida_csv.close();
    
    if (parametros_base.irace || parametros_base.silence ){
        if (usar_bd){
            std::stringstream ss;
            ss << "python POSTDB.py " << ruta_archivo_salida_csv << " " << ruta_archivo_config_salida_csv;
            std::string comando3 = ss.str();
            std::system(comando3.c_str()); 
        }
    }



    std::string resultados = "resultados.txt";
    std::string ruta_archivo_salida = directorio_salida.string() + "/" + resultados;
    std::ofstream archivo_salida(ruta_archivo_salida);
    //archivo_salida << "Mejor hormiga: " << mejor_solucion.id << endl;
    archivo_salida << "Mejor longitud: " << mejor_solucion.longitud_camino_final << endl;
    archivo_salida << "Longitud tour: " << mejor_solucion.saltosTour << endl;     //
    archivo_salida << "Longitud salida: " << mejor_solucion.saltosSalida << endl; //
    archivo_salida << "Cantidad arcos: " << grafo->arcos.size() << endl;
    archivo_salida << "Costo recoleccion: " << suma_recoleccion << endl;
    archivo_salida << "Costo recorrer: " << suma_recorrer << endl;
    archivo_salida << "Costo pesos pasada: " << costo_pesos_pasada << endl;
    archivo_salida << "Mejor costo: " << mejor_solucion.costo_camino << endl;
    archivo_salida << "Nodo inicio: " << mejor_solucion.camino_tour.front().origen->id << endl;
    if (mejor_solucion.camino_salida.empty())
        archivo_salida << "Nodo fin: " << mejor_solucion.camino_tour.back().destino->id << endl;
    else
        archivo_salida << "Nodo fin: " << mejor_solucion.camino_salida.back().destino->id << endl;
    archivo_salida << "Tiempo de resolucion: " << duration.count() << " microsegundos" << endl;
    archivo_salida << "La solución es:" << endl;
    for (auto &arco : mejor_solucion.camino_final)
    {
        archivo_salida << arco.origen->id << endl;
    }
    archivo_salida << mejor_solucion.camino_final.back().destino->id << endl;
    archivo_salida.close();

    cout << "La solución es: " << endl;
    // Abre el archivo para escribir
    std::ofstream archivo("Temp/camino.txt");
    // Escribe cada arco y la cantidad de veces que se pasó por él
    for (const auto &arco : mejor_solucion.camino_final)
    {
        archivo << arco.origen->id << endl;
    }
    archivo << mejor_solucion.camino_final.back().destino->id << endl;
    archivo.close();
}

void ACO::exportar_mapa_resultados()
{
    for (auto &par : mejor_solucion.arcos_visitados_tour)
    {
        Arco *arco = par.first;
        int suma = par.second;

        // Sumar el valor del segundo mapa
        if (mejor_solucion.arcos_visitados_salida.find(arco) != mejor_solucion.arcos_visitados_salida.end())
        {
            suma += mejor_solucion.arcos_visitados_salida[arco];
        }

        // Agregar al mapa de suma
        mejor_solucion.arcos_visitados_final[arco] = suma;
    }
    // Abre el archivo para escribir
    std::ofstream archivo("Temp/mapa_resultados.txt");
    // Escribe cada arco y la cantidad de veces que se pasó por él
    // sumar arcos visitados de salida
    for (const auto &arco : mejor_solucion.arcos_visitados_final)
    {
        auto it = mejor_solucion.arcos_visitados_final.find(arco.first);
        if (it != mejor_solucion.arcos_visitados_final.end())
        {
            archivo << "(" << arco.first->origen->id << ", " << arco.first->destino->id << ") "
                    << " : " << (arco.second) << endl;
        }
    }

    archivo.close();
}
/*
    Este método guarda la mejor solución, es decir, la hormiga con el mejor(menor) costo y longitud de camino.

    Retorna:
    - Hormiga: La mejor hormiga
*/
Hormiga ACO::guardar_mejor_solucion_iteracion()
{

    for (auto &hormiga : hormigas)
    {
        if (hormiga.solucion_valida){
            if (hormiga.costo_camino < mejor_costo && (hormiga.longitud_camino_tour + hormiga.longitud_camino_salida) < mejor_longitud)
            {
                mejor_costo = hormiga.costo_camino;
                mejor_longitud = hormiga.longitud_camino_final;
                mejor_hormiga = hormiga;
            }
        } else {
            continue;
        }
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

        hormiga.camino_tour.clear();
        hormiga.camino_salida.clear();
        hormiga.camino_final.clear();
        hormiga.solucion_valida = true;
        for (auto &par : grafo->arcos)
        {
            hormiga.arcos_visitados_tour[par.second] = 0;
            hormiga.arcos_visitados_salida[par.second] = 0;
        }
        hormiga.arcos_no_visitados = hormiga.arcos_visitados_tour;
        hormiga.longitud_camino_tour = 0;
        hormiga.longitud_camino_salida = 0;
        hormiga.longitud_camino_final = 0;
        hormiga.saltosSalida = 0;
        hormiga.saltosTour = 0;
        hormiga.costo_camino = 0;
        hormiga.feromonas_locales = feromonas;
        hormiga.nodo_actual = &grafo->metadatos.nodos_iniciales[int((generar_numero_aleatorio(0, grafo->metadatos.nodos_iniciales.size() - 1)))];

    }
}
void ACO::limpiar_rastro()
{
    for (auto &arco : grafo->arcos)
        arco.second->veces_recorrida = 0;
}

void ACO::reset()
{
    iteraciones = 0;
    limpiar();
    for (auto &arco : grafo->arcos)
        arco.second->veces_recorrida = 0;
}

void ACO::abrir_file()
{
    file.open(nombre_archivo_salida);
}

void ACO::cerrar_file()
{
    file.close();
}

std::string ACO::get_filename()
{
    return nombre_archivo_salida;
}

void ACO::set_filename(std::string filename)
{
    this->nombre_archivo_salida = filename;
}

void ACO::set_parametros(const ACOArgs parametros_base)
{
    nombre_instancia = parametros_base.nombre_instancia;
    metodo = parametros_base.metodo;

    alfa = parametros_base.alfa;
    float alfa_inc = parametros_base.inc_alfa;
    float alfa_min = parametros_base.min_alfa;
    float alfa_max = parametros_base.max_alfa;
    oscilador.agregarParametro(alfa,alfa_inc,alfa_min,alfa_max);

    
    beta_salida = parametros_base.beta_salida;
    //oscilador.agregarParametro(beta_salida,0.01,1,5); // el oscilador rompe la matriz de salida, no tocar


    rho = parametros_base.rho;
    float rho_inc = parametros_base.inc_rho;
    float rho_min = parametros_base.min_rho;
    float rho_max = parametros_base.max_rho;
    oscilador.agregarParametro(rho,rho_inc,rho_min,rho_max);

    rho_secundario = parametros_base.rho_secundario;
    float rho_secundario_inc = parametros_base.inc_rho_secundario;
    float rho_secundario_min = parametros_base.min_rho_secundario;
    float rho_secundario_max = parametros_base.max_rho_secundario;
    oscilador.agregarParametro(rho_secundario,rho_secundario_inc,rho_secundario_min,rho_secundario_max); 

    rho_salida = parametros_base.rho_salida;
    //oscilador.agregarParametro(rho_salida,0.01,0.1,0.5); // el oscilador rompe la matriz de salida, no tocar

    tau = parametros_base.tau;
    //oscilador.agregarParametro(tau,0.01,1,5);
    umbral_inferior = parametros_base.umbral_inferior;
    num_hormigas = parametros_base.num_hormigas;
    epocas = parametros_base.epocas;
    usarMatrizSecundaria = parametros_base.usaMatrizSecundaria;
    epoca_actual = 0;
    usar_iteraciones = parametros_base.usar_iteraciones;
    usar_evaluaciones = parametros_base.usar_evaluaciones;
    if (usar_evaluaciones && usar_iteraciones){
        cout << "No se puede usar ambos criterios de parada" << endl;
        exit(1);
    } else{
        if (usar_iteraciones){
            iteraciones_max = parametros_base.iteraciones_max;        
        } else if (usar_evaluaciones){
            evaluaciones_maximas = parametros_base.evaluaciones_maximas;
            iteraciones_max = evaluaciones_maximas / num_hormigas;
        } else{
            cout << "No se ha especificado un criterio de parada" << endl;
            //exit(1);
            //Para debug
            cout << "Usando iteraciones" << endl;
            iteraciones_max = parametros_base.iteraciones_max;        
            //

        }
    } 
    usar_oscilador = parametros_base.oscilador;
    usar_bd = parametros_base.conectar_bd;
    beta = parametros_base.beta;
    bool beta0 = parametros_base.beta_0;
    if (beta0) {
        beta = 0;     
    } else {
        float beta_inc = parametros_base.inc_beta;
        float beta_min = parametros_base.min_beta;
        float beta_max = parametros_base.max_beta;
        oscilador.agregarParametro(beta,beta_inc,beta_min,beta_max); 
    }
    bool usar_limitador = parametros_base.limitador;
    if (usar_limitador){
        valor_limitador = parametros_base.valor_limitador;        
    } else {
        valor_limitador = INT_MAX;
    }


    
}

Hormiga ACO::get_mejor_solucion()
{
    return mejor_solucion;
}
