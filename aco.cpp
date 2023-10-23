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

    std::string prefijo = instancia->metadatos.encabezado["NOMBRE"] + "-" + instancia->metadatos.encabezado["METODO"] + "-";
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
    saltos_salida_iteracion = 0;
    for (auto &hormiga : hormigas)
    {
        construirSolucion(hormiga);
        limpiar_rastro();
        saltos_salida_iteracion += hormiga.saltos_hormiga;
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
    while (!solucionCompleta(hormiga))
    {
        actual = hormiga.nodo_actual;
        if (debug)
            cout << "Hormiga numero " << hormiga.id << " en el nodo " << actual->id << endl;
        siguiente = eligeSiguiente(hormiga);
        visitar(hormiga, siguiente);
    }
    hormiga.saltosTour = hormiga.longitud_camino_tour;
    if (usarMatrizSecundaria)
    {
        buscarSalida(hormiga);
    }
    hormiga.saltosSalida = hormiga.longitud_camino_tour - hormiga.saltosTour;
    file << "Epoca: " << epoca_actual << ", Evaluacion: " << evaluaciones << ", Mejor costo: " << mejor_costo << endl;
    
    hormiga.camino_final.insert(hormiga.camino_final.end(), hormiga.camino_tour.begin(), hormiga.camino_tour.end());
    hormiga.camino_final.insert(hormiga.camino_final.end(), hormiga.camino_salida.begin(), hormiga.camino_salida.end());
    hormiga.longitud_camino_final = hormiga.longitud_camino_tour + hormiga.longitud_camino_salida;

    evaluaciones++;
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
            }
            else
            {   // si no es bidireccional, se agrega a las probabilidades de paso
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
            }
        }
        else
        {
            // si el camino esta vacio, simplemente se agrega el primero que encuentre sin comprobaciones extra
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
    if (nodo == nullptr)
        cout << "nodo nulo" << endl;
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
    bool completo = false;
    for (auto &par : hormiga.arcos_visitados_tour)
    {
        if (par.second == 0 && par.first->obligatoria == true)
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
        else if (par.first->obligatoria == false)
        {
            continue;
        }
        else
        {
            completo = true;
        }
    }
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
    cout << "Mejor costo: " << mejor_solucion.costo_camino << " ⏩" << endl;
    cout << "Mejor longitud: " << mejor_solucion.longitud_camino_final << " ⚡" << endl;

    if (show_solucion){
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

void ACO::exportar_solucion(std::chrono::microseconds duration)
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

    std::string resultados = "resultados.txt";
    std::string ruta_archivo_salida = directorio_salida.string() + "/" + resultados;
    std::ofstream archivo_salida(ruta_archivo_salida);
    archivo_salida << "Mejor hormiga: " << mejor_solucion.id << endl;
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
    archivo_salida << "Tiempo de modelo: "
                   << "N/A" << endl;
    archivo_salida << "Tiempo de backtrack: "
                   << "N/A" << endl;

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
     for (auto& par : mejor_solucion.arcos_visitados_tour) {
        Arco *arco = par.first;
        int suma = par.second;

        // Sumar el valor del segundo mapa
        if (mejor_solucion.arcos_visitados_salida.find(arco) != mejor_solucion.arcos_visitados_salida.end()) {
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
        if (hormiga.costo_camino < mejor_costo && (hormiga.longitud_camino_tour + hormiga.longitud_camino_salida) < mejor_longitud)
        {
            mejor_costo = hormiga.costo_camino;
            mejor_longitud = hormiga.longitud_camino_final;
            mejor_hormiga = hormiga;
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
        usarMatrizSalida = false;
        for (auto &par : grafo->arcos)
        {
            hormiga.arcos_visitados_tour[par.second] = 0;
            hormiga.arcos_visitados_salida[par.second] = 0;
        }
        hormiga.longitud_camino_tour = 0;
        hormiga.longitud_camino_salida = 0;
        hormiga.longitud_camino_final = 0;
        hormiga.saltosSalida = 0;
        hormiga.saltosTour = 0;
        hormiga.costo_camino = 0;
        hormiga.feromonas_locales = feromonas;
        hormiga.nodo_actual = &grafo->metadatos.nodos_iniciales[int((generar_numero_aleatorio(0, grafo->metadatos.nodos_iniciales.size() - 1)))];

        // borrar despues, solo debug
        hormiga.saltos_hormiga = 0;
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
    beta = parametros_base.beta;
    rho = parametros_base.rho;
    rho_secundario = parametros_base.rho_secundario;
    rho_salida = parametros_base.rho_salida;
    tau = parametros_base.tau;
    evaluaciones_maximas = parametros_base.evaluaciones_maximas;
    umbral_inferior = parametros_base.umbral_inferior;
    num_hormigas = parametros_base.num_hormigas;
    epocas = parametros_base.epocas;
    usarMatrizSecundaria = parametros_base.usaMatrizSecundaria;
    epoca_actual = 0;
    // calcular numero de evaluaciones en base a la cantidad de iteraciones
    //iteraciones_max = parametros_base.iteraciones_max;
    iteraciones_max = evaluaciones_maximas / num_hormigas;
}

Hormiga ACO::get_mejor_solucion()
{
    return mejor_solucion;
}
