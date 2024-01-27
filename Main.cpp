#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>

#include <thread>
#include <chrono>

#include "argparse/argparse.hpp"

#include "graph.h"
#include "generador_numeros_aleatorios.h"
#include "aco.h"
#include "antsystem.h"
#include "minmax.h"
#include "AntColonySystem.h"
#include "instance_reader.h"
#include "helpers.h"
#include "enums.h"
#include "test.h"


#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include <string>
#include "handlerJSON.cpp"
using namespace rapidjson;

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
    - iteraciones: Número de itecout << "Iteracion: " << iteraciones << endl;raciones que realizará el algoritmo
    - hormigas: Número de hormigas que se utilizarán
    - leer_restricciones: Indica si se leerán las restricciones del archivo
    - debug: Indica si se mostrarán mensajes de debug
*/
int main(int argc, char *argv[])
{


FILE* fp = fopen("rutas_mas_cortas_chiquito.json", "rb"); 
  
    // Check if the file was opened successfully 
    if (!fp) { 
        std::cerr << "Error: unable to open file"
                  << std::endl; 
        return 1; 
    }     char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    MyHandler handler;
    Reader reader;  
    reader.Parse(is, handler);

    fclose(fp);



    ConfigPrograma config;
    Graph grafo = Graph();
    ACO *aco;
    ACOArgs parametros_base = argparse::parse<ACOArgs>(argc, argv);
    if (parametros_base.full_aleatorio){
        inicializar_generador(std::random_device{}());  
    } else {
        inicializar_generador(parametros_base.semilla);  
    }
    grafo = leerInstancia(parametros_base.nombre_instancia, config.leer_restricciones, config.leer_coordenadas, parametros_base.irace);
    if (config.debug)
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
        for (int i = 0; i < 161; i++)
            cout << "-";
        cout << endl;
    }
    cout << endl;
    switch (parametros_base.metodo)
    {
    case ANT_SYSTEM:
        aco = new AntSystem(&grafo, parametros_base);
        break;
    case MIN_MAX:
        aco = new MinMax(&grafo, parametros_base);
        break;
    case ELITIST:
        break;
    case ACS:
        aco = new AntColonySystem(&grafo, parametros_base);
        break;
    case TEST:
        bfs(grafo.metadatos.nodos_iniciales[0].id, grafo.informacion_heuristica);
    }
    ///////////////////////
    bfs(grafo.metadatos.nodos_iniciales[0].id, grafo.informacion_heuristica);
    //bfs(32308, grafo.informacion_heuristica);
    //////////////////TEST

    aco->abrir_file();
    auto start = std::chrono::high_resolution_clock::now();
    for (aco->epoca_actual; aco->epoca_actual < aco->epocas; aco->epoca_actual++)
    {
        cout << "⌚"<< "Época " << aco->epoca_actual << endl;
        aco->resolver();
        aco->reset();
    }
    auto stop = std::chrono::high_resolution_clock::now(); 
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    aco->cerrar_file();
    aco->mostrar_solucion(config.show_solucion);
    if (parametros_base.crear_directorios_solucion){
        aco->exportar_solucion(duration,parametros_base);
    }
    if (!parametros_base.silence){
        aco->exportar_mapa_resultados();
        std::string archivo_salida = aco->get_filename();

        std::stringstream ss;
        ss << "python Grafico.py " << archivo_salida << " " << config.show_grafico;
        std::string comando = ss.str();

        std::stringstream ss2;
        ss2 << "python Visualizador.py " << parametros_base.nombre_instancia << " " << archivo_salida << " " << config.show_grafico;
        std::string comando2 = ss2.str();
        
        cout << "Programa finalizado correctamente" << endl;
        for (int i = 0; i < parametros_base.num_hormigas; i++)
            cout << "🐜 ";
        cout << endl;
        cout << endl;
        

        std::system(comando.c_str());    
        std::system(comando2.c_str());     
    }


    delete aco;
    return 0;
}
