#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream> 
#include <conio.h>

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

    // Parámetros y configuración por defecto
    string nombre_archivo = "Formato5x5.txt";
    int epocas = 3;
    int iteraciones = 1000;
    int hormigas = 10;
    float alfa = 1.0;
    float beta = 3.0;
    float rho = 0.5;
    float tau = 1.0;
    // float q0 = 0.5;

    bool carga_auto = true;
    bool leer_restricciones = false;
    bool leer_coordenadas = false;
    bool debug = true;
    bool debug_ACO = false;

    MetodoACO metodo = MIN_MAX;
    Graph grafo = Graph();
    ACO *aco;

    if (carga_auto)
    {
        grafo = leerInstancia(nombre_archivo, leer_restricciones, leer_coordenadas);
    }
    else
    {
        if (argc != 13)
        {
            //TODO usar boost para poder ingresar los parametros como --algo --iteraciones ...
            cout << "Uso: " << argv[0] << " <nombre_Instancia> <#epocas> <metodo> <#iteraciones> <#hormigas> <alfa> <beta> <rho> <tau> <carga_auto> <leer_restricciones> <debug>" << endl;
            return 1;
        }

        // Carga los parametros pasados por consola
        nombre_archivo = argv[1];
        epocas = atoi(argv[2]);
        metodo = (MetodoACO)atoi(argv[3]);
        iteraciones = atoi(argv[4]);
        hormigas = atoi(argv[5]);
        alfa = atof(argv[6]);
        beta = atof(argv[7]);
        rho = atof(argv[8]);
        tau = atof(argv[9]);
        // q0 = atof(argv[]);

        carga_auto = atoi(argv[10]);
        leer_restricciones = atoi(argv[11]);
        leer_coordenadas = atoi(argv[12]);

        // Intenta abrir el archivo
        ifstream archivo(nombre_archivo);
        if (!archivo.is_open())
        {
            cout << "No se pudo abrir el archivo ❌" << nombre_archivo << endl;
            return 1;
        }
        grafo = leerInstancia(nombre_archivo, leer_restricciones, leer_coordenadas);
    }

    (grafo.vector_arcos.empty()) ? cout << "Error al leer la instancia ❌" << endl : cout << "Instancia leída correctamente ✔️" << endl;
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
    ParametrosAS parametrosAS;
    ParametrosMM parametrosMM;

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
    aco->abrir_file();
    for (aco->epoca_actual; aco->epoca_actual < aco->epocas; aco->epoca_actual++){
        cout <<"⌚"<<"Epoca " << aco->epoca_actual << endl;
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
    for (int i = 0; i < hormigas; i++)
        cout << "🐜 ";
    cout << endl;
    cout << endl;    
    delete aco;
    std::system(comando.c_str()); 
    return 0;
}