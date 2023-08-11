#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream> 
#include <conio.h>

#include "magic_enum/include/magic_enum.hpp"
#include "argparse/argparse.hpp"
   
#include "graph.h"
#include "aco.h"
#include "antsystem.h"
#include "minmax.h"
#include "instance_reader.h"
#include "helpers.h"
#include "enums.h"

using namespace std;

/*
    Implementacion de la metaheuristica ACO para el problema de recoleccion de residuos.
    Autor: Alonso M. Robledo Álamos.
    Fecha: 2023-07-23
    Correo electrónico: alonsorobledo@protonmail.com | alonso.robledo.a@mail.pucv.cl

    Este programa implementa una metaheuristica tipo Ant Colony Optimization (ACO) para resolver un problema de ruteo de arcos (ARP)
    en el contexto de la recoleccion de residuos domiciliarios.
    Esto en el contexto de mi proyecto de tesis para optar al grado de magister en ingenieria informatica.
*/

// Funcion principal
/* Parámetros:
    - nombre_archivo: Nombre del archivo que contiene la instancia
    - iteraciones: Número de iteraciones que realizará el algoritmo
    - hormigas: Número de hormigas que se utilizarán
    - leer_restricciones: Indica si se leerán las restricciones del archivo
    - carga_auto: Indica si se cargará una instancia automática o manual
    - debug: Indica si se mostrarán mensajes de debug
*/
int main(int argc, char *argv[])
{
    bool carga_auto = true;
    bool leer_restricciones = false;
    bool leer_coordenadas = false;
    bool debug = true;
    bool debug_ACO = false;

    MetodoACO metodo = MIN_MAX;
    Graph grafo = Graph();
    ACO *aco;
    ACOArgs parametros_base = argparse::parse<ACOArgs>(argc, argv);    

    grafo = leerInstancia(parametros_base.nombre_instancia, leer_restricciones, leer_coordenadas);
        
    if (debug)
    {
        cout << endl;
        for (int i = 0; i < 161; i++)
            cout << "-";
        cout << endl;
        cout << "Datos de debug 🪲: " << endl;
        cout << endl;
        cout << "Nombre de la instancia: " << grafo.metadatos.encabezado["NOMBRE"] << endl;
        cout << "Comentario: " << grafo.metadatos.encabezado["COMENTARIO"] << endl;
        cout << "Número de nodos: " << grafo.metadatos.encabezado["VERTICES"] << endl;
        cout << "Número de arcos obligatorios: " << grafo.metadatos.encabezado["ARISTAS_REQ"] << endl;
        (grafo.metadatos.encabezado["ARISTAS_NOREQ"] != "0") ? cout << "Número de arcos opcionales: " << grafo.metadatos.encabezado["ARISTAS_NOREQ"] << endl : cout << "No hay arcos opcionales" << endl;
        cout << "Número de nodos iniciales: " << grafo.metadatos.encabezado["NODOS_INICIALES"] << endl;
        (grafo.metadatos.encabezado["NODOS_TERMINO"] != "0") ? cout << "Número de nodos terminales: " << grafo.metadatos.encabezado["NODOS_TERMINO"] << endl : cout << "Todos los nodos pueden ser terminales" << endl;
        cout << "Número de nodos terminales: " << grafo.metadatos.encabezado["NODOS_TERMINO"] << endl;
        (grafo.metadatos.encabezado["RESTRICCIONES"] != "0") ? cout << "Número de restricciones: " << grafo.metadatos.encabezado["RESTRICCIONES"] << endl : cout << "No hay restricciones" << endl;
        cout << endl;
        cout << "Datos: " << endl;
        cout << "Nodos iniciales: " << endl;
        for (auto &nodo : grafo.metadatos.nodos_iniciales)
        {
            cout << nodo.id << " " << endl;
        }
        cout << endl;
        cout << "Nodos terminales: " << endl;
        for (auto &nodo : grafo.metadatos.nodos_termino)
        {
            cout << nodo.id << " ";
        }
        cout << endl;
        cout << "Arcos:" << endl;
        cout << "nodo_origen nodo_destino costo_recorrido costo_recoleccion obligatorio(1|0) bidireccional(1|0)" << endl;
        for (auto &arco : grafo.vector_arcos)
        {
            cout << arco->origen->id << " " << arco->destino->id << " " << arco->costo_recorrido << " " << arco->costo_recoleccion << " " << arco->obligatoria << " " << arco->bidireccional << endl;
        }
        cout << endl;
        cout << "Nodos:" << endl;
        cout << "id posX posY grado" << endl;
        for (auto &par : grafo.nodos)
        {
            cout << par.second.id << " " << par.second.x << " " << par.second.y << " " << par.second.grado << endl;
        }
        for (int i = 0; i < 161; i++)
            cout << "-";
        cout << endl;
    }
    cout << endl;
    
    ASArgs parametrosAS = argparse::parse<ASArgs>(argc, argv);
    MMArgs parametrosMM = argparse::parse<MMArgs>(argc, argv);

    ParametrosAS parametrosAS_param;
    ParametrosMM parametrosMM_param;

    bool argparse_activado = true;    

    if (argparse_activado){
        switch (metodo)
        {
        case ANT_SYSTEM:
            aco = new AntSystem(&grafo, parametrosAS);        
            break;
        case MIN_MAX:
            aco = new MinMax(&grafo, parametrosMM);
            break;
        case ELITIST:
            break;
        case ACS:
            break;
        }
    } else 
    {
        switch (metodo)
        {
        case ANT_SYSTEM:
            aco = new AntSystem(&grafo, parametrosAS_param);        
            break;
        case MIN_MAX:
            aco = new MinMax(&grafo, parametrosMM_param);
            break;
        case ELITIST:
            break;
        case ACS:
            break;
        }
    }
    aco->abrir_file();
    for (aco->epoca_actual; aco->epoca_actual < aco->epocas; aco->epoca_actual++){
        cout <<"⌚"<<"Época " << aco->epoca_actual << endl;
        aco->resolver(); // Llamada al método resolver
        aco->reset();
    }
    aco->cerrar_file();

    // Muestra la solución
    bool show_solucion = true;
    aco->mostrar_solucion(show_solucion);

    std::string archivo_salida = aco->get_filename();
    std::stringstream ss;
    ss << "python Grafico.py " << archivo_salida; 
    std::string comando = ss.str();     
    cout << "Programa finalizado correctamente" << endl;
    for (int i = 0; i < parametros_base.num_hormigas; i++)
        cout << "🐜 ";
    cout << endl;
    cout << endl;    
    delete aco;
    std::system(comando.c_str()); 
    return 0;
}